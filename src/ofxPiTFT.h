//
//  FtfMirror.h
//
//  Thanks to Matt Bell
//  http://forum.openframeworks.cc/t/pitft-adafruit-tft-touchscreen/15107/5
//
//  https://github.com/notro/fbtft
//
//  And Mark Williams
//  https://github.com/mwilliams03/Pi-Touchscreen-basic

#include "ofMain.h"

#ifdef TARGET_RASPBERRY_PI
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <syslog.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <bcm_host.h>
#include <signal.h>

char *events[EV_MAX + 1] = {
	[0 ... EV_MAX] = NULL,
	[EV_SYN] = "Sync",[EV_KEY] = "Key",
	[EV_REL] = "Relative",[EV_ABS] = "Absolute",
	[EV_MSC] = "Misc",[EV_LED] = "LED",
	[EV_SND] = "Sound",[EV_REP] = "Repeat",
	[EV_FF] = "ForceFeedback",[EV_PWR] = "Power",
	[EV_FF_STATUS] = "ForceFeedbackStatus",
};

#endif

class ofxPiTFT{
public:
    ofxPiTFT();
    ~ofxPiTFT();
    
    void setupTouchEvents(ofBaseApp *_app);
    
    void update(ofEventArgs & args);
    void draw(ofEventArgs & args);
    void exit(ofEventArgs & args);
    
private:
    
    ofBaseApp *app;
    
    int screenXmax, screenXmin;
	int screenYmax, screenYmin;
    int rawX, rawY, rawPressure;
	float scaleXvalue, scaleYvalue;
    
#ifdef TARGET_RASPBERRY_PI
    //DISPMANX_DISPLAY_HANDLE_T display;
    int display;
    DISPMANX_MODEINFO_T display_info;
    
    //DISPMANX_RESOURCE_HANDLE_T screen_resource;
    int screen_resource;
    VC_IMAGE_TRANSFORM_T transform;
    uint32_t image_prt;
    VC_RECT_T rect1;
    
    int ret;
    int fbfd;
    char *fbp;
    
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
#endif
};