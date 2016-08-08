#pragma once

#include "ofMain.h"
#include "ofxMultiKinectV2.h"
#include "ofxSyphon.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp{
    
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofShader depthShader;
    ofShader irShader;
    ofxXmlSettings XML;
    ofxMultiKinectV2 kinect;
    ofTexture colorTex;
    ofTexture depthTex;
    ofTexture irTex;
    ofxSyphonServer colourSyphon, depthSyphon, iRSyphon;
    ofFbo irFbo, depthFbo;
    int openCLDevice;
    bool minimised;
    bool flip;
    int recievePort;
    
    ofxOscReceiver receiver;
    ofxOscSender sender;
    string sendIp;
    int sendPort;
    bool hasColor, hasIr, hasDepth;
   
};