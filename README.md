
# Light Controller

This application lets you control MiLight (and equivalent) LED bulbs and LED strips controlled by a 2.4GHz WiFi Bridge.

## Features:
  1. Select a color for each zone independently, or through the master (Zone 0)
  2. Master-Master controller for Multiple Bridges (although I have only one currently, so I have not tested this)
  3. Audio triggered effects
  4. Fading

## Get sources
```
  apt-get install git
  git clone https://github.com/bakkerr/lightcontroller.git
```

## Build:

```
  cd lightcontroller
  apt-get install qt5-default qtmultimedia5-dev libfftw3-dev
  qmake
  make
```

## Windows Pre-compiled Binary:
28-05-2015: https://github.com/bakkerr/lightcontroller/releases/download/Windows_Binary_0.1/lightcontroller.exe


## Bugs and Feature ideas:

Feel free to contact me :)
 


