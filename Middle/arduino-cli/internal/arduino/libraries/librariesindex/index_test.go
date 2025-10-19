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

package librariesindex

import (
	json "encoding/json"
	"fmt"
	"testing"

	"github.com/arduino/arduino-cli/internal/arduino/libraries"
	"github.com/arduino/go-paths-helper"
	easyjson "github.com/mailru/easyjson"
	"github.com/stretchr/testify/require"
	semver "go.bug.st/relaxed-semver"
)

func TestIndexer(t *testing.T) {
	fail1, err := LoadIndex(paths.New("testdata/inexistent"))
	require.Error(t, err)
	require.Nil(t, fail1)

	fail2, err := LoadIndex(paths.New("testdata/invalid.json"))
	require.Error(t, err)
	require.Nil(t, fail2)

	index, err := LoadIndex(paths.New("testdata/library_index.json"))
	require.NoError(t, err)
	require.Equal(t, 4124, len(index.Libraries), "parsed libraries count")

	alp := index.Libraries["Arduino Low Power"]
	require.NotNil(t, alp)
	require.Equal(t, 5, len(alp.Releases))
	require.Equal(t, "Arduino Low Power@1.2.2", alp.Latest.String())
	require.Len(t, alp.Latest.Dependencies, 1)
	require.Equal(t, "RTCZero", alp.Latest.Dependencies[0].GetName())
	require.Equal(t, "", alp.Latest.Dependencies[0].GetConstraint().String())
	require.Equal(t, "[1.0.0 1.1.0 1.2.0 1.2.1 1.2.2]", fmt.Sprintf("%v", alp.Versions()))

	rtc100, err := index.FindRelease("RTCZero", semver.MustParse("1.0.0"))
	require.NoError(t, err)
	require.NotNil(t, rtc100)
	require.Equal(t, "RTCZero@1.0.0", rtc100.String())

	rtcLatest, err := index.FindRelease("RTCZero", nil)
	require.NoError(t, err)
	require.NotNil(t, rtcLatest)
	require.Equal(t, "RTCZero@1.6.0", rtcLatest.String())

	rtcInexistent, err := index.FindRelease("RTCZero", semver.MustParse("0.0.0-blah"))
	require.Error(t, err)
	require.Nil(t, rtcInexistent)

	rtcInexistent, err = index.FindRelease("RTCZero-blah", nil)
	require.Error(t, err)
	require.Nil(t, rtcInexistent)

	rtc := index.FindIndexedLibrary(&libraries.Library{Name: "RTCZero"})
	require.NotNil(t, rtc)
	require.Equal(t, "RTCZero", rtc.Name)

	rtcUpdate := index.FindLibraryUpdate(&libraries.Library{Name: "RTCZero", Version: semver.MustParse("1.0.0")})
	require.NotNil(t, rtcUpdate)
	require.Equal(t, "RTCZero@1.6.0", rtcUpdate.String())

	rtcUpdateNoVersion := index.FindLibraryUpdate(&libraries.Library{Name: "RTCZero", Version: nil})
	require.NotNil(t, rtcUpdateNoVersion)
	require.Equal(t, "RTCZero@1.6.0", rtcUpdateNoVersion.String())

	rtcNoUpdate := index.FindLibraryUpdate(&libraries.Library{Name: "RTCZero", Version: semver.MustParse("3.0.0")})
	require.Nil(t, rtcNoUpdate)

	rtcInexistent2 := index.FindLibraryUpdate(&libraries.Library{Name: "RTCZero-blah", Version: semver.MustParse("1.0.0")})
	require.Nil(t, rtcInexistent2)

	resolve1 := index.ResolveDependencies(alp.Releases["1.2.1"], nil)
	require.Len(t, resolve1, 2)
	require.Contains(t, resolve1, alp.Releases["1.2.1"])
	require.Contains(t, resolve1, rtc.Releases["1.6.0"])

	oauth010, err := index.FindRelease("Arduino_OAuth", semver.MustParse("0.1.0"))
	require.NoError(t, err)
	require.NotNil(t, oauth010)
	require.Equal(t, "Arduino_OAuth@0.1.0", oauth010.String())
	eccx135, err := index.FindRelease("ArduinoECCX08", semver.MustParse("1.3.5"))
	require.NoError(t, err)
	require.NotNil(t, eccx135)
	require.Equal(t, "ArduinoECCX08@1.3.5", eccx135.String())
	bear172, err := index.FindRelease("ArduinoBearSSL", semver.MustParse("1.7.2"))
	require.NoError(t, err)
	require.NotNil(t, bear172)
	require.Equal(t, "ArduinoBearSSL@1.7.2", bear172.String())
	http040, err := index.FindRelease("ArduinoHttpClient", semver.MustParse("0.4.0"))
	require.NoError(t, err)
	require.NotNil(t, http040)
	require.Equal(t, "ArduinoHttpClient@0.4.0", http040.String())

	resolve2 := index.ResolveDependencies(oauth010, nil)
	require.Len(t, resolve2, 4)
	require.Contains(t, resolve2, oauth010)
	require.Contains(t, resolve2, eccx135)
	require.Contains(t, resolve2, bear172)
	require.Contains(t, resolve2, http040)
}

func BenchmarkIndexParsingStdJSON(b *testing.B) {
	indexFile := paths.New("testdata/library_index.json")
	buff, err := indexFile.ReadFile()
	require.NoError(b, err)
	b.SetBytes(int64(len(buff)))
	for i := 0; i < b.N; i++ {
		var i indexJSON
		err = json.Unmarshal(buff, &i)
		require.NoError(b, err)
	}
}

func BenchmarkIndexParsingEasyJSON(b *testing.B) {
	indexFile := paths.New("testdata/library_index.json")
	buff, err := indexFile.ReadFile()
	require.NoError(b, err)
	b.SetBytes(int64(len(buff)))
	for i := 0; i < b.N; i++ {
		var i indexJSON
		err = easyjson.Unmarshal(buff, &i)
		require.NoError(b, err)
	}
}
