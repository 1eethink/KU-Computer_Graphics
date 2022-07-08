#include "scene.h"
#include "binary/animation.h"
#include "binary/skeleton.h"
#include "binary/player.h"

Shader* Scene::vertexShader = nullptr;
Shader* Scene::fragmentShader = nullptr;
Program* Scene::program = nullptr;
Camera* Scene::camera = nullptr;
Object* Scene::player = nullptr;
Texture* Scene::diffuse = nullptr;
Material* Scene::material = nullptr;
Object* Scene::lineDraw = nullptr;
Texture* Scene::lineColor = nullptr;
Material* Scene::lineMaterial = nullptr;

void Scene::setup(AAssetManager* aAssetManager) {
    Asset::setManager(aAssetManager);

    Scene::vertexShader = new Shader(GL_VERTEX_SHADER, "vertex.glsl");
    Scene::fragmentShader = new Shader(GL_FRAGMENT_SHADER, "fragment.glsl");

    Scene::program = new Program(Scene::vertexShader, Scene::fragmentShader);

    Scene::camera = new Camera(Scene::program);
    Scene::camera->eye = vec3(0.0f, 0.0f, 80.0f);

    Scene::diffuse = new Texture(Scene::program, 0, "textureDiff", playerTexels, playerSize);
    Scene::material = new Material(Scene::program, diffuse);
    Scene::player = new Object(program, material, playerVertices, playerIndices);
    player->worldMat = scale(vec3(1.0f / 3.0f));

    Scene::lineColor = new Texture(Scene::program, 0, "textureDiff", {{0xFF, 0x00, 0x00}}, 1);
    Scene::lineMaterial = new Material(Scene::program, lineColor);
    Scene::lineDraw = new Object(program, lineMaterial,{{}}, {{}},  GL_LINES);
    lineDraw->worldMat = scale(vec3(1.0f / 3.0f));
}

void Scene::screen(int width, int height) {
    Scene::camera->aspect = (float) width/height;
}



bool mouseDown = false;

