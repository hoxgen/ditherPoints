#include "ofApp.h"
#include "ofxDither.h"
#include <iostream>

enum
{
    ORDERED_BAYER,
    FLOYD_STEINBERG,
    ATKINSON
};

//--------------------------------------------------------------

void ofApp::setup()
{
    // SETTINGS ---------------------------------------------
//    mode = 0; // single image
    mode = 1; // video

    int mult_x, mult_y;
    
    // Resize multiplier (moves instances far from each other)
    mult_x = 2;
    mult_y = 2;
    
    width =  1920*mult_x;
    height = 1080*mult_y;

//    width =  2200*mult_x;
//    height = 1466*mult_y;

//      width = 3840*mult_y;
//      height = 2160*mult_x;


//    width =  2160*mult_x;
//    height = 3840*mult_y;

    //How many images in single output file?
    nBatch = 64;
    
    //Color settings
    col_bg.r = 220;
    col_bg.g = 220;
    col_bg.b = 220;

    col.r = 0;
    col.g = 0;
    col.b = 0;

    col_over.r = 255;
    col_over.g = 255;
    col_over.b = 255;

    //BW
//        col_bg.r = 255;
//        col_bg.g = 255;
//        col_bg.b = 255;

//        col.r = 0;
//        col.g = 0;
//        col.b = 0;

//        col_over.r = 255;
//        col_over.g = 255;
//        col_over.b = 255;


    //-------------------------------------------------------

    //
    outTime = ofGetTimestampString("%m-%d-%H-%M-%S");
    
    fbo.allocate(width, height, GL_RGBA);
    savepix.allocate(width, height, GL_RGBA);

    ofSetVerticalSync( true );
    ofEnableAlphaBlending();


    // Load directory
    DIR.listDir("src");
    DIR.allowExt("png");
    DIR.sort();
    if( DIR.size() ){
        images.assign(DIR.size(), ofImage());
    }
    nImages = (int)DIR.size();
    currentImage = 0;
    DIR.sort();
    
    ditherType = ATKINSON;
  

    //for print:
    //fbo.allocate(3100, 3100);
    //for gifs
    //   fbo.allocate(1024, 1024, GL_RGBA);
    //for gifs
    //    fbo.allocate(3072*2, 2047*2);
    
    //Male na poster FIK
    //fbo.allocate(512, 512);
    
    }

//--------------------------------------------------------------

void ofApp::update()
{

}

//--------------------------------------------------------------

