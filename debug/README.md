kati debug
==========

kati debug is an extension for kati that enable debugging feature addition to kati.

How to use for debug
----------------------

Set environment variables to enable debugging

    % export KATI_DEBUG_FLAG=true
    % export KATI_DEBUG_CLIENT="tcp:8234"

Currently, only TCP protocol is supported. We will implement more like udp, unix socket or pipe.

Start android build as usual. Build Android:

    % cd <android-directory>
    % source build/envsetup.sh
    % lunch <your-choice>
    % m -j4

You will see prompt as below

    % Waiting for connection
    
And start a new console to start telnet to connect to debugger

    % telnet localhost 8234
    Trying localhost...
    Connected to localhost.
    Escape character is '^]'.
    <current>build/core/main.mk:3
    <end>

Supported Commands
-------------------

### cont c
To continue paused building. Command will return immediately.

     cont
     <cont>OK
     <end>

### break
To break current building. Command will return after building is actually paused

     break
     <current>device/generic/arm64/AndroidProducts.mk:17
     <end>
     <break>OK
     <end>

### bp b
To set break point at specified position. Command will return immediately.
Debugger will be paused when the breakpoint is hit.

     bp device/generic/arm64/AndroidProducts.mk 17
     <bp>OK
     <end>

### bl
To list all break points. Command will return immediately with break point list in response.

     bl
     <bl><1>device/generic/arm64/AndroidProducts.mk:17
     <bl><2>device/generic/arm64/AndroidProducts.mk:33
     <bl><3>device/generic/arm64/Android.mk:23
     <end>

### br d
To delete one break point or all. Command will return immediately.

     bl
     <bl><1>device/generic/arm64/AndroidProducts.mk:17
     <bl><2>device/generic/arm64/AndroidProducts.mk:33
     <bl><3>device/generic/arm64/Android.mk:23
     <end>
     br 1
     <br>OK
     <end>
     bl
     <bl><1>device/generic/arm64/AndroidProducts.mk:33
     <bl><2>device/generic/arm64/Android.mk:23
     <end>
     br
     <br>OK
     <end>
     bl
     <end>

### next n
To run to next stop. Command will return when next position is hitting.

     n
     <current>./art/CleanSpec.mk:48
     <end>
     <step>OK
     <end>


### backtrace bt
To list current file stack.

     bt
     <bt><1>device/generic/arm64/AndroidProducts.mk:17
     <bt><2>build/core/product_config.mk:171
     <bt><3>build/core/envsetup.mk:202
     <bt><4>build/core/config.mk:180
     <bt><5>build/core/main.mk:12
     <end>


To be continue
==============
