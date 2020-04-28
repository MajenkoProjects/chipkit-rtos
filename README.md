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

Plans for the future
--------------------

* Integrate lwIP into the SDK
* Removal of all `pic32-gcc` specific macros and register names (to allow compilation by `mipsel-unknown-elf-gcc`)
* Expand to MX, MK and MM chips

Contributing
------------

Contributions are more than welcome. This is shaping up to be a big project, so the more people helping out the better. So a couple
of words on style and layout:

* Indent 4 spaces (tabs expanded to spaces)
* Keep drivers if possible to one file per peripheral. 
* No camelcase in function names. Use underscores to separate words.
* Prefix functions with the driver they are associated with.
* No C++
* At the moment Doxygen is being used to generate documentation, so document your work in your source (not the header unless you have to).

In short: stick to the existing style and we'll all get on well.