void ofApp::draw(){
    //ofBeginSaveScreenAsPDF("export/screenshot-"+ofGetTimestampString()+".pdf", false);
    if (mode == 0) {
    fbo.begin();
    
    //transparent background
    //ofClear(255, 255, 255, 0);
    
    //solid background
    ofBackground(255);
    
    ofSetColor(0,0,0,255);
    //imageIn.draw( 0, 0 );
    
    //--- draw circles instead of pixels

    //--- MODE: SINGLE IMAGE -----------------------------------------
    for (int i = 0; i < nImages; i++){
        //transparent background
        //ofClear(255, 255, 255, 0);
        
        ofBackground(255);
        image.load(DIR.getPath(i));
        dither.dither_atkinson( image, imageOut );

        //Gif Machinemojis 256 / fbo 1024:
        //chaosPoints(imageOut,4,2,127,2);
        //chaosPoints(imageOut,4,2,127,2);
        //(imageOut,resize, circleDiam,treshold,chaos);

        //m = resize multiplier (multiplies size of original image)
        //r = circle diameter (diameter in pixels)
        //t = treshold - not useful on BW dithered images
        //c = chaos power (1-100)
         chaosPoints(imageOut,4,2,127,2);
        
        //Foto timechain test
//        chaosPoints(imageOut,2,1,100,2);
        
        //Velke z 384px zdroju (pro tisk)
        //chaosPoints(imageOut,8,5,127,2);
                
        fbo.readToPixels(savepix);
        ofSaveImage(savepix, "export/"+ofToString(DIR.getName(i)), OF_IMAGE_QUALITY_BEST);
    }
    fbo.end();
    fbo.draw(0,0);
    //ofEndSaveScreenAsPDF();

    ofExit() ;
        
    //--- MODE: VIDEO -----------------------------------------
    } else if (mode == 1) {
        
        fbo.begin();
        // Start from some point in the process
      for (int i = 680-nBatch; (i < nImages); i++){


        // Regular start
//        for (int i = -nBatch; (i < nImages); i++){
           // std::cout  << "progress: " << ofToString(i) << "/" << ofToString(nImages) << '\n';
            ofClear(ofColor(0,0,0,0));
            ofBackground(col_bg);

            float c_run = 1;

            for (int ii = i + nBatch; (ii > (i - nBatch)); ii--){
                if ((ii < nImages) && (ii > 0)) {
                    image.load(DIR.getPath(ii));
                    dither.dither_atkinson( image, imageOut );
                    
                    //Draw original mask in place
                    //images[ii].draw(images_x[ii],images_y[ii],images_w[ii],images_h[ii]);
                    
                    //Draw dithered mask in place
                    //imageOut.draw(images_x[ii],images_y[ii],images_w[ii],images_h[ii]);

                    //c = chaos power (1-100)
                    //int c = int(100* ((ii-i+1)/nBatch));

                    //int c = int(100-(100*(1/c_run)));
                    float c = 100-(100*(1/c_run));
                    c_run = c_run+0.3;

                    //Chaos points
                    //std::cout << "c: " << c << '\n';
                    //m = resize multiplier (multiplies size of original image)
                    //r = circle diameter (diameter in pixels)
                    //t = treshold - not useful on BW dithered images
                    //c = chaos power (1-100)
                    //colors
                    chaosPointsVid(imageOut,image,2,2,250,c,col_bg,col,col_over);
                    ofPopMatrix();
                }
            }
            //Save file with the result
            fbo.readToPixels(savepix);
            ofSaveImage(savepix, "export/part"+ofToString(int(i+nBatch), 4, '0') + ".png", OF_IMAGE_QUALITY_BEST);
        }
        fbo.end();
        ofExit() ;
    }
}

//--------------------------------------------------------------

void ofApp::keyPressed(int key)
{

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
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
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

//----------FUNCTIONS

// Draws an image with fixed aspect ratio
void ofApp::drawImage(ofImage image, int x, int y, int w){
    float h = floor(w*(image.getHeight()/image.getWidth()));
    image.draw(x,y,w,h);
}

void ofApp::chaosPoints(ofImage image, int m, int r, int t, int c){
    //m = resize multiplier
    //r = circle diameter
    //t = treshold
    //c = chaos randomness
    
    ofPixels pix;
    imageOut.getTexture().readToPixels(pix);
    int i = 0;
    while( i < pix.size()) {
        //ofSetColor(pix.getColor(i));
        int n  = i / pix.getWidth() + 1;
        int light1 = pix.getColor(i).getLightness();
        int light2 = 255;
        int light3 = 255;
        int light4 = 255;
        if (i>1) {
            light2 = pix.getColor(i-1).getLightness();
        }
        if (i>pix.getWidth()) {
            light3 = pix.getColor(i-pix.getWidth()).getLightness();
        }
        if (i>(1+pix.getWidth())) {
            light4 = pix.getColor(i-pix.getWidth()-1).getLightness();
        }
        if (light1 < t){
            //ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(0,r), n * m - ofRandom(0,r),r);
            //ofSetColor(0,0,0,255);
            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r,r), n * m - ofRandom(-r,r),r);
            //ofSetColor(0,0,0,255);
            //ofDrawCircle((i % pix.getWidth() +1) * m - r, n * m - r,r);
        }
        if ((light1 == 0) && (light2==0)) {
            //ofSetColor(255,0,0,255);
            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(0,r), n * m - ofRandom(0,r),r);
            //ofDrawCircle((i % pix.getWidth() +1) * m, n * m ,r);
        }
        if ((light1 == 0) && (light2==0) && (light3==0)) {
            //ofSetColor(0,255,0,255);
            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r,2 * r), n * m - ofRandom(-r,2*r),r);
            //ofDrawCircle((i % pix.getWidth() +1) * m, n * m ,r);
        }
        if ((light1 == 0) && (light2==0) && (light3==0) && (light4==0)) {
            //ofSetColor(0,0,255,255);
            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-2 * r,r), n * m - ofRandom(-2 * r,r),1.5*r);
            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r, 2 * r), n * m - ofRandom(-r, 2 * r),1.5*r);
            //ofDrawCircle((i % pix.getWidth() +1) * m, n * m ,r);
        }
        i++;
    }
    
}


