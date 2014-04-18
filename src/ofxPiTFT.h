//
//  FtfMirror.h
//
//  Thanks to Matt Bell
//  http://forum.openframeworks.cc/t/pitft-adafruit-tft-touchscreen/15107/5
//  https://github.com/notro/fbtft
//
//
//  TODO:
//      - add touch events from this code by Mark Williams
//       https://github.com/mwilliams03/Pi-Touchscreen-basic

#include "ofMain.h"

#ifdef TARGET_RASPBERRY_PI

#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <bcm_host.h>

//#include <linux/input.h>
//#include <string.h>
//
//extern "C" {
//    #include "touch.h"
//}

#endif

typedef enum {
    NO_ROTATE = 0,
    ROTATE_90 = 1,
    ROTATE_180 = 2,
    ROTATE_270 = 3,
    
    FLIP_HRIZ = 1 << 16,
    FLIP_VERT = 1 << 17,
} SCREEN_TRANSFORMATION;

class ofxPiTFT{
public:
    ofxPiTFT();
    ~ofxPiTFT();
    
    void setupTouchEvents(ofBaseApp *_app);
    
    void setScreenTransformation(SCREEN_TRANSFORMATION _trans);
    
    void update(ofEventArgs & args);
    void draw(ofEventArgs & args);
    void exit(ofEventArgs & args);
    
private:
    
    ofBaseApp *app;
    
//    int rawX, rawY, rawPressure;
//	float scaleXvalue, scaleYvalue;
    
#ifdef TARGET_RASPBERRY_PI
    int primaryDisplay;
    DISPMANX_MODEINFO_T primaryDisplayInfo;
    
    int secondaryDisplayBuffer;
    VC_RECT_T secondaryDisplayRect;
    struct fb_var_screeninfo secondaryVInfo;
    struct fb_fix_screeninfo secondaryFInfo;
    
    int screenResource;
    
    DISPMANX_TRANSFORM_T screenTransformation;
    uint32_t image_prt;
    
    int ret;
    char *fbp;

#endif
};