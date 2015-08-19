#include "ofMain.h"
#include "ofxMultiKinectV2.h"
#include "ofxSyphon.h"
#include "ofxXmlSettings.h"

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
class ofApp : public ofBaseApp{
    ofShader depthShader;
    ofShader irShader;
    ofxXmlSettings XML;
    ofxMultiKinectV2 kinect0;
    ofxMultiKinectV2 kinect1;
    ofTexture colorTex0;
    ofTexture colorTex1;
    ofTexture depthTex0;
    ofTexture depthTex1;
    ofTexture irTex0;
    ofTexture irTex1;
    ofxSyphonServer colourSyphon, depthSyphon, iRSyphon;
    ofFbo irFbo, depthFbo;
    int openCLDevice;
    bool minimised;
    bool flip;
public:
    void setup()
    {
        
        XML.loadFile("settings");
        openCLDevice= XML.getValue("OPENCLDEVICE", 0);
        flip= XML.getValue("FLIP", 0);
        minimised = XML.getValue("MINIMISED", 0);
        
        ofSetVerticalSync(true);
        ofSetFrameRate(60);
        
        depthShader.setupShaderFromSource(GL_FRAGMENT_SHADER, depthFragmentShader);
        depthShader.linkProgram();
        
        irShader.setupShaderFromSource(GL_FRAGMENT_SHADER, irFragmentShader);
        irShader.linkProgram();
        
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

        colourSyphon.setName("KinectV2 Colour");
        iRSyphon.setName("KinectV2 IR");
        depthSyphon.setName("KinectV2 Depth");
        irFbo.allocate(512, 424);
        depthFbo.allocate(512, 424);
        
        if (minimised) {
            ofSetWindowShape(1024, 50);
        }
        if (!minimised) {
            ofSetWindowShape(640+512+512, 424);
        }
    }
    
    void update() {
        kinect0.update();
        if (kinect0.isFrameNew()) {
            colorTex0.loadData(kinect0.getColorPixelsRef());
            
            depthTex0.loadData(kinect0.getDepthPixelsRef());
            
            irTex0.loadData(kinect0.getIrPixelsRef());
            
        }

    }
    
    void draw()
    {
        ofClear(0);
        
        
        

        if (colorTex0.isAllocated()) {
            if (!minimised) {
                colorTex0.draw(0, 0, 640, 360);
            }
            
            colourSyphon.publishTexture(&colorTex0);
        }
        if (depthTex0.isAllocated()) {
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
       
        ofPushStyle();
        
        ofDrawBitmapStringHighlight("Frame Rate " + ofToString(ofGetFrameRate()), 10, 20);
        ofDrawBitmapStringHighlight("OpenCL Device : " + ofToString(openCLDevice), 10, 40);
        ofPopStyle();
        
 
    }
    
    void keyPressed(int key)
    {
        if (key == 'f') {
            kinect0.setEnableFlipBuffer(!kinect0.isEnableFlipBuffer());
        }
        
        if (key == 'm') {
            minimised=!minimised;
            
            if (minimised) {
                ofSetWindowShape(1024, 50);
            }
            if (!minimised) {
                ofSetWindowShape(640+512+512, 424);
            }
        }
      
    }
    void exit(){
        kinect0.close();
        
    }
};

//========================================================================
int main( ){
    ofSetupOpenGL(1680,800,OF_WINDOW);            // <-------- setup the GL context
    
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
    
}
