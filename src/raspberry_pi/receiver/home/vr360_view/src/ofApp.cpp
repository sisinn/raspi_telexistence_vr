#include "ofApp.h"

//float movie_plane_width = 1280.0;
//float movie_plane_height = 720.0;
//float movie_plane_width = 1472.0;
//float movie_plane_height = 736.0;
float movie_plane_width = 1920.0;
float movie_plane_height = 960.0;
ofMatrix4x4 modelMatrix;

int th_max;
int r_max;
float focal;
float invTmax;
float smooth;
int circle_center_x;
int circle_center_y;
float radius_b[2];
float center_b[2];
float radius_f[2];
float center_f[2];
int wxyz[4];
int front_tilt;
int back_tilt;

string portName = "/dev/ttyACM0";
int baudrate = 115200;

ofVec3f Znormal(0,0,1);
ofVec3f Xnormal(1,0,0);
ofVec3f Ynormal(0,1,0);

void ofApp::onVideoEnd(ofxOMXPlayerListenerEventData& e)
{
//    ofLogVerbose(__func__) << "at: " << ofGetElapsedTimeMillis();
}
void ofApp::onVideoLoop(ofxOMXPlayerListenerEventData& e)
{
//    ofLogVerbose(__func__) << "at: " << ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofVec3f ov[PRIMITIVE_COUNT][VERTEX_COUNT];
    unsigned int oi[PRIMITIVE_COUNT][VERTEX_COUNT];
    ofMatrix4x4 monitor_matrix;

    ofSetVerticalSync(true);
    //ofEnableDepthTest();
    ofBackground(0,128,0);
    ofNoFill();
    ofSetCircleResolution(50);
    ofSetFrameRate(25);
    ofHideCursor();
        
    if(shader.isLoaded()){
        shader.unload();
    }
    shader.load("shader360.vert", "shader360.frag");
    
    //string videoPath = ofToDataPath("/home/pi/openFrameworks/addons/ofxOMXPlayer/video/Timecoded_Big_bunny_1.mov", true);
    //string videoPath = "http://192.168.11.113:8080/?action=stream";
    //string videoPath = "http://192.168.250.4:8080/?action=stream";
    //string videoPath = "udp://192.168.11.66:1234";
    string videoPath = "udp://192.168.250.3:1234";
    
    ofxOMXPlayerSettings settings;
    settings.videoPath = videoPath;
    settings.enableAudio = false;
    settings.videoWidth = movie_plane_width;
    settings.videoHeight = movie_plane_height;
    settings.enableFilters = false;
    settings.enableTexture = true;
    settings.doFlipTexture = false;
    settings.listener = this; //this app extends ofxOMXPlayerListener so it will receive events
    
    omxPlayer.setup(settings);

    initMatrix();

    // Mesh
    for(int i = 0; i < PRIMITIVE_COUNT; i++){
            for(int j = 0; j <= SLICES; j++){
                ov[i][j * 2]       = ofVec3f((float)j / (float)SLICES, (float)i       / (float)STACKS, 0.0);
                ov[i][(j * 2) + 1] = ofVec3f((float)j / (float)SLICES, (float)(i + 1) / (float)STACKS, 0.0);
        }
        for(int j = 0; j < VERTEX_COUNT; j++){
            oi[i][j]       = j;
        }
    }
    monitor_matrix.glScale(ofGetWidth(), ofGetHeight(), 1);
    monitor_matrix.glRotate(0, 0, 0, 1);
    for(int i = 0; i < PRIMITIVE_COUNT; i++){
        createMesh(&monitor_matrix, &(Mesh[i]), ov[i], oi[i], VERTEX_COUNT);
    }

    calcCircle();

    // Serial
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    ofLogNotice("ofApp::setup") << "Connected Devices: ";

    for (std::size_t i = 0; i < devicesInfo.size(); ++i){
        ofLogNotice("ofApp::setup") << "\t" << devicesInfo[i];
    }

    if (!devicesInfo.empty()){
        if(!serial.setup(portName, baudrate)) {
            // log and error and leave
            ofLogError() << "could not open serial port - listing serial devices";
        }
        serial.registerAllEvents(this);
    }
    input_start = 0;
    yellow_state = YELLOW_STATE_STARTUP;
    rot_x = 0;
    rot_y = 0;
    rot_z = 0;
}

void ofApp::exit()
{
    serial.unregisterAllEvents(this);
}

void ofApp::initMatrix(void)
{
    // rotate matrix init
    ofQuaternion qp(0, Xnormal);
    ofQuaternion qy(0, Ynormal);
    ofQuaternion qr(0, Znormal);

    qt = qp * qy * qr;
    sensor_q = qt;
    sensor_q_first_inv = sensor_q.inverse();
}

