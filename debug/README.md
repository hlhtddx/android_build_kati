kati debug
==========

kati debug is an extension for kati that enable debugging feature addition to kati.

How to build and install kati
----------------------

Change to kati source directory. There are 2 ways to build kati: cmake and make

## CMake (Preferred)
    $ mkdir build && cd build
    $ cmake -DCMAKE_BUILD_TYPE=<Build_Type> .. # Build_Type=Release or Debug
    $ make
target executable is located in **${kati_dir}/build/ckati**

## make
    $ make
target executable is located in **${kati_dir}/ckati**

## install kati to Android Build Environment    
    $ ln -sf ${TARGET_PATH}/ckati $ANDROID_TOP/prebuilts/build-tools/${HOST_OS}_x86/bin


How to use for debug
----------------------

Set environment variables to enable debugging

    $ export KATI_DEBUG_FLAG=true
    $ export KATI_DEBUG_CLIENT="tcp:8234" # 8234 is the port used for listening

Currently, only TCP protocol is supported. We will implement more like udp, unix socket or pipe.

Start android build as usual. Build Android:

    $ cd <android-directory>
    $ source build/envsetup.sh
    $ lunch <your-choice>
    $ m -j4

You will see prompt as below

    $ Waiting for connection

And start a new console to start telnet to connect to debugger

    $ telnet localhost 8234 # 8234 is the port we used to launch kati
    Trying localhost...
    Connected to localhost.
    Escape character is '^]'.
    <current>build/core/main.mk:3
    <end>

Supported Commands
-------------------

## cont
### alias: c
To continue paused building. Command will return immediately.
### example:

command:
     
     cont
 
response:
 
     <cont>OK
     <end>

## break
To break current building. Command will return after building is actually paused

### example:

command:
     
     break
 
response:
 
     <current>device/generic/arm64/AndroidProducts.mk:17
     <end>
     <break>OK
     <end>

## bp
### alias: b
To set break point at specified position. Command will return immediately.
Debugger will be paused when the breakpoint is hit.

### example:

command:
     
     bp device/generic/arm64/AndroidProducts.mk 17
 
response:
 
     <bp>OK
     <end>

## bl
To list all break points. Command will return immediately with break point list in response.

### example:

command:
     
     bl
 
response:
 
     <bl><1>device/generic/arm64/AndroidProducts.mk:17
     <bl><2>device/generic/arm64/AndroidProducts.mk:33
     <bl><3>device/generic/arm64/Android.mk:23
     <end>

## br
### alias: d
To delete one break point or all. Command will return immediately.

### example:
command:
     
     bl
 
response:
 
     <bl><1>device/generic/arm64/AndroidProducts.mk:17
     <bl><2>device/generic/arm64/AndroidProducts.mk:33
     <bl><3>device/generic/arm64/Android.mk:23
     <end>

command:
     
     br 1
 
response:
 
     <br>OK
     <end>

command:
     
     bl
 
response:
 
     <bl><1>device/generic/arm64/AndroidProducts.mk:33
     <bl><2>device/generic/arm64/Android.mk:23
     <end>

command:
     
     br
 
response:
 
     <br>OK
     <end>

command:
     
     bl
 
response:
 
     <end>

## next
### alias: n
To run to next stop. Command will return when next position is hitting.


command:
     
     n
 
response:
 
     <current>./art/CleanSpec.mk:48
     <end>
     <step>OK
     <end>


## backtrace
### alias: bt
To list current file stack.


command:
     
     bt
 
response:
 
     <bt><1>device/generic/arm64/AndroidProducts.mk:17
     <bt><2>build/core/product_config.mk:171
     <bt><3>build/core/envsetup.mk:202
     <bt><4>build/core/config.mk:180
     <bt><5>build/core/main.mk:12
     <end>


To be continue
==============
TODO
====
1. Implement more commands(display variable, rules and commands, etc.)
2. Implement plugin for vscode to integrate is into IDE
3. Currently, we cannot setp into function because ckati doesn't support that. This should be changed in future.