void Scene::update(float deltaTime) {
    static float time = 0.0f;


    mat4 M_p[28];

    for(int i=0;i<=27;i++){
        M_p[i] = translate(mat4(1.0f), vec3(jOffsets[i][0],jOffsets[i][1],jOffsets[i][2]));
    }


    //LOG_PRINT_DEBUG("You can also debug variables with this function: %f", M_p[2][3][1]);


    mat4 M_d[28];
    M_d[0] = M_p[0];

    for(int j=1;j<=27;j++){
        M_d[j] = M_d[jParents[j]] * M_p[j];
    }


    mat4 rotate_x, rotate_y, rotate_z;

    mat4 M_l_key0[28];
    mat4 M_l_key1[28];
    mat4 M_l_key2[28];
    mat4 M_l_key3[28];
    mat4 M_temp_rotate[28];

        for (int k = 0; k <= 27; k++) {
            rotate_y = rotate(radians(motions[0][3 * k + 4]), vec3(0.0f, 1.0, 0.0f));

            rotate_x = rotate(radians(motions[0][3 * k + 3]), vec3(1.0f, 0.0f, 0.0f));

            rotate_z = rotate(radians(motions[0][3 * k + 5]), vec3(0.0f, 0.0f, 1.0f));

            M_l_key0[k] = rotate_z * rotate_x * rotate_y;
        }


        for (int k = 0; k <= 27; k++) {
            rotate_y = rotate(radians(motions[1][3 * k + 4]), vec3(0.0f, 1.0, 0.0f));

            rotate_x = rotate(radians(motions[1][3 * k + 3]), vec3(1.0f, 0.0f, 0.0f));

            rotate_z = rotate(radians(motions[1][3 * k + 5]), vec3(0.0f, 0.0f, 1.0f));


            M_l_key1[k] = rotate_z * rotate_x * rotate_y;
        }


        for (int k = 0; k <= 27; k++) {
            rotate_y = rotate(radians(motions[2][3 * k + 4]), vec3(0.0f, 1.0, 0.0f));

            rotate_x = rotate(radians(motions[2][3 * k + 3]), vec3(1.0f, 0.0f, 0.0f));

            rotate_z = rotate(radians(motions[2][3 * k + 5]), vec3(0.0f, 0.0f, 1.0f));


            M_l_key2[k] = rotate_z * rotate_x * rotate_y;
        }


        for (int k = 0; k <= 27; k++) {
            rotate_y = rotate(radians(motions[3][3 * k + 4]), vec3(0.0f, 1.0, 0.0f));

            rotate_x = rotate(radians(motions[3][3 * k + 3]), vec3(1.0f, 0.0f, 0.0f));

            rotate_z = rotate(radians(motions[3][3 * k + 5]), vec3(0.0f, 0.0f, 1.0f));


            M_l_key3[k] = rotate_z * rotate_x * rotate_y;
        }

    if (time == 0.0f) { //0
        //motions[0]; //motion 0 하고 1 interpolate -> t = fmod(time, 4.0f) 이용
        for (int j = 0; j < 28; j++) {
            M_temp_rotate[j] = translate(mat4(1.0f),
                                         vec3((1.0f - fmod(time, 4.0f)) * M_p[j][3][0] +
                                              fmod(time, 4.0f) * M_p[j][3][0],
                                              (1.0f - fmod(time, 4.0f)) * M_p[j][3][1] +
                                              fmod(time, 4.0f) * M_p[j][3][1],
                                              (1.0f - fmod(time, 4.0f)) * M_p[j][3][2] +
                                              fmod(time, 4.0f) * M_p[j][3][2]))
                               * mat4_cast(
                    mix(quat_cast(M_l_key0[j]), quat_cast(M_l_key1[j]), fmod(time, 4.0f)));

        }

        mat4 M_a[28];
        M_a[0] = M_temp_rotate[0];

        for (int j = 1; j <= 27; j++) {
            M_a[j] = M_a[jParents[j]] * M_temp_rotate[j];
        }

        vec3 temp_pos0, temp_pos1, temp_pos2, temp_pos3;

        for (int a = 0; a <= 2811; a++) {
            if (playerVertices[a].bone[0] != -1) {
                temp_pos0 = playerVertices[a].weight[0] *
                            vec3(M_a[playerVertices[a].bone[0]] *
                                 inverse(M_d[playerVertices[a].bone[0]]) *
                                 vec4(playerVertices[a].pos, 1.0f));
            } else {
                temp_pos0 = vec3(0.0f, 0.0f, 0.0f);
            }
            if (playerVertices[a].bone[1] != -1) {
                temp_pos1 = playerVertices[a].weight[1] *
                            vec3(M_a[playerVertices[a].bone[1]] *
                                 inverse(M_d[playerVertices[a].bone[1]]) *
                                 vec4(playerVertices[a].pos, 1.0f));
            } else {
                temp_pos1 = vec3(0.0f, 0.0f, 0.0f);
            }
            if (playerVertices[a].bone[2] != -1) {
                temp_pos2 = playerVertices[a].weight[2] *
                            vec3(M_a[playerVertices[a].bone[2]] *
                                 inverse(M_d[playerVertices[a].bone[2]]) *
                                 vec4(playerVertices[a].pos, 1.0f));
            } else {
                temp_pos2 = vec3(0.0f, 0.0f, 0.0f);
            }
            if (playerVertices[a].bone[3] != -1) {
                temp_pos3 = playerVertices[a].weight[3] * vec3(M_a[playerVertices[a].bone[3]] *
                                                               inverse(M_d[playerVertices[a].bone[3]]) *
                                                               vec4(playerVertices[a].pos, 1.0f));
            } else {
                temp_pos3 = vec3(0.0f, 0.0f, 0.0f);
            }

            player->vertices[a].pos = temp_pos0 + temp_pos1 + temp_pos2 + temp_pos3;
        }
    }



    if(mouseDown) {

        time += deltaTime;

        mat4 M_temp_rotate[28];


        if (fmod(time, 4.0f) < 1.0f) { //0
            //motions[0]; //motion 0 하고 1 interpolate -> t = fmod(time, 4.0f) 이용
            for (int j = 0; j < 28; j++) {
                M_temp_rotate[j] = translate(mat4(1.0f),
                                             vec3((1.0f - fmod(time, 4.0f)) * M_p[j][3][0] +
                                                  fmod(time, 4.0f) * M_p[j][3][0],
                                                  (1.0f - fmod(time, 4.0f)) * M_p[j][3][1] +
                                                  fmod(time, 4.0f) * M_p[j][3][1],
                                                  (1.0f - fmod(time, 4.0f)) * M_p[j][3][2] +
                                                  fmod(time, 4.0f) * M_p[j][3][2]))
                                   * mat4_cast(
                        mix(quat_cast(M_l_key0[j]), quat_cast(M_l_key1[j]), fmod(time, 4.0f)));

            }


        } else if ((fmod(time, 4.0f) > 1.0f) && (fmod(time, 4.0f) < 2.0f)) { //1
            //motions[1]; //motion 1 하고 2 interpolate -> t = fmod(time, 4.0f) - 1.0f  이용
            for (int j = 0; j < 28; j++) {
                M_temp_rotate[j] = translate(mat4(1.0f), vec3((1.0f - (fmod(time, 4.0f) - 1.0f)) *
                                                              M_p[j][3][0] +
                                                              (fmod(time, 4.0f) - 1.0f) *
                                                              M_p[j][3][0],
                                                              (1.0f - (fmod(time, 4.0f) - 1.0f)) *
                                                              M_p[j][3][1] +
                                                              (fmod(time, 4.0f) - 1.0f) *
                                                              M_p[j][3][1],
                                                              (1.0f - (fmod(time, 4.0f) - 1.0f)) *
                                                              M_p[j][3][2] +
                                                              (fmod(time, 4.0f) - 1.0f) *
                                                              M_p[j][3][2]))
                                   * mat4_cast(mix(quat_cast(M_l_key1[j]), quat_cast(M_l_key2[j]),
                                                   (fmod(time, 4.0f) - 1.0f)));
            }


        } else if ((fmod(time, 4.0f) > 2.0f) && (fmod(time, 4.0f) < 3.0f)) { //2
            // motions[2]; //motion 2 하고 3 interpolate -> t = fmod(time, 4.0f) -2.0f 이용
            for (int j = 0; j < 28; j++) {
                M_temp_rotate[j] = translate(mat4(1.0f), vec3((1.0f - (fmod(time, 4.0f) - 2.0f)) *
                                                              M_p[j][3][0] +
                                                              (fmod(time, 4.0f) - 2.0f) *
                                                              M_p[j][3][0],
                                                              (1.0f - (fmod(time, 4.0f) - 2.0f)) *
                                                              M_p[j][3][1] +
                                                              (fmod(time, 4.0f) - 2.0f) *
                                                              M_p[j][3][1],
                                                              (1.0f - (fmod(time, 4.0f) - 2.0f)) *
                                                              M_p[j][3][2] +
                                                              (fmod(time, 4.0f) - 2.0f) *
                                                              M_p[j][3][2]))
                                   * mat4_cast(mix(quat_cast(M_l_key2[j]), quat_cast(M_l_key3[j]),
                                                   (fmod(time, 4.0f) - 2.0f)));
            }

        } else { //3
            // motions[3]; //motion 3 하고 0 interpolate -> t = fmod(time, 4.0f) - 3.0f 이용
            for (int j = 0; j < 28; j++) {
                M_temp_rotate[j] = translate(mat4(1.0f), vec3((1.0f - (fmod(time, 4.0f) - 3.0f)) *
                                                              M_p[j][3][0] +
                                                              (fmod(time, 4.0f) - 3.0f) *
                                                              M_p[j][3][0],
                                                              (1.0f - (fmod(time, 4.0f) - 3.0f)) *
                                                              M_p[j][3][1] +
                                                              (fmod(time, 4.0f) - 3.0f) *
                                                              M_p[j][3][1],
                                                              (1.0f - (fmod(time, 4.0f) - 3.0f)) *
                                                              M_p[j][3][2] +
                                                              (fmod(time, 4.0f) - 3.0f) *
                                                              M_p[j][3][2]))
                                   * mat4_cast(mix(quat_cast(M_l_key3[j]), quat_cast(M_l_key0[j]),
                                                   (fmod(time, 4.0f) - 3.0f)));
            }


        }
        mat4 M_a[28];
        M_a[0] = M_temp_rotate[0];

        for (int j = 1; j <= 27; j++) {
            M_a[j] = M_a[jParents[j]] * M_temp_rotate[j];
        }


        vec3 temp_pos0, temp_pos1, temp_pos2, temp_pos3;

        for (int a = 0; a <= 2811; a++) {
            if (playerVertices[a].bone[0] != -1) {
                temp_pos0 = playerVertices[a].weight[0] *
                            vec3(M_a[playerVertices[a].bone[0]] *
                                 inverse(M_d[playerVertices[a].bone[0]]) *
                                 vec4(playerVertices[a].pos, 1.0f));
            } else {
                temp_pos0 = vec3(0.0f, 0.0f, 0.0f);
            }
            if (playerVertices[a].bone[1] != -1) {
                temp_pos1 = playerVertices[a].weight[1] *
                            vec3(M_a[playerVertices[a].bone[1]] *
                                 inverse(M_d[playerVertices[a].bone[1]]) *
                                 vec4(playerVertices[a].pos, 1.0f));
            } else {
                temp_pos1 = vec3(0.0f, 0.0f, 0.0f);
            }
            if (playerVertices[a].bone[2] != -1) {
                temp_pos2 = playerVertices[a].weight[2] *
                            vec3(M_a[playerVertices[a].bone[2]] *
                                 inverse(M_d[playerVertices[a].bone[2]]) *
                                 vec4(playerVertices[a].pos, 1.0f));
            } else {
                temp_pos2 = vec3(0.0f, 0.0f, 0.0f);
            }
            if (playerVertices[a].bone[3] != -1) {
                temp_pos3 = playerVertices[a].weight[3] * vec3(M_a[playerVertices[a].bone[3]] *
                                                               inverse(M_d[playerVertices[a].bone[3]]) *
                                                               vec4(playerVertices[a].pos, 1.0f));
            } else {
                temp_pos3 = vec3(0.0f, 0.0f, 0.0f);
            }

            player->vertices[a].pos = temp_pos0 + temp_pos1 + temp_pos2 + temp_pos3;
        }

    }




    Scene::program->use();

    Scene::camera->update();

    Scene::player->load(player->vertices, playerIndices);
    Scene::player->draw();

/*
    for(int i = 1; i < 28; i++){
        joint[i].pos = jOffsets[i];
        joint[i].pos += joint[jParents[i]].pos;
    }
*/
    // Line Drawer & Debugger
    glLineWidth(20);
    //Scene::lineDraw->load({{vec3(-20.0f, 0.0f, 0.0f)}, {vec3(20.0f, 0.0f, 0.0f)}}, {0, 1});
    //Scene::lineDraw->load(joint,jointIndices);
    //Scene::lineDraw->draw();



}

void Scene::mouseUpEvents(float x, float y, bool doubleTouch) {
    mouseDown = false;
}

void Scene::mouseDownEvents(float x, float y, bool doubleTouch) {
    mouseDown = true;
}

void Scene::mouseMoveEvents(float x, float y, bool doubleTouch) {

}