void ofApp::chaosPointsVid(ofImage image, ofImage image_bg, int m, int r, int t, float c, ofColor vcol_bg, ofColor vcol, ofColor vcol_over){
    //m = resize multiplier (multiplies size of original image)
    //r = circle diameter (diameter in pixels)
    //t = treshold - not useful on BW dithered images
    //c = chaos power (1-100)

    ofPixels pix;
    ofPixels pix_bg;
    
    image_bg.setImageType(OF_IMAGE_GRAYSCALE);
    
    //read source pixels
    image.getTexture().readToPixels(pix);
    image_bg.getTexture().readToPixels(pix_bg);
    int i = 0;
    
    // --- draw white circles to get some background
    ofSetColor(vcol_bg);
    while( i < pix_bg.size()) {
        //examine nearby pixels
        
        //width of each pixels row
        int n  = i / pix_bg.getWidth() + 1;
        
        //lightness of current pixel
        int light1 = pix_bg.getColor(i).getLightness();
        
        if ((light1 < t) && (ofRandom(0,170)<100-c)) {
            //            cout << "l: " << ofToString(light1) << '\n';;
            //blur
            //ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r*c/2,r*c/2), n * m - ofRandom(-r*c,r*c),r);
                        ofDrawCircle((i % pix_bg.getWidth() +1) * m - ofRandom(-r,r), n * m - ofRandom(-r,r),r*2);
        }
        if ((light1 < t) && (ofRandom(0,230)<100-c)) {
            //            cout << "l: " << ofToString(light1) << '\n';;
            //blur
            //ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r*c/4,r*c/4), n * m - ofRandom(-r*c/4,r*c/4),r);
                        ofDrawCircle((i % pix_bg.getWidth() +1) * m - ofRandom(-r,r), n * m - ofRandom(-r,r),r*2);
        }
        i++;
    }
    
    // --- Draw black circles
    i = 0;
    ofSetColor(vcol);

    while( i < pix.size()) {

        //Random colors
        vcol.r = int(ofRandom(c*2,255));
        vcol.g = int(ofRandom(c*2,255));
        vcol.b = int(ofRandom(c*2,255));

//        vcol.r = int(c);
//        vcol.g = int(c);
//        vcol.b = int(c);


        ofSetColor(vcol);


        //examine nearby pixels
        
        //width of each pixels row
        int n  = i / pix.getWidth() + 1;
        
        //lightness of current pixel
        int light1 = pix.getColor(i).getLightness();
        int light2 = 255;
        int light3 = 255;
        int light4 = 255;
        
        //lightness of previous pixel
        if (i>1) {
            light2 = pix.getColor(i-1).getLightness();
        }
        //lightness of pixel above
        if (i>pix.getWidth()) {
            light3 = pix.getColor(i-pix.getWidth()).getLightness();
        }
        //lightness of pixel above left
        if (i>(1+pix.getWidth())) {
            light4 = pix.getColor(i-pix.getWidth()-1).getLightness();
        }
        
        //draw circle instead of current pixel
        // V1 --- without randomness factor c --------------------
        //draw black pixels
        //        if (light1 < t) {
        //            ofSetColor(col);
        //            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r,r), n * m - ofRandom(-r,r),r);
        //        }
        // V2 --- use c to multiply randomness 1-100 -----------------
        //display only some points according to randomness factor c
        if ((light1 < t) && (ofRandom(0,100)<110-c)) {
            //            blur
//                        ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r*c/4,r*c/4), n * m - ofRandom(-r*c,r*c),r);

                        ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r,r), n * m - ofRandom(-r,r),r);
        }

        //useful when the circles are larger than 1
        //draw more circles to fill in black parts
        if ((light1 == 0) && (light2==0) && (ofRandom(0,150)<120-c)) {
            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(0,r), n * m - ofRandom(0,r),r);
        }
        //draw more circles to fill in black parts
        if ((light1 == 0) && (light2==0) && (light3==0) && (ofRandom(0,150)<120-c)) {
            //ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r*c/4,2 * r*c/4), n * m - ofRandom(-r*c,2*r*c),r);
            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-2 * r,r), n * m - ofRandom(-2 * r,r),1.5*r);
        }
        //draw more circles to fill in black parts
        //        if ((light1 == 0) && (light2==0) && (light3==0) && (light4==0)) {
        //            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-2 * r,r), n * m - ofRandom(-2 * r,r),1.5*r);
        //            ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r, 2 * r), n * m - ofRandom(-r, 2 * r),1.5*r);
        //        }
        i++;
    }
    

