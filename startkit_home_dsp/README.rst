A startKIT audio effects demo
=============================

.. version:: 1.0.2

Summary
-------

This application note shows a demo that applies audio effects to a
stereo audio stream on the XMOS startKIT. It shows the use of driving
audio using the I2S library, performing simple DSP in xC and accessing
I/O on the startKIT using the startKIT support library.

The application loops audio input back to audio output with a biquad
filter and a modulating gain applied to the signal. The effects are
controlled via the button and the sliders on the startKIT. The example
also shows the xSCOPE tracing functionality of the xTIMEcomposer tools
by sending internal signal values to the development PC via program instrumentation.

Required tools and libraries
............................

.. appdeps::

Required hardware
.................

The application note is designed to run on the XMOS startKIT with the
XMOS audio slice card (XA-SK-AUDIO) connnected to it.

Prerequisites
.............

  - This document assumes familiarity with the XMOS xCORE architecture, the XMOS GPIO library, 
    the XMOS tool chain and the xC language. Documentation related to these aspects which are 
    not specific to this application note are linked to in the references appendix.
  - For descriptions of XMOS related terms found in this document please see the XMOS Glossary [#]_.

  - The demo uses various libaries, full details of the functionality
    of a library can be found in its user guide [#]_.

  .. [#] http://www.xmos.com/published/glossary

  .. [#] http://www.xmos.com/support/libraries


