# Lidar Visualization

Easily visualize point cloud data (e.g from LIDAR).

## Table of contents

- [Gallery](#gallery)
- [Binaries](#binaries)
- [Compilation](#compilation)
  - [Linux, MacOS](#linux-macos)
  - [Windows (Visual Studio)](#windows-visual-studio)
- [Usage](#usage)


## Gallery

Indoor scan

![doc/screenshots/garden.gif](doc/screenshots/room.gif)

Outdoor scan (passing cars visible)

![doc/screenshots/cars.gif](doc/screenshots/cars.gif)

Outdoor scan (lots of trees and shrubs around)

![doc/screenshots/garden.gif](doc/screenshots/garden.gif)

## Binaries

We're still figuring out the automation process, so if you want
to run the program, you have to compile it yourself.

## Compilation

### Linux, MacOS

1. Install SFML:

   ```
   ./install_sfml
   ```

2. Build:

   ```
   make
   ```

3. Run.

### Windows (Visual Studio)

coming soon (or never)

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
