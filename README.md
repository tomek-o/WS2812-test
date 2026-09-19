# WS2812 test
Windows application driving a WS2812 (NeoPixel) LED strip from a PC UART, using
single transistor signal inverter.

## Features
- configurable length of LED strip (check power demand!)
- manual per-LED color control
- simple animations
- seting all LEDs to one color
- audio visualisation (bargraph): WASAPI loopback capture of the current Windows default output
  device, rendered as a dB-scaled VU bar with configurable gain, smoothing, update rate,
  brightness cap, optional peak-hold marker, and optional left/right channel separation - plus
  a full-brightness on-screen preview independent of the strip's own brightness limit
- options to auto-start and start minimized to tray

![WS2812_test](https://github.com/user-attachments/assets/0598e2a2-5bbf-403d-99e5-abb9fbf0787b)

![FT232RL and WS2812](https://tomeko.net/software/WS2812_test/WS2812_FT232RL.jpg)

https://tomeko.net/software/WS2812_test/
