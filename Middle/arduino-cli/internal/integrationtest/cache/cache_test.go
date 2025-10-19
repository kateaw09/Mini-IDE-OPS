// This file is part of arduino-cli.
//
// Copyright 2022 ARDUINO SA (http://www.arduino.cc/)
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

package cache_test

import (
	"testing"

	"github.com/arduino/arduino-cli/internal/integrationtest"
	"github.com/stretchr/testify/require"
)

func TestCacheClean(t *testing.T) {
	// This test should not use shared download directory because it will be cleaned up with 'cache clean' command
	env := integrationtest.NewEnvironment(t)
	cli := integrationtest.NewArduinoCliWithinEnvironment(env, &integrationtest.ArduinoCLIConfig{
		ArduinoCLIPath: integrationtest.FindArduinoCLIPath(t),
	})
	defer env.CleanUp()

	_, _, err := cli.Run("cache", "clean")
	require.NoError(t, err)

	// Generate /staging directory
	_, _, err = cli.Run("lib", "list")
	require.NoError(t, err)

	_, _, err = cli.Run("cache", "clean")
	require.NoError(t, err)

	staging := cli.DataDir().Join("staging")
	require.False(t, staging.IsDir())
}
