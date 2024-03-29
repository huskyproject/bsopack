# bsopack

BSOpack 1.9
===========

## 1. Introduction

BSOpack is used to pack outgoing netmail for links. It works with
HUSKY package and requires its files. You must use fidoconfig 1.9-current.
If you don't have these packages, download them from
[Fidoconf repository](https://github.com/huskyproject/fidoconf)


## 2. FidoConfig keywords and configuration

a) packNetmail <on/off>
- Determines if you want to pack outgoing netmail for this link.
- Used only in link definition.

b) maxUnpackedNetmail <size in kb>
- Determines how much unpacked netmail we should leave for this link.
- If we have more outgoing netmail - it will be packed.
- You also can have packed & unpacked netmail in your outbound
simultaneously - packet, which had had size more than maxUnpackedNetmail
was packed into bundle and new packed with size less than
maxUnpackedNetmail was created.
- If you select 0 kb - all netmail will be packed.

Attention: If you use 'packNetmail on' - you *must* define a packer for this link.

Example:
```
Link            Dmitry Sergienko
AKA             2:464/910
ourAka          2:464/910.1
Password        xxxxxxxx
LinkGrp         D
AccessGrp       D
Level           200
autoAreaCreate  on
autoFileCreate  on
forwardRequests on
AutoAreaCreateDefaults -lr 100 -lw 150 -g A -p 14 -dupeCheck move -dupeHistory 11
Packer          zip
allowEmptyPktPwd on
packNetmail on
maxUnpackedNetmail 0
```

## 3. Command line switches

Usage: bsopack [-c fidconfig] [options]
Options:
```
        -q            Quiet mode (ignoring screenloglevels in fidoconfig)
        -d            Debud mode
        -h or --help  Help screen
```

You can type several switches in one line like this: -qdc /etc/fido/config


## 4. How this program works

At first it checks if packNetmail for the link is on, if this link is not busy
and if a packer is defined. Then it searches for *.?ut files in the outbound.
If the size of all found *.?ut is greater than maxUnpackedNetmail, these files
will be renamed to *.pkt (using tempOutbound token) and put into an arcmail
bundle. So, your mail should never be lost. If any error occurs hpt
scan will find this packet and (if this packet is for a link listed in
fidoconfig) pack it into a bundle with echomail. The packet will be added to
the most recently used bundle until the size of the bundle is greater than
'arcmailSize' for the link. If you select 'bundleNameStyle addrDiff' your
netmail will be packed together with echomail. Each flavour is processed
independently.  
It creates bundle names using an algorithm of two addresses differences.  
The log file 'bsopack.log' is placed into 'logFileDir'.

This program now supports OS/2, Win32 and Linux.  
You are welcome to port this program to other platforms

Some parts of code were used & modified from hpt.

Thanx to Kees Bergwerf for OS/2 porting  
Thanx to Oleg W Levchenko (2:6035/6) for testing  
Thanx to Max Chernogor for MSVC compiler support

Regards,  
Dmitry Sergienko
