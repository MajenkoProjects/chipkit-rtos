chipKIT RTOS
============

This is both an attempt to build a new PIC32 Arduino-compatible core with FreeRTOS support, and an attempt to build, from the ground up,
an entire new PIC32 SDK based around FreeRTOS.

At the moment only the MZ chips are supported, but the plan is to expand it to all other PIC32 chips in time.

----

How I see this working
----------------------

* Gradually implement each piece of interfacing code in the core Arduino API whilst coding the actual hardware functions in the SDK
  * For example: writing `digitalRead()` first entails writing code in the SDK to actually control and read the GPIOs
* Rewrite each library to only use pure SDK calls whilst at the same time writing the code in the SDK to support those calls
  * For example: the SPI library should only call functions in the SDK and not use any registers.

Eventually the SDK will become fully featured. If any user requires some functionality for a library that doesn't exist in the SDK
the SDK must be enhanced to provide that functionality. It is *not* OK for the user to use direct register access or implement
interryupts in their own code.

Once all the internal API hardware abstraction is complete the veneer of the rest of the API can be overlaid (things like String and Print, etc).