void ofApp::createMesh(ofMatrix4x4 * mat, ofVboMesh *pMesh, ofVec3f *p_ov, unsigned int *p_oi, int array_max)
{
    pMesh->setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    pMesh->clear();

    //std::printf("DEBUG -- ");
    for (int i = 0; i < array_max; i++) {
        //std::printf("%d ", i);
        pMesh->addVertex(p_ov[i] * *mat);
    }
    //std::printf("\n");

    pMesh->enableIndices();
    pMesh->disableColors();
    pMesh->disableNormals();
    pMesh->disableTextures();

    for (int i = 0; i < array_max; i++) {
        pMesh->addIndex(p_oi[i]);
    }
 }

void ofApp::calcCircle(void)
{
    // 1 / (2 x pi x r x theta_max / 360);
    invTmax = 180.0 / (PI_VALUE * th_max);

    // raius & center
    radius_b[0] = (-0.25) * ((float)r_max / 90.0);
    radius_b[1] = (0.25) * (movie_plane_width / movie_plane_height) * ((float)r_max / 90.0);
    center_b[0] = 0.25  - ((float)circle_center_x / movie_plane_width);
    center_b[1] = 0.25 * (movie_plane_width / movie_plane_height) - ((float)circle_center_y / movie_plane_height);
    radius_f[0] = -radius_b[0];
    radius_f[1] = radius_b[1];
    center_f[0] = 0.5 + center_b[0];
    center_f[1] = center_b[1];
}

//--------------------------------------------------------------
void ofApp::update()
{
    ofMatrix4x4  def_qmatr_inv;
    ofMatrix4x4  qmatr;

    getSerialData(SerialData_Update);
    SerialData_Update.clear();

    ofQuaternion qp(rot_x, Xnormal);
    ofQuaternion qy(rot_y, Ynormal);
    ofQuaternion qr(rot_z, Znormal);
    qt = qp * qy * qr;
    qt.get(modelMatrix);

    sensor_q.get(qmatr);
    sensor_q_first_inv.get(def_qmatr_inv);
    modelMatrix =  modelMatrix * qmatr * def_qmatr_inv;
}

