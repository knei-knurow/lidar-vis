# Lidar Visualization

Easily visualize point cloud data (e.g from LIDAR).

## Table of contents

- [Gallery](#gallery)
- [Usage](#usage)
- [Compilation](#compilation)
  - [Linux, MacOS](#linux-macos)
  - [Windows (Visual Studio)](#windows-visual-studio)

## Gallery

Indoor scan

![doc/screenshots/garden.gif](doc/screenshots/room.gif)

Outdoor scan (passing cars visible)

![doc/screenshots/cars.gif](doc/screenshots/cars.gif)

Outdoor scan (lots of trees and shrubs around)

![doc/screenshots/garden.gif](doc/screenshots/garden.gif)

## Usage

After successful compilation and linking, you will have a `lidar-vis` executable.
You can control its behavior by passing command-line flags.

For example, to visualize a single cloud scan:

`$ cat cloud.txt | lidar-vis`

To visualize only the first 200 points of a single cloud scan:

`$ head -n 30 cloud.txt | lidar-vis`

To visualize a cloud series scan:

`$ cat cloud_series.txt | lidar-vis --series`

To learn more about all available options and keyboard shortcuts:

`$ lidar-vis --help`

## Compilation

### Linux, macOS

1. Install SFML:

   `$ ./install_sfml`

2. Build:

   `$ make`

3. Run:

   `$ ./lidar-vis`

4. Install (Optional):

   `$ make install`

### Windows (Visual Studio)

coming soon (or never)
