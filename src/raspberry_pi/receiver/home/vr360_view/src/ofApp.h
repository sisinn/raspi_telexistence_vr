#pragma once

#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "ofxSerial.h"

#define SCREEN_WIDTH        1920
#define SCREEN_HIGH         1080
#define SLICES              240
#define STACKS              135
//#define SLICES              48
//#define STACKS              27

#define VERTEX_COUNT        (SLICES * 2 + 2)
#define PRIMITIVE_COUNT     (STACKS)

#define PI_VALUE        (3.14159265359)

extern int wxyz[4];
extern int th_max;
extern int r_max;
extern float focal;
extern float smooth;
extern int circle_center_x;
extern int circle_center_y;
extern int front_tilt;
extern int back_tilt;

typedef enum{
    YELLOW_STATE_STARTUP,
    YELLOW_STATE_NOMAL,
    YELLOW_STATE_CROSS,
    YELLOW_STATE_ROTATE,
    YELLOW_STATE_PAUSE,
    YELLOW_STATE_CLEAR,
    YELLOW_STATE_MAX
} eYELLOW_STATE;

class ofApp : public ofBaseApp, public ofxOMXPlayerListener{
    
public:
    
    void setup();
    void update();
    void draw();
    void exit();

    ofxOMXPlayer omxPlayer;
    ofShader shader;

    ofxIO::SLIPPacketSerialDevice serial;
    ofxIO::ByteBuffer SerialData_Update; 

    int input_start;
    eYELLOW_STATE yellow_state;
    ofQuaternion qt;
    ofQuaternion sensor_q_first_inv;
    ofQuaternion sensor_q;

    int rot_x;
    int rot_y;
    int rot_z;

    ofVboMesh Mesh[PRIMITIVE_COUNT];
    void initMatrix(void);
    void createMesh(ofMatrix4x4 * mat, ofVboMesh *pMesh, ofVec3f *p_ov, unsigned int *p_oi, int array_max);
    void calcCircle(void);

    void getSerialData(ofxIO::ByteBuffer SerialData);

    void onVideoEnd(ofxOMXPlayerListenerEventData& e);
    void onVideoLoop(ofxOMXPlayerListenerEventData& e);

    void onSerialBuffer(const ofxIO::SerialBufferEventArgs& args);
    void onSerialError(const ofxIO::SerialBufferErrorEventArgs& args);
};

