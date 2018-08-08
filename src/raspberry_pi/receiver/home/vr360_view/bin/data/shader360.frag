precision mediump float;

// texture coordinates
varying vec2 texcoord_b;
varying vec2 texcoord_f;

// texture mixing ratio
varying float blend;

uniform sampler2D   tex0;

void main()
{
    vec4 color_b = texture2D(tex0, texcoord_b);
    vec4 color_f = texture2D(tex0, texcoord_f);

    gl_FragColor = mix(color_f, color_b, blend);
}

