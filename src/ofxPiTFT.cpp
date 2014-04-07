//
//  ofxPiTFT.cpp
//  example
//
//  Created by Patricio Gonzalez Vivo on 4/7/14.
//
//

#include "ofxPiTFT.h"


ofxPiTFT::ofxPiTFT(){
    //  Init the display
    //
#ifdef TARGET_RASPBERRY_PI
    fbfd = 0;
    fbp = 0;
    
    setlogmask(LOG_UPTO(LOG_DEBUG));
    openlog("fbcp", LOG_NDELAY | LOG_PID, LOG_USER);
    
    bcm_host_init();
    
    display = vc_dispmanx_display_open(0);
    if (!display) {
        syslog(LOG_ERR, "Unable to open primary display");
        return;
    }
    ret = vc_dispmanx_display_get_info(display, &display_info);
    if (ret) {
        syslog(LOG_ERR, "Unable to get primary display information");
        return;
    }
    syslog(LOG_INFO, "Primary display is %d x %d", display_info.width, display_info.height);
    
    
    fbfd = open("/dev/fb1", O_RDWR);
    if (!fbfd) {
        syslog(LOG_ERR, "Unable to open secondary display");
        return;
    }
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        syslog(LOG_ERR, "Unable to get secondary display information");
        return;
    }
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        syslog(LOG_ERR, "Unable to get secondary display information");
        return;
    }
    
    syslog(LOG_INFO, "Second display is %d x %d %dbps\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    
    screen_resource = vc_dispmanx_resource_create(VC_IMAGE_RGB565, vinfo.xres, vinfo.yres, &image_prt);
    if (!screen_resource) {
        syslog(LOG_ERR, "Unable to create screen buffer");
        close(fbfd);
        vc_dispmanx_display_close(display);
        return;
    }
    
    fbp = (char*) mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbp <= 0) {
        syslog(LOG_ERR, "Unable to create mamory mapping");
        close(fbfd);
        ret = vc_dispmanx_resource_delete(screen_resource);
        vc_dispmanx_display_close(display);
        return;
    }
    
    vc_dispmanx_rect_set(&rect1, 0, 0, vinfo.xres, vinfo.yres);
#endif
    
    ofAddListener(ofEvents().draw,this,&ofxPiTFT::draw);
}

ofxPiTFT::~ofxPiTFT(){
#ifdef TARGET_RASPBERRY_PI
    munmap(fbp, finfo.smem_len);
    close(fbfd);
    ret = vc_dispmanx_resource_delete(screen_resource);
    vc_dispmanx_display_close(display);
#endif
}

void ofxPiTFT::setupTouchEvents(ofBaseApp *_app){
    

}

void ofxPiTFT::draw(ofEventArgs & args){
#ifdef TARGET_RASPBERRY_PI
    vc_dispmanx_snapshot(display, screen_resource, (DISPMANX_TRANSFORM_T)0);
    vc_dispmanx_resource_read_data(screen_resource, &rect1, fbp, vinfo.xres * vinfo.bits_per_pixel / 8);
#endif
}