# OpenLTTO
The OpenLTTO Project is an attempt to recreate the Lazer Tag Team Ops protocol, and game system, in a completely open-source solution. Eventually our intent, is to expand on the LTTO system, adding new functionality, and features, while maintaining backwards compatibility with this classic Lazer Tag gear.

> This Project wouldn't be possible without all the hard work of Richie Mickan https://github.com/rmick
> His work on recreating the LTTO Protocol, and the development of the Combobulator app are core to the firmware and capabilities of this project.

> Additionally this is built on top of the excellent product developed and released by Hasbro/Tiger Electronics. And the work done by Tag Ferret to release the technical specs, and unlock much of the magic of the protocol, and the inner workings.

We intend to do this via a collection of open-source hardware components that can act as building blocks, to build anything you might require for a Lazer Tag game (Taggers, Hosting Pods, Zone controllers, utilities, etc). And an opensource firmware to operate it all. 

# OpenLTTO Firmware

This Firmware Project is still a Work In Progress! It is not fully functional yet. 

More info coming soon!

## Development

This project is developed on top of ESP-IDF 5.1.1
The repo has submodules, so you will need to update those as well with `git submodule update --remote --init --recursive` before attempting to build.
The UI is developed using Squareline Studio, and exported into the appropriate subfolder in the UI component.

More detailed instructions coming soon.
