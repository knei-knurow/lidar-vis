# Lidar Visualization

Easily visualize point cloud data (e.g from LIDAR).

## Table of contents

- [Gallery](#gallery)
- [About RPLIDAR](#about-rplidar)
- [Binaries](#binaries)
- [Compilation](#compilation)
  - [Linux, MacOS](#linux-macos)
  - [Windows (Visual Studio)](#windows-visual-studio)
- [Usage](#usage)
  - [Options](#options)
  - [Scenarios](#senarios)
  - [GUIs](#guis)
  - [RPLIDAR Modes](#rplidar-modes)
- [Datasets](#datasets)
  - [Point cloud](#point-cloud)
  - [Point cloud series](#point-cloud-series)
- [RPLIDAR with STM32](#rplidar-with-stm32)
- [Thanks](#thanks)

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

3. Run

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

### Scenarios

Scenarios are sets of actions which are executed just after grabbing the cloud data, and
just before its visualization by the GUI.

```
0    Do nothing, just grab a cloud and visualize (default).
1    Save each cloud as a part of cloud series.
2    Save each cloud as a new screenshot (extremely unoptimized).
```

## Datasets

This software can visualize 2 types of point cloud data – **point cloud** and **point cloud series**.
Both are very straightforward and can be modified with a basic text editor, but usually they'll be _huge_.

### Single point cloud

A single file contains data of a single point cloud (e.g. a full 360° scan, combined scan). Each
line (except for comments, which start with a `#`) represents a single point which
consists of an **angle value [°]** and a **distance value [mm]**. Both may be a floating
point number, and have to be separated by any whitespace character.

**Example**:

```
# A comment
# Angle [°]   Distance [mm]
90.0    42.0
180.0   1000
270.0   1920.11
360.0   2002.0
```

**Preview**:

```
lidar -f datasets/example.txt
```

![doc/screenshots/example.gif](doc/screenshots/example.jpg)

### Point cloud series

A single file contain a list of point clouds. This variant is be used to represent a series of captured
clouds. The file format is the same as in **single point cloud**, but there are some special lines
starting with `!` which separates two point clouds. Each line marked with `!` must consist
of the **ID number of the following point cloud** and **number of milliseconds elapsed from
grabbing the previous one**. Clouds should be sorted by their ID number.

Example:

```
# A comment
# ! ID Number   Elapsed time [ms]
# Angle [°]   Distance [mm]
! 0 0
120  100
240  100
360  100
! 1 500
120  200
240  200
360  200
! 2 500
120  300
240  300
360  300
! 3 500
```

Preview:

```
lidar -fs datasets/example-series.txt
```

![doc/screenshots/example.gif](doc/screenshots/example-series.gif)

## See also

We encourage you to follow ours friend's project in which he combined lidar technology with
a portable STM32 microcontroller. He created a similar visualization software, but running on a
completely different low-level platform with extremely constrained resoures.

**GitHub**: [https://github.com/knei-knurow/lidar-stm32](https://github.com/knei-knurow/lidar-stm32)

<img src="doc/imgs/stm32.jpg" width=500>

## Thanks

This project is developed within Electronics and Computer Science Club in Knurów
(KNEI for short) where tons of amazing projects and ideas are born. Take a look at
our website - [https://knei.pl/](https://knei.pl/) - unfortunately, at the moment,
only available in Polish. Check out our GitHub too - https://github.com/knei-knurow.

Numerous packages with colorful electronic gadgets like RPLIDAR have been granted to
us by our friends from [KAMAMI.pl](http://kamami.pl).
