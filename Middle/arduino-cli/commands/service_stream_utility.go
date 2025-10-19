// This file is part of arduino-cli.
//
// Copyright 2020 ARDUINO SA (http://www.arduino.cc/)
//
// This software is released under the GNU General Public License version 3,
// which covers the main part of arduino-cli.
// The terms of this license can be found at:
// https://www.gnu.org/licenses/gpl-3.0.en.html
//
// You can be released from the requirements of the above licenses by purchasing
// a commercial license. Buying such a license is mandatory if you want to
// modify or otherwise use the software for commercial activities involving the
// Arduino software without disclosing the source code of your own applications.
// To purchase a commercial license, send an email to license@arduino.cc.

package commands

import (
	"errors"
	"io"
	"sync"
	"time"

	"github.com/djherbis/buffer"
	"github.com/djherbis/nio/v3"
)

// implWriteCloser is an helper struct to implement an anonymous io.WriteCloser
type implWriteCloser struct {
	write func(buff []byte) (int, error)
	close func() error
}

func (w *implWriteCloser) Write(buff []byte) (int, error) {
	return w.write(buff)
}

func (w *implWriteCloser) Close() error {
	return w.close()
}

// feedStreamTo creates a pipe to pass data to the writer function.
// feedStreamTo returns the io.WriteCloser side of the pipe, on which the user can write data.
// The user must call Close() on the returned io.WriteCloser to release all the resources.
// If needed, the context can be used to detect when all the data has been processed after
// closing the writer.
func feedStreamTo(writer func(data []byte)) io.WriteCloser {
	r, w := nio.Pipe(buffer.New(32 * 1024))
	var wg sync.WaitGroup
	wg.Add(1)
	go func() {
		defer wg.Done()
		data := make([]byte, 16384)
		for {
			if n, err := r.Read(data); err == nil {
				writer(data[:n])

				// Rate limit the number of outgoing gRPC messages
				// (less messages with biggger data blocks)
				if n < len(data) {
					time.Sleep(50 * time.Millisecond)
				}
			} else {
				r.Close()
				return
			}
		}
	}()
	return &implWriteCloser{
		write: w.Write,
		close: func() error {
			if err := w.Close(); err != nil {
				return err
			}
			wg.Wait()
			return nil
		},
	}
}

// consumeStreamFrom creates a pipe to consume data from the reader function.
// consumeStreamFrom returns the io.Reader side of the pipe, which the user can use to consume the data
func consumeStreamFrom(reader func() ([]byte, error)) io.Reader {
	r, w := io.Pipe()
	go func() {
		for {
			if data, err := reader(); err != nil {
				if errors.Is(err, io.EOF) {
					w.Close()
				} else {
					w.CloseWithError(err)
				}
				return
			} else if _, err := w.Write(data); err != nil {
				w.Close()
				return
			}
		}
	}()
	return r
}

// SynchronizedSender is a sender function with an extra protection for
// concurrent writes, if multiple threads call the Send method they will
// be blocked and serialized.
type SynchronizedSender[T any] struct {
	lock          sync.Mutex
	protectedSend func(T) error
}

// Send the message using the underlyng stream.
func (s *SynchronizedSender[T]) Send(value T) error {
	s.lock.Lock()
	err := s.protectedSend(value)
	s.lock.Unlock()
	return err
}

// NewSynchronizedSend takes a Send function and wraps it in a SynchronizedSender
func NewSynchronizedSend[T any](send func(T) error) *SynchronizedSender[T] {
	return &SynchronizedSender[T]{
		protectedSend: send,
	}
}
