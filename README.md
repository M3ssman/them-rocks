# them-rocks-open
A 2D asteroids clone using SDL framework.   
Originally developed on Ubuntu 12.04 using SDL Media Library V1.2 back in 2012.  
More Information on SDL, the "Simple Direct Media Layer", can be found on the [official SDL Website][1] or the [un-official GiHub Mirror][2].

## Installation on Ubuntu 16.04 LTS
### Prerequisites
* GCC to compile plain C-Files
* Development Libs for SDL

Be sure to have the proper Version of SDL with the Development Libraries on your local Machine.

Otherwise, open a Terminal and type as admin: 
```bash
apt-get install libsdl1.2-dev
```
to install your local Development Headers and Libraries for the latest SDL 1.2.

### Compilation
To create an Executable, go to the root-Folder and type:
```bash
<project-root>$ gcc -o them-rocks src/main.c `sdl-config --cflags --libs`
``` 
where "-o" stands for the final Name of the Executable, so you're to change this.  
The part in between the Backticks dynamically evaluates the necessary Header Files and Libraries
that SDL needs to sucessully compile. It's a shorthand Notation to something like
```bash
<project-root>$ gcc -o t src/main.c -I/usr/include/SDL -L/usr/lib/x86_64-linux-gnu -lSDL
```

Otherwise, feel free to import the Project Folder into your IDE of choice (ex., [Eclipse Neon CDT][3]). Be sure to set Includes and Link-Paths appropriate and start the Build.

Run the compiled Binary afterwards with
```bash
<project-root>$ ./them-rocks
```
and watch up for the Folder "Data" that contains the Game Data.

## Acknowledgment
Inspired by "C++ für Spieleprogrammierer" by Heiko Kallista, and ported to plain C99 for Lecture Purposes back in 2009.

[1]: https://www.libsdl.org/
[2]: https://github.com/spurious/SDL-mirror
[3]: http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/neon1a