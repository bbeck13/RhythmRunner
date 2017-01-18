Rhythm Runner
=============
- [About](#about)
- [Authors](#authors)
- [Requirements](#requirements)
   - [Mac OS X](#mac-os-x)
   - [Unix](#unix)
- [Playing](#playing)

About
-----
Rhythm Runner is a graphics project for Zoe Wood's [CPE 476 class](http://users.csc.calpoly.edu/~zwood/teaching/csc476/index.html)
Authors
-------
- Braden Beck (bbeck13, bnbeck)

Requirements
------------
You will need to install:

- [Aquila](http://aquila-dsp.org/download/)
- [SFML](https://www.sfml-dev.org/download.php)
- [GLFW](http://www.glfw.org/)
- [GLM](http://glm.g-truc.net/0.9.8/index.html)
- [GLEW](http://glew.sourceforge.net/)

### Mac OS X:
[Brew](http://brew.sh) can install most of these except for Aquila. When installing
Aquila install it to /usr/local as that is where the CMakeLists.txt will look for it.
### Unix:
Everything should be available from your package manager or you can install it from source
if [you're into that sort of thing](https://stallman.org/).

If you don't have the most recent version of cmake either get it or you may need to
download some modules in order to find you dependencies. I only had to do this with SFML
[here's the modules you will need](https://github.com/SFML/SFML/tree/master/cmake/Modules).
Place them in your ../cmake/Modules dir and you should be able to compile the program.

Playing
-------
Coming soon!
