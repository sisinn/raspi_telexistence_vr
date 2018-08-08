#include "ofMain.h"
#include "ofApp.h"
#include "unistd.h"

int main(int argc, char *argv[])
{
    int opt;
    int qopt = 0;
    char qparam[128] = "";
    int topt = 0;
    char tparam[128] = "";
    int fopt = 0;
    char fparam[128] = "";
    int ropt = 0;
    char rparam[128] = "";
    int sopt = 0;
    char sparam[128] = "";
    int sum_chk = 0;

    // parameter q: quatanion sort select
    // parameter f: focal
    // parameter t: theta
    // parameter r: radius
    // parameter s: smooth
    while ((opt = getopt(argc, argv, "q:f:t:r:s:")) != -1) {
        switch (opt) {
            case 'q':
                qopt = 1;
                if(strlen(optarg) < 128){
                    strcpy(qparam, optarg);
                }
                break;

            case 't':
                topt = 1;
                if(strlen(optarg) < 128){
                    strcpy(tparam, optarg);
                }
                break;

            case 'f':
                fopt = 1;
                if(strlen(optarg) < 128){
                    strcpy(fparam, optarg);
                }
                break;

            case 'r':
                ropt = 1;
                if(strlen(optarg) < 128){
                    strcpy(rparam, optarg);
                }
                break;

            case 's':
                sopt = 1;
                if(strlen(optarg) < 128){
                    strcpy(sparam, optarg);
                }
            break;

            default:
                printf("error! \'%c\' \'%c\'\n", opt, optopt);
                return 1;
        }
    }

    if(qopt == 1){
        char *p[4];
        int err_flag = 0;

        p[0] = strtok(qparam,",");
        if(p[0] == NULL){
            err_flag = 1;
        }
        else{
            for(int i = 1; i < 4; i++){
                p[i] = strtok(NULL,",");
                if(p[i] == NULL){
                    err_flag = 1;
                    break;
                }
            }
        }

        if(err_flag == 0){
            for(int i = 0; i < 4; i++){
                if(p[i][0] == 'w'){
                    wxyz[i] = 1;
                    sum_chk += 1;
                }
                else if((p[i][0] == '_') && (p[i][1] == 'w')){
                    wxyz[i] = -1;
                    sum_chk += 1;
                }
                else if(p[i][0] == 'x'){
                    wxyz[i] = 2;
                    sum_chk += 2;
                }
                else if((p[i][0] == '_') && (p[i][1] == 'x')){
                    wxyz[i] = -2;
                    sum_chk += 2;
                }
                else if(p[i][0] == 'y'){
                    wxyz[i] = 3;
                    sum_chk += 4;
                }
                else if((p[i][0] == '_') && (p[i][1] == 'y')){
                    wxyz[i] = -3;
                    sum_chk += 4;
                }
                else if(p[i][0] == 'z'){
                    wxyz[i] = 4;
                    sum_chk += 8;
                }
                else if((p[i][0] == '_') && (p[i][1] == 'z')){
                    wxyz[i] = -4;
                    sum_chk += 8;
                }
            }
        }
    }
    if(sum_chk != 15){
        wxyz[0] = 1;
        wxyz[1] = 2;
        wxyz[2] = 3;
        wxyz[3] = 4;
    }
    printf("wxyz= %d %d %d %d\n", wxyz[0],wxyz[1],wxyz[2],wxyz[3]);

    th_max = 90;
    if(topt == 1){
        th_max = atoi(tparam);
        if((th_max < 60) || (th_max > 120)){
            th_max = 90;
        }
    }

    focal = 1.0;
    if(fopt == 1){
        focal = atof(fparam);
        if((focal < 0.5) || (focal > 3.0)){
            focal = 1.0;
        }
    }

    r_max = 90;
    if(ropt == 1){
        r_max = atoi(rparam);
        if((r_max < 60) || (r_max > 120)){
            r_max = 90;
        }
    }

    smooth = 0.02;
    if(sopt == 1){
        smooth = atof(sparam);
        if((smooth < 0.001) || (smooth > 0.1)){
            smooth = 0.02;
        }
    }

    printf("th_max = %d, r_max = %d, focal = %f, smooth = %f\n", th_max, r_max, focal, smooth);
    circle_center_x = 0.0;
    circle_center_y = 0.0;
    printf("circle_center = %d %d\n", circle_center_x, circle_center_y);

    ofSetLogLevel(OF_LOG_VERBOSE);

    ofGLESWindowSettings settings;
    settings.setSize(SCREEN_WIDTH, SCREEN_HIGH);
    settings.setGLESVersion(2);
    settings.windowMode = OF_FULLSCREEN;

    ofCreateWindow(settings);
    ofRunApp( new ofApp());
}