//    // --- draw circles to get some overlay with BG color
//    ofSetColor(vcol_over);
//    i = 0;
//    while( i < pix_bg.size()) {
//        //examine nearby pixels
        
//        //width of each pixels row
//        int n  = i / pix_bg.getWidth() + 1;
        
//        //lightness of current pixel
//        int light1 = pix_bg.getColor(i).getLightness();
   
//        if ((light1 < t) && (ofRandom(50,100)>c)) {
//            //            cout << "l: " << ofToString(light1) << '\n';;
//            //blur
//            //ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r*c/2,r*c/2), n * m - ofRandom(-r*c,r*c),r);
//            ofDrawCircle((i % pix_bg.getWidth() +1) * m - ofRandom(-r,r), n * m - ofRandom(-r,r),r*2);
//        }
//        if ((light1 < t) && (ofRandom(20,100)>c)) {
//            //            cout << "l: " << ofToString(light1) << '\n';;
//            //blur
//            //ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r*c/4,r*c/4), n * m - ofRandom(-r*c/4,r*c/4),r*2);
//            ofDrawCircle((i % pix_bg.getWidth() +1) * m - ofRandom(-r,r), n * m - ofRandom(-r,r),r*2);
//        }
//        i++;

////                //examine nearby pixels

////                //width of each pixels row
////                int n  = i / pix_bg.getWidth() + 1;

////                //lightness of current pixel
////                int light1 = pix_bg.getColor(i).getLightness();

////                if ((light1 < t) && (ofRandom(0,170)<100-c)) {
////                    //            cout << "l: " << ofToString(light1) << '\n';;
////                    //blur
////                    //ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r*c/2,r*c/2), n * m - ofRandom(-r*c,r*c),r);
////                                ofDrawCircle((i % pix_bg.getWidth() +1) * m - ofRandom(-r,r), n * m - ofRandom(-r,r),r*2);
////                }
////                if ((light1 < t) && (ofRandom(00,230)<100-c)) {
////                    //            cout << "l: " << ofToString(light1) << '\n';;
////                    //blur
////                    //ofDrawCircle((i % pix.getWidth() +1) * m - ofRandom(-r*c/4,r*c/4), n * m - ofRandom(-r*c/4,r*c/4),r);
////                                ofDrawCircle((i % pix_bg.getWidth() +1) * m - ofRandom(-r,r), n * m - ofRandom(-r,r),r*2);
////                }
////                i++;
//    }
    
    
}
