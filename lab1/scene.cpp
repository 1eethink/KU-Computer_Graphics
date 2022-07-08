#include "scene.h"

#include "obj_teapot.h"
#include "tex_flower.h"

Shader* Scene::vertexShader = nullptr;
Shader* Scene::fragmentShader = nullptr;
Program* Scene::program = nullptr;
Camera* Scene::camera = nullptr;
Light* Scene::light = nullptr;
Object* Scene::teapot = nullptr;
Material* Scene::flower = nullptr;

void Scene::setup(AAssetManager* aAssetManager) {

    // set asset manager
    Asset::setManager(aAssetManager);

    // create shaders
    vertexShader = new Shader(GL_VERTEX_SHADER, "vertex.glsl");
    fragmentShader = new Shader(GL_FRAGMENT_SHADER, "fragment.glsl");

    // create program
    program = new Program(vertexShader, fragmentShader);

    // create camera
    camera = new Camera(program);
    camera->eye = vec3(60.0f, 00.0f, 0.0f);

    // create light
    light = new Light(program);
    light->position = vec3(100.0f, 0.0f, 0.0f);

    // create floral texture
    flower = new Material(program, texFlowerData, texFlowerSize);

    // create teapot object
    teapot = new Object(program, flower, objTeapotVertices, objTeapotIndices,
                        objTeapotVerticesSize, objTeapotIndicesSize);

    mat4 scaleM, eulerX, rotMat;
    scaleM = transpose(mat4(1.0f, 0.0f, 0.0f, 0.0f,  //In OpenGL, the matrix must be transposed
                            0.0f, 2.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f));

    eulerX = transpose(mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                      0.0f, 1.0f / sqrt(2.0f), -1.0f / sqrt(2.0f), 0.0f,
                                      0.0f, 1.0f / sqrt(2.0f), 1.0f / sqrt(2.0f), 0.0f,
                                      0.0f, 0.0f, 0.0f, 1.0f));
    rotMat = transpose(mat4(0.5f, -1.0f / sqrt(2.0f), 0.5f, 0.0f,
                                    1.0f / sqrt(2.0f), 0.0f, -1.0f / sqrt(2.0f), 0.0f,
                                    0.5f, 1.0f / sqrt(2.0f), 0.5f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f));
    teapot->worldMatrix = rotMat*eulerX*scaleM;
}

void Scene::screen(int width, int height) {

    // set camera aspect ratio
    camera->aspect = (float) width / height;
}

void Scene::update(float deltaTime) {
    static float time = 0.0f;

    // use program
    program->use();

    // rotate the camera relative to the object
    time += deltaTime;
    camera->eye = vec3(60.0f * sin(time), 30.0f * sin(time), -60.0f * cos(time));

    // setup camera and light
    camera->setup();
    light->setup();

    // draw teapot
    teapot->draw();
}
