# gate-controller

A Particle Photon2 project named gate-controller-photon2

This code is based on my gate-controller code which originally used a Particle Photon (first generation) plugged into an NCD2Relay board. The Photon2, however, uses the Feather form factor instead of the form factor of the previous generation Photon and as such won't plug into the NCD2Relay board. It would be possible to adapt it, but it's cheaper and easier to just plug it into the Adafruit non latching relay board for Feather. That means simplified code since it is simply pulling a pin high and low (D2) to control the relay instead of needing to talk to an I/O extender, which is what the NCD2Relay board uses.

Publishes {controller name}_1 on/off when the relay state is changed.

Relay state can be changed by the cloud by any device publishing {controller name}_relay_1 with “1on” or “1off”.

Relay state can be gathered from all gates by publishing "getstate" to particle cloud.

Future commands can be skipped by publishing "skipcommand X" where X is 0-9. For example, if you publish "skipcommand 2" then every gate will skip the next two commands sent to it by particle cloud.

Relay state can also be changed by the Particle app under Functions. (Warning: this does not trigger the firmware to publish any status to particle cloud. It also does NOT count as a "command" as far as skips are concerned.)


## Compiling your project

When you're ready to compile your project, make sure you have the correct Particle device target selected and run `particle compile <platform>` in the CLI or click the Compile button in the Desktop IDE. The following files in your project folder will be sent to the compile service:

- Everything in the `/src` folder, including your `.ino` application file
- The `project.properties` file for your project
- Any libraries stored under `lib/<libraryname>/src`

You then just need to run 'particle flash <device_name>' to flast your compiled code to your Particle Photon2

## Setting up a new Photon 2 on OSX from CLI

Plug into USB on the Mac and then:

particle setup
particle serial wifi
particle identify
particle device add <device_id>
particle device rename <device_id> <new_device_name>
particle flash <new_device_name>
