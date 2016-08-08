A simple utility for using kinectV2 on OSX.

This app will send the colour, depth and IR streams over syphon.

For a precompiled binary visit the releases section of this repo



This version requires 

openframeworks 0.9.0 or later
https://github.com/openframeworks/openFrameworks

ofxMultiKinectV2 
https://github.com/hanasaan/ofxMultiKinectV2
(Check the readme and dependancies)

ofxSyhpon
https://github.com/astellato/ofxSyphon

There is a settings file inside the data folder (leave the folder structure as is).

XML Settings (inside bin/data) change the values as needed

"\<OPENCLDEVICE\>0\</OPENCLDEVICE\>"
If you have a discreet and onboard GFX card try setting this to 1 otherwise leave it at 0

"\<FLIP\>0\</FLIP\>"

Image stream flip setting

"\<MINIMISED\>0\</MINIMISED\>

Tiny mode setting



Key Commands

‘f’ Flip all image streams

‘m’ Toggle tiny mode


	
