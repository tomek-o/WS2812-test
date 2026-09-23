/** \file
	\brief Changelog
*/

/** \page changelog Changelog

Version 1.1
- added WASAPI-based audio visualisation (VU-meter)
	- works with Windows 7 and Windows 10
	- captures either audio output (loopback) or input
	- option to start visualisation automatically
- added option to hide to tray
- added option to add to autostart
- updated to match latest turbocpp_template

Version 1.1.1
- option to turn off LEDs on close (enabled by default)
- audio visualisation
	- changed minimum brightness to 2 - acceptable even in darkness but reducing color palette
	- not updating bargraph in the GUI (intended mostly for debug purposes) if application is minimized or hidden to tray - reduced CPU usage down to ~0.2% on Haswell i5 laptop
	- added additional modes for stereo - bagraph scales extending from the middle or LED chain to outside and from outside to the middle	
*/
