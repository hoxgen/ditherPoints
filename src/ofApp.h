#pragma once

#include "ofMain.h"
#include "ofxDither.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofFbo fbo;
    ofPixels savepix;
    
    //Output filename timestamp
    string outTime;
    
    ofxDither dither;
    int width, height, ditherType,mult_x, mult_y, mode;
    
    //images
    ofImage image, pix, imageOut, background, backgroundOut;
    
    ofColor col, col_bg, col_over;
    
    // Function for drawing images
    void drawImage(ofImage image, int x, int y, int w);
    
    // Function for pointilizing
    void chaosPoints(ofImage image, int m, int r, int t, int c);
    void chaosPointsVid(ofImage image, ofImage image_bg, int m, int r, int t, float c, ofColor vcol_bg, ofColor vcol, ofColor vcol_over);

    // we will have a dynamic number of images, based on the content of a directory:
    int  nImages;
    ofDirectory DIR;
    vector<ofImage> images;
    int currentImage;
    float nBatch;
    
    // images positions
    int * images_x;
    int * images_y;
    
    
};



