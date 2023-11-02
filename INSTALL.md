# Instruction to build bsopack

Please read the files (especially README.Makefiles) in the huskybse package FIRST!

## Table of Contents
- [Prebuild](#prebuild)
- [Build](#build)
  - [Compiling and installing with the standard Makefile and huskymak.cfg](#compiling-and-installing-with-the-standard-makefile-and-huskymakcfg)
  - [Compiling with the Legacy Makefiles](#compiling-with-the-legacy-makefiles)
- [Afterbuild actions](#afterbuild-actions)

## Prebuild

- Put the bsopack package in the directory where the other packages of fido husky reside:

  - unix, beos, possible cygwin:
      ```text
      /usr/src/packages/        -> huskybse/
                                -> huskylib/
                                -> smapi/
                                -> fidoconfig/
                                -> hpt/
                                -> htick/
                                ...some other
      ```
   - windows, dos, os/2 & etc:
      ```text
         d:\husky\              -> huskybse\
                                -> huskylib\
                                -> smapi\
                                -> fidoconf\
                                -> hpt\
                                -> htick\
                                ...some other
      ```
## Build

### Compiling and installing with the standard Makefile and huskymak.cfg

See huskybse/INSTALL.asciidoc

### Compiling with the Legacy Makefiles

_This functionality hasn't been tested for years, so be ready that some changes in makefiles might be required_

unix:
```sh
   $ make -f make/makefile.lnx
   $ make -f make/makefile.lnx install
```
dos:
```sh
   d:\husky\bsopack>make -f make\makefile.djg
```

## Afterbuild actions

- (For UNIXes only) Ensure /usr/local/lib/ is in /etc/ld.so.conf
- (For UNIXes only) Execute ldconfig as root

You're ready. Now you can install software which uses bsopack.
