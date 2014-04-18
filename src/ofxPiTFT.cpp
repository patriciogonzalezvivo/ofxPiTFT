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
int fd;

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


#ifdef TARGET_RASPBERRY_PI
//void getTouchScreenDetails(int *screenXmin,int *screenXmax,int *screenYmin,int *screenYmax){
//	unsigned short id[4];
//    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
//    char name[256] = "Unknown";
//    int abs[6] = {0};
//    
//    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
//    printf("Input device name: \"%s\"\n", name);
//    
//    memset(bit, 0, sizeof(bit));
//    ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
//    printf("Supported events:\n");
//    
//    int i,j,k;
//    for (i = 0; i < EV_MAX; i++)
//        if (test_bit(i, bit[0])) {
//            printf("  Event type %d (%s)\n", i, events[i] ? events[i] : "?");
//            if (!i) continue;
//            ioctl(fd, EVIOCGBIT(i, KEY_MAX), bit[i]);
//            for (j = 0; j < KEY_MAX; j++){
//                if (test_bit(j, bit[i])) {
//                    printf("    Event code %d (%s)\n", j, names[i] ? (names[i][j] ? names[i][j] : "?") : "?");
//                    if (i == EV_ABS) {
//                        ioctl(fd, EVIOCGABS(j), abs);
//                        for (k = 0; k < 5; k++)
//                            if ((k < 3) || abs[k]){
//                                printf("     %s %6d\n", absval[k], abs[k]);
//                                if (j == 0){
//                                    if (absval[k] == "Min  ") *screenXmin =  abs[k];
//                                    if (absval[k] == "Max  ") *screenXmax =  abs[k];
//                                }
//                                if (j == 1){
//                                    if (absval[k] == "Min  ") *screenYmin =  abs[k];
//                                    if (absval[k] == "Max  ") *screenYmax =  abs[k];
//                                }
//                            }
//                    }
//                    
//                }
//            }
//        }
//}
//
//void getTouchSample(int *rawX, int *rawY, int *rawPressure){
//	int i;
//    /* how many bytes were read */
//    size_t rb;
//    /* the events (up to 64 at once) */
//    struct input_event ev[64];
//    
//	rb=read(fd,ev,sizeof(struct input_event)*64);
//    for (i = 0;  i <  (rb / sizeof(struct input_event)); i++){
//        if (ev[i].type ==  EV_SYN)
//            printf("Event type is %s%s%s = Start of New Event\n",KYEL,events[ev[i].type],KWHT);
//        
//        else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 1)
//            printf("Event type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s1%s = Touch Starting\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,KWHT);
//        
//        else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 0)
//			printf("Event type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s0%s = Touch Finished\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,KWHT);
//        
//        else if (ev[i].type == EV_ABS && ev[i].code == 0 && ev[i].value > 0){
//            printf("Event type is %s%s%s & Event code is %sX(0)%s & Event value is %s%d%s\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,ev[i].value,KWHT);
//			*rawX = ev[i].value;
//		}
//        else if (ev[i].type == EV_ABS  && ev[i].code == 1 && ev[i].value > 0){
//            printf("Event type is %s%s%s & Event code is %sY(1)%s & Event value is %s%d%s\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,ev[i].value,KWHT);
//			*rawY = ev[i].value;
//		}
//        else if (ev[i].type == EV_ABS  && ev[i].code == 24 && ev[i].value > 0){
//            printf("Event type is %s%s%s & Event code is %sPressure(24)%s & Event value is %s%d%s\n", KYEL,events[ev[i].type],KWHT,KYEL,KWHT,KYEL,ev[i].value,KWHT);
//			*rawPressure = ev[i].value;
//		}
//        
//	}
//}
#endif

void ofxPiTFT::setupTouchEvents(ofBaseApp *_app){
    app = _app;

    #ifdef TARGET_RASPBERRY_PI
    //  Touch
    //
//    if ((fd = open("/dev/input/event0", O_RDONLY)) < 0) {
//        return 1;
//    }
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