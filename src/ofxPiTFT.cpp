//
//  ofxPiTFT.cpp
//  example
//
//  Created by Patricio Gonzalez Vivo on 4/7/14.
//
//

#include "ofxPiTFT.h"
#define KWHT  "\x1B[37m"
#define KYEL  "\x1B[33m"

ofxPiTFT::ofxPiTFT(){
    
    app = NULL;
    
    //  Init the display
    //
#ifdef TARGET_RASPBERRY_PI
    secondaryDisplayBuffer = 0;
    screenTransformation = (DISPMANX_TRANSFORM_T)0;
    fbp = 0;
    
    // PRIMARY DISPLAY
    //
    bcm_host_init();
    primaryDisplay = vc_dispmanx_display_open(0);
    if (!primaryDisplay) {
        ofLog(OF_LOG_ERROR, "Unable to open primary display");
        return;
    }
    ret = vc_dispmanx_display_get_info(primaryDisplay, &primaryDisplayInfo);
    if (ret) {
         ofLog(OF_LOG_ERROR, "Unable to get primary display information");
        
        return;
    }
    ofLog(OF_LOG_NOTICE, "Primary display is " + ofToString(primaryDisplayInfo.width) + " x " + ofToString(primaryDisplayInfo.height));
    
    // SECONDARY DISPLAY
    //
    secondaryDisplayBuffer = open("/dev/fb1", O_RDWR);
    if (!secondaryDisplayBuffer) {
         ofLog(OF_LOG_ERROR, "Unable to open secondary display");
        return;
    }
    if (ioctl(secondaryDisplayBuffer, FBIOGET_FSCREENINFO, &secondaryFInfo)) {
         ofLog(OF_LOG_ERROR, "Unable to get secondary display information");
        return;
    }
    if (ioctl(secondaryDisplayBuffer, FBIOGET_VSCREENINFO, &secondaryVInfo)) {
         ofLog(OF_LOG_ERROR, "Unable to get secondary display information");
        return;
    }
    ofLog(OF_LOG_NOTICE, "Second display is " + ofToString(secondaryVInfo.xres) + " x " + ofToString(secondaryVInfo.yres) + " " + ofToString(secondaryVInfo.bits_per_pixel) );
    
    //  SCREEN RESOURCE
    //
    screenResource = vc_dispmanx_resource_create(VC_IMAGE_RGB565, secondaryVInfo.xres, secondaryVInfo.yres, &image_prt);
    if (!screenResource) {
         ofLog(OF_LOG_ERROR, "Unable to create screen buffer");
        close(secondaryDisplayBuffer);
        vc_dispmanx_display_close(primaryDisplay);
        return;
    }
    
    //  Memory BUFFER
    //
    fbp = (char*) mmap(0, secondaryFInfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, secondaryDisplayBuffer, 0);
    if (fbp <= 0) {
        ofLog(OF_LOG_ERROR, "Unable to create mamory mapping");
        close(secondaryDisplayBuffer);
        ret = vc_dispmanx_resource_delete(screenResource);
        vc_dispmanx_display_close(primaryDisplay);
        return;
    }
    
    //  PiTFT Proportions
    //
    vc_dispmanx_rect_set(&secondaryDisplayRect, 0, 0, secondaryVInfo.xres, secondaryVInfo.yres);
#endif
    
    ofAddListener(ofEvents().draw,this,&ofxPiTFT::draw);
    ofAddListener(ofEvents().exit,this,&ofxPiTFT::exit);
}

ofxPiTFT::~ofxPiTFT(){
    ofRemoveListener(ofEvents().update,this,&ofxPiTFT::update);
    ofRemoveListener(ofEvents().draw,this,&ofxPiTFT::draw);
    ofRemoveListener(ofEvents().exit,this,&ofxPiTFT::exit);
}

void ofxPiTFT::setupTouchEvents(ofBaseApp *_app){
    app = _app;

    #ifdef TARGET_RASPBERRY_PI
    //  Touch
    //
//    if (openTouchScreen() == 1)
//		perror("error opening touch screen");
//    
//    int screenXmax, screenXmin;
//	int screenYmax, screenYmin;
//    
//    getTouchScreenDetails(&screenXmin,&screenXmax,&screenYmin,&screenYmax);
//    scaleXvalue = ((float)screenXmax-screenXmin) / 320.0;
//    scaleYvalue = ((float)screenYmax-screenYmin) / 240.0;
    #endif
    
    ofAddListener(ofEvents().update,this,&ofxPiTFT::update);
}

void ofxPiTFT::setScreenTransformation(SCREEN_TRANSFORMATION _trans){
    #ifdef TARGET_RASPBERRY_PI
    screenTransformation = (DISPMANX_TRANSFORM_T)_trans;
    #endif
}

void ofxPiTFT::update(ofEventArgs & args){
    #ifdef TARGET_RASPBERRY_PI
//    getTouchSample(&rawX, &rawY, &rawPressure);
//    app->mouseX = (rawX/scaleXvalue)*ofGetWidth();
//    app->mouseY = (rawY/scaleYvalue)*ofGetHeight();
    #endif
}

void ofxPiTFT::draw(ofEventArgs & args){
#ifdef TARGET_RASPBERRY_PI
    vc_dispmanx_snapshot(primaryDisplay, screenResource, screenTransformation);
    vc_dispmanx_resource_read_data(screenResource, &secondaryDisplayRect, fbp, secondaryVInfo.xres * secondaryVInfo.bits_per_pixel / 8);
#endif
}

void ofxPiTFT::exit(ofEventArgs & args){
#ifdef TARGET_RASPBERRY_PI
    munmap(fbp, secondaryFInfo.smem_len);
    close(secondaryDisplayBuffer);
    ret = vc_dispmanx_resource_delete(screenResource);
    vc_dispmanx_display_close(primaryDisplay);
#endif
}