
#include "ofApp.h"


#define STRINGIFY(x) #x

static string depthFragmentShader =
STRINGIFY(
          uniform sampler2DRect tex;
          void main()
          {
              vec4 col = texture2DRect(tex, gl_TexCoord[0].xy);
              float value = col.r;
              float low1 = 500.0;
              float high1 = 5000.0;
              float low2 = 1.0;
              float high2 = 0.0;
              float d = clamp(low2 + (value - low1) * (high2 - low2) / (high1 - low1), 0.0, 1.0);
              if (d == 1.0) {
                  d = 0.0;
              }
              gl_FragColor = vec4(vec3(d), 1.0);
          }
          );

static string irFragmentShader =
STRINGIFY(
          uniform sampler2DRect tex;
          void main()
          {
              vec4 col = texture2DRect(tex, gl_TexCoord[0].xy);
              float value = col.r / 65535.0;
              gl_FragColor = vec4(vec3(value), 1.0);
          }
          );

//========================================================================

void ofApp::setup()
{
    
    XML.loadFile("settings");
    openCLDevice= XML.getValue("OPENCLDEVICE", 0);
    flip= XML.getValue("FLIP", 0);
    minimised = XML.getValue("MINIMISED", 0);
    recievePort	= XML.getValue("RECIEVEPORT", 12334);
    sendPort =XML.getValue("SENDPORT", 12335);
    sendIp = XML.getValue("SENDIP", "127.0.0.1");
    hasColor = XML.getValue("SENDIP", 1);
    hasIr = XML.getValue("SENDIP", 1);
    hasDepth = XML.getValue("SENDIP", 1);
    receiver.setup(recievePort);
    sender.setup(sendIp, sendPort);
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    if (hasDepth) {
        depthShader.setupShaderFromSource(GL_FRAGMENT_SHADER, depthFragmentShader);
        depthShader.linkProgram();
    }
    if (hasIr) {
        irShader.setupShaderFromSource(GL_FRAGMENT_SHADER, irFragmentShader);
        irShader.linkProgram();
    }
    if (openCLDevice==0) {
        kinect0.open(true, true, 0);
    }
    
    if (openCLDevice==1) {
        kinect0.open(true, true, 0, 2);
    }
    
    
    // Note :
    // Default OpenCL device might not be optimal.
    // e.g. Intel HD Graphics will be chosen instead of GeForce.
    // To avoid it, specify OpenCL device index manually like following.
    // kinect1.open(true, true, 0, 2); // GeForce on MacBookPro Retina
    
    kinect0.start();
    
    if (hasColor) {
        colourSyphon.setName("KinectV2 Colour");
    }
    if (hasDepth) {
        depthSyphon.setName("KinectV2 Depth");
        depthFbo.allocate(512, 424);
    }
    if (hasIr) {
        iRSyphon.setName("KinectV2 IR");
        irFbo.allocate(512, 424);
    }
    if (minimised) {
        ofSetWindowShape(1024, 50);
    }
    if (!minimised) {
        ofSetWindowShape(640+512+512, 424);
    }
}

void ofApp::update() {
    kinect0.update();
    if (kinect0.isFrameNew()) {
        
        if (hasColor) {
            colorTex0.loadData(kinect0.getColorPixelsRef());
        }
        if (hasDepth) {
            depthTex0.loadData(kinect0.getDepthPixelsRef());
        }
        if (hasIr) {
            irTex0.loadData(kinect0.getIrPixelsRef());
        }
    }
    
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        if ( m.getAddress() == "/minimise" ){
            if (minimised) {
                ofSetWindowShape(1024, 50);
            }
            if (!minimised) {
                ofSetWindowShape(640+512+512, 424);
            }
            minimised=m.getArgAsInt32(0);
            ofxOscMessage  myMessage;
            myMessage.setAddress("/minimise");
            myMessage.addIntArg(minimised);
            sender.sendMessage(myMessage);
        }
        
        if ( m.getAddress() == "/flip" ){
            flip=m.getArgAsInt32(0);
            ofxOscMessage  myMessage;
            myMessage.setAddress("/flip");
            myMessage.addIntArg(flip);
            sender.sendMessage(myMessage);
        }
    }
}

void ofApp::draw()
{
    ofClear(0);
    
    
    if (hasColor) {
        if (colorTex0.isAllocated()) {
            if (!minimised) {
                colorTex0.draw(0, 0, 640, 360);
            }
            colourSyphon.publishTexture(&colorTex0);
        }
    }
    
    
    if (depthTex0.isAllocated()) {
        if (hasDepth) {
            depthFbo.begin();
            ofClear(0, 0, 0);
            depthShader.begin();
            depthTex0.draw(0, 0, 512, 424);
            depthShader.end();
            depthFbo.end();
            depthSyphon.publishTexture(&depthFbo.getTextureReference());
            if (!minimised) {
                depthFbo.draw(640, 0, 512, 424);
            }
        }
        
        if (hasIr) {
            irFbo.begin();
            ofClear(0,0,0);
            irShader.begin();
            irTex0.draw(0, 0, 512, 424);
            irShader.end();
            irFbo.end();
            iRSyphon.publishTexture(&irFbo.getTextureReference());
            if (!minimised) {
                irFbo.draw(1152, 0, 512, 424);
            }
        }
    }
    
    ofPushStyle();
    ofDrawBitmapStringHighlight("Frame Rate " + ofToString(ofGetFrameRate()), 10, 20);
    ofDrawBitmapStringHighlight("OpenCL Device : " + ofToString(openCLDevice), 10, 40);
    ofPopStyle();
    
    
}

void ofApp::keyPressed(int key)
{
    if (key == 'f') {
        kinect0.setEnableFlipBuffer(!kinect0.isEnableFlipBuffer());
        ofxOscMessage  myMessage;
        myMessage.setAddress("/flip");
        myMessage.addIntArg(flip);
        sender.sendMessage(myMessage);
    }
    
    if (key == 'm') {
        minimised=!minimised;
        ofxOscMessage  myMessage;
        myMessage.setAddress("/minimise");
        myMessage.addIntArg(minimised);
        sender.sendMessage(myMessage);
        
        if (minimised) {
            ofSetWindowShape(1024, 50);
        }
        if (!minimised) {
            ofSetWindowShape(640+512+512, 424);
        }
    }
    
}
void ofApp::exit(){
    kinect0.close();
    
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//========================================================================

