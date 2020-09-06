XMOS startKIT Home DSP
==============================

This project realizes a DSP routine on XMOS startKIT hardware and is built on top / adapted from the XMOS sliceKIT DSP audio demo, but reduced in peripherals complexity and equipped with own DSP functions.

At the moment, the DSP processes 6dB Gain, heavy Bass EQ, and decent Treble EQ. Postprocessing limiter prevents from integer overflows / hard clipping. You can change the processing behavior in cppdsp.h / cppdsp.cpp

Build
-----

Run the build with xmake (e.g. build via XTime Composer Studio IDE) without any errors.

Tested development toolchains:

- XTime Composer Studio 14.4.1 with related xmake.

External Dependencies
---------------------

lib_i2c, lib_i2s, lib_gpio, lib_logging, lib_startkit_support, lib_xassert

License
---------------------

The licenses vary depending on the file, since this project is based on the DSP audio demo of XMOS, which reserves all rights. Feel free to use the DSP functions as defined by the MIT license. See end of file or file header for specific licenses per file.