# Instruction to build bsopack

Please read the files (especially README.Makefiles) in the huskybse package FIRST!

# Unix:

#### 0. Packages "huskylib" "smapi" and "fidoconfig" are required for bsopack.  
If you don't have them, please download these packages from
[Husky project home page](https://github.com/huskyproject/)
You need fidoconfig at least version 1.9
Compile, install and configure them.

#### 1. Unpack bsopack package to make following source tree:
```
	./huskylib/
	./smapi/
	./fidoconfig/
	./bsopack/
	./huskymak.cfg
```

#### 2. Compile and install it:
```
	$ make
	$ make install
```

Read README.md and you are ready to use bsopack.
