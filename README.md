# ale-wrapper
The purpose of this repository is to link the TPG [GEGELATI library](https://github.com/gegelati/gegelati) to the [ALE interface](https://github.com/mgbellemare/Arcade-Learning-Environment). 

## How to install ?
First of all, install the [GEGELATI library](https://github.com/gegelati/gegelati). 
The wrapper has not been tested with gegelati v1.3.1 yet but it should work.

To do so, you can execute the following commands :
```
$ git clone --branch v1.2.0 https://github.com/gegelati/gegelati.git
$ cd gegelati/bin
$ cmake ..
$ cmake --build . --target INSTALL # On Windows
$ cmake --build . --target install # On Linux
```

You also need the [ALE library](https://github.com/mgbellemare/Arcade-Learning-Environment).
To install it, you can execute the following commands :
```
$ git clone https://github.com/mgbellemare/Arcade-Learning-Environment.git
$ sudo apt-get install libsdl1.2-dev libsdl-gfx1.2-dev libsdl-image1.2-dev cmake # On Linux
$ cd Arcade-Learning-Environment
$ git reset --hard d3f2b25 # back to the last tested version.
$ mkdir build && cd build
$ cmake -DUSE_SDL=ON -DUSE_RLGLUE=OFF -DBUILD_EXAMPLES=ON ..
$ cmake --build . --target INSTALL # On Windows
$ cmake --build . --target install # On Linux
```

Finally, you can build this project with an IDE of using some commands like :
```
$ git clone https://github.com/gegelati/ale-wrapper.git 
$ cd ale-wrapper/cmake-build-release && cmake ..
$ Release/ALEGegelati
```

**If you need to make it working in debug mode**, you need to edit the Arcade-Learning-Environment/src/CMakeLists.txt and to replace :
```
target_compile_options(ale PUBLIC -O3 -fPIC)
```
by
```
target_compile_options(ale PUBLIC -O0 -fPIC)
```
And
```
target_compile_options(ale
  		PUBLIC
   		 $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wunused -fomit-frame-pointer>
	)
```
by
```
target_compile_options(ale
  		PUBLIC
  		  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wunused>
	)
```

## How does this work ?
The ale-wrapper is built to hide the ALE part, so the main file which handles the library call is really close to the ones written for other Gegelati applications, see [GEGELATI-APPS](https://github.com/gegelati/gegelati-apps) for examples. There is also an example called "learningExample" in this repository.

Warning : the ALE simulation can take a while and with for ex. a configuration of 1000 roots, you'll do at least 1000 simulations. Be aware it could take a lot of time, like 1000s per generation.

## License
This project is distributed under the CeCILL-C license (see LICENSE file).
