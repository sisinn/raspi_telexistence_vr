precision mediump float;

attribute vec4 position;                // set automatically by OF
uniform mat4 modelViewProjectionMatrix; // set automatically by OF 

// texture width x height
uniform vec2        resolution;
// monitor width x height
uniform vec2        screen;

//#define SINGLE_VIEW_MODE

vec2 screen_tmp;
vec2 position_tmp;

// HMD rotate
uniform mat4 mo;

// monitor depth
uniform float focal;
// invert Theta Max
uniform float invTmax;
// mixing ratio
uniform float smooth;

// camera circle(backward)
uniform vec2 radius_b;
uniform vec2 center_b;
// camera circle(forward)
uniform vec2 radius_f;
uniform vec2 center_f;

uniform float rotate_b;
uniform float rotate_f;

// texture coordinates
varying vec2 texcoord_b;
varying vec2 texcoord_f;

// texture mixing ratio
varying  float blend;

void main()
{
    gl_Position = modelViewProjectionMatrix * position;

#ifdef SINGLE_VIEW_MODE
    screen_tmp = vec2(screen.x, screen.y);
    position_tmp = vec2(position.x, position.y);
#else
    screen_tmp = vec2(screen.x / 2.0, screen.y);
    if(position.x >= (screen.x / 2.0)){
        position_tmp = vec2(position.x - (screen.x / 2.0), position.y);
    }
    else{
        position_tmp = vec2(position.x, position.y);
    }
#endif

    vec2 p = vec2((position_tmp.x - (screen_tmp.x / 2.0)) / screen_tmp.y , (position_tmp.y - (screen_tmp.y / 2.0)) / screen_tmp.y);
    vec4 vr_vector = mo * normalize(vec4(p, -focal, 1.0));

    float angle = 1.0 - (acos(vr_vector.z) * invTmax);
    vec2 orientation = vr_vector.xy;

    mat2 m_rotate_b = mat2(cos(rotate_b), sin(rotate_b), -sin(rotate_b), cos(rotate_b));
    mat2 m_rotate_f = mat2(cos(rotate_f), sin(rotate_f), -sin(rotate_f), cos(rotate_f));

    texcoord_b = (1.0 - angle) * m_rotate_b * orientation;
    texcoord_b = texcoord_b * radius_b;
    texcoord_b = texcoord_b + center_b;
    texcoord_f = (1.0 + angle) * m_rotate_f * orientation;
    texcoord_f = texcoord_f * radius_f;
    texcoord_f = texcoord_f + center_f;

    blend = smoothstep(-1.0 * smooth, smooth, angle);
}

