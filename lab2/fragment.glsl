#version 300 es

precision mediump float;

uniform sampler2D textureDiff;

uniform int blendState;

uniform vec3 matSpec, matAmbi, matEmit;
uniform float matSh;
uniform vec3 srcDiff, srcSpec, srcAmbi;

uniform vec3 lightDir;

in vec3 v_normal, v_view;
in vec2 v_texCoord;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 color = texture(textureDiff, v_texCoord).rgb;

    // 2. Implement Phong lighting

    vec3 normal = normalize(v_normal);
    vec3 view = normalize(v_view);
    vec3 light = normalize(lightDir);

    vec3 diff = max(dot(normal, light), 0.0)*srcDiff*color;

    vec3 refl = 2.0*normal*dot(normal, light)-light;
    vec3 spec = pow(max(dot(refl,view), 0.0),matSh)*srcSpec*matSpec;

    vec3 ambi = srcAmbi*matAmbi;

    color = diff+spec+ambi+matEmit;

    float alpha = 1.0f;

    // 4. Implement alpha blending
    if (blendState == 0)
        alpha = texture(textureDiff, v_texCoord).r;
    if (blendState == 1)
        alpha = texture(textureDiff, v_texCoord).g;
    if (blendState == 2)
         alpha = texture(textureDiff, v_texCoord).b;

     fragColor = vec4(color, alpha);
}