void ofApp::getSerialData(ofxIO::ByteBuffer SerialData)
{
    std::size_t data_size;
    float w, x, y, z;
    float dst[4];

    int reset_flag = 0;
    int key_y = 0;
    int key_left = 0;
    int key_down = 0;
    int key_up = 0;
    int key_right = 0;

    data_size = SerialData.size();
    if(data_size >= 20){
        if(SerialData[0] != 'q'){
            goto read_end;
        }
        if(SerialData[1] != 'u'){
            goto read_end;
        }
        if(SerialData[2] != 'a'){
            goto read_end;
        }
        if(SerialData[3] != 't'){
            goto read_end;
        }
        if(SerialData[4] != '='){
            goto read_end;
        }
        //std::printf("quart Detect OK\n");

        // ----------------------------------------
        {
            int16_t x_int, y_int, z_int, w_int;
            const double scale = (1.0 / (1<<14));

            w_int = (((uint16_t)SerialData[5+1]) << 8) | ((uint16_t)SerialData[5+0]);
            x_int = (((uint16_t)SerialData[5+3]) << 8) | ((uint16_t)SerialData[5+2]);
            y_int = (((uint16_t)SerialData[5+5]) << 8) | ((uint16_t)SerialData[5+4]);
            z_int = (((uint16_t)SerialData[5+7]) << 8) | ((uint16_t)SerialData[5+6]);

            w = scale * w_int;
            x = scale * x_int;
            y = scale * y_int;
            z = scale * z_int;

            //std::printf("w=%f x=%f y=%f z=%f\n", w, x, y, z);
        }

        // ----------------------------------------
        // buttonRed
        reset_flag = SerialData[13];

        // buttonYellow
        key_y = SerialData[14];

        // buttonLeft
        key_left = SerialData[15];

        // buttonDown
        key_down = SerialData[16];

        // buttonUp
        key_up = SerialData[17];

        // buttonRight
        key_right = SerialData[18];

        for(int j = 0; j < 4; j++){
            if(wxyz[j] == 1){
                dst[j] = w;
            }
            else if(wxyz[j] == -1){
                dst[j] = -1.0 * w;
            }
            
            else if(wxyz[j] == 2){
                dst[j] = x;
            }
            else if(wxyz[j] == -2){
                dst[j] = -1.0 * x;
            }
            
            else if(wxyz[j] == 3){
                dst[j] = y;
            }
            else if(wxyz[j] == -3){
                dst[j] = -1.0 * y;
            }

            else if(wxyz[j] == 4){
                dst[j] = z;
            }
            else if(wxyz[j] == -4){
                dst[j] = -1.0 * z;
            }
        }

        if(yellow_state != YELLOW_STATE_PAUSE){
            //default sensor_q.set(x,y,z,w);
            sensor_q.set(dst[1],dst[2],dst[3],dst[0]);
        }

        if(reset_flag == 1){
            sensor_q_first_inv = sensor_q.inverse();
        }
        else if(yellow_state == YELLOW_STATE_STARTUP){
            sensor_q_first_inv = sensor_q.inverse();
            yellow_state = YELLOW_STATE_NOMAL;
        }

read_end:
        ;
        //for(int i = 0; i < data_size; i++){
        //    std::printf("%02x ", SerialData[i]);
        //}
        //std::printf("\n");
    }

    if(key_y == 1){
        yellow_state = (eYELLOW_STATE)((int)yellow_state + 1);
        if(yellow_state == YELLOW_STATE_MAX){
            yellow_state = YELLOW_STATE_NOMAL;
        }
    }
    if(yellow_state == YELLOW_STATE_CROSS){
        if(key_up == 1){
            if(rot_x == 0){
                rot_x = 360;
            }
            rot_x -= 1;
        }
        else if(key_down == 1){
            rot_x += 1;
            if(rot_x == 360){
                rot_x = 0;
            }
        }
        else if(key_left == 1){
            rot_y += 1;
            if(rot_y == 360){
                rot_y = 0;
            }
        }
        else if(key_right == 1){
            if(rot_y == 0){
                rot_y = 360;
            }
            rot_y -= 1;
        }
    }
    else if(yellow_state == YELLOW_STATE_ROTATE){
        if(key_left == 1){
            rot_z += 1;
            if(rot_z == 360){
                rot_z = 0;
            }
        }
        else if(key_right == 1){
            if(rot_z == 0){
                rot_z = 360;
            }
            rot_z -= 1;
        }
    }
    else if(yellow_state == YELLOW_STATE_CLEAR){
        if(key_down == 1){
            rot_x = 0;
            rot_y = 0;
            rot_z = 0;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    //omxPlayer.draw(0.0, 0.0, movie_plane_width, movie_plane_height);

    // PostEffect
    shader.begin();
        shader.setUniformTexture("tex0", omxPlayer.getTextureReference(), omxPlayer.getTextureID());
        shader.setUniform2f("resolution", movie_plane_width, movie_plane_height);
        shader.setUniform2f("screen", ofGetWidth(), ofGetHeight());

        shader.setUniform1f("focal", focal);
        shader.setUniform1f("invTmax", invTmax);
        shader.setUniform1f("smooth", smooth);
        shader.setUniform2fv("radius_b", radius_b);
        shader.setUniform2fv("center_b", center_b);
        shader.setUniform2fv("radius_f", radius_f);
        shader.setUniform2fv("center_f", center_f);
        shader.setUniform1f("rotate_b", (PI_VALUE * (float)back_tilt) / 180.0);
        shader.setUniform1f("rotate_f", (PI_VALUE * (float)front_tilt) / 180.0);

        shader.setUniformMatrix4f("mo", modelMatrix);

        for(int i = 0; i < PRIMITIVE_COUNT; i++){
            Mesh[i].draw();
        }
    shader.end();

    if(yellow_state == YELLOW_STATE_CROSS){
        // Draw +
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        ofSetLineWidth(50);
        ofDrawLine(-300, 0, 300, 0);
        ofDrawLine(0, -300, 0, 300);
        ofSetLineWidth(1);
        ofPopMatrix();
    }
    else if(yellow_state == YELLOW_STATE_ROTATE){
        // Draw O
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        ofSetLineWidth(50);
        ofDrawCircle(0, 0, 300);
        ofSetLineWidth(1);
        ofPopMatrix();
    }
    else if(yellow_state == YELLOW_STATE_PAUSE){
        // Draw []
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        ofSetLineWidth(50);
        ofDrawRectangle(-900, -500, 1800, 1000);
        ofSetLineWidth(1);
        ofPopMatrix();
    }
    else if(yellow_state == YELLOW_STATE_CLEAR){
        // Draw X
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        ofSetLineWidth(50);
        ofDrawLine(-300, -300, 300, 300);
        ofDrawLine(300, -300, -300, 300);
        ofSetLineWidth(1);
        ofPopMatrix();
    }

    /*
    stringstream info;
    info << omxPlayer.getInfo();
    info << "\n";
    ofDrawBitmapStringHighlight(info.str() , 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);
    */
}

void ofApp::onSerialBuffer(const ofxIO::SerialBufferEventArgs& args)
{
    SerialData_Update.clear();
    SerialData_Update = args.buffer();
}


void ofApp::onSerialError(const ofxIO::SerialBufferErrorEventArgs& args)
{
    std::string message;
    std::string exception;

    message = args.buffer().toString();
    exception = args.exception().displayText();

    ofLogNotice("onSerialBuffer") << "got serial buffer : " << message;
    ofLogNotice("onSerialError") << "got serial error : " << exception;
}

