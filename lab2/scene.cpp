#include "scene.h"
#include "binary/teapot.h"
#include "binary/rgb.h"
#include "binary/cloud.h"
#include "binary/tex_flower.h"
#include "checker.h"

Shader* Scene::vertexShader = nullptr;
Shader* Scene::fragmentShader = nullptr;
Program* Scene::program = nullptr;
Camera* Scene::camera = nullptr;
Object* Scene::teapot = nullptr;
Texture* Scene::diffuse = nullptr;
Texture* Scene::dissolve = nullptr;
Material* Scene::material = nullptr;
Light* Scene::light = nullptr;

void Scene::setup(AAssetManager* aAssetManager) {
    Asset::setManager(aAssetManager);

    Scene::vertexShader = new Shader(GL_VERTEX_SHADER, "vertex.glsl");
    Scene::fragmentShader = new Shader(GL_FRAGMENT_SHADER, "fragment.glsl");

    Scene::program = new Program(Scene::vertexShader, Scene::fragmentShader);

    Scene::camera = new Camera(Scene::program);
    Scene::camera->eye = vec3(20.0f, 30.0f, 20.0f);

    Scene::light = new Light(program);
    Scene::light->direction = vec3(0.0f, 0.6f, 1.0f);

    // 3. Change the texture to RGB texture
    Scene::diffuse  = new Texture(Scene::program, 0, "textureDiff", rgbTexels, rgbSize);
    Scene::material = new Material(Scene::program, diffuse);
    Scene::teapot = new Object(program, material, teapotVertices, teapotIndices);
}

void Scene::screen(int width, int height) {
    Scene::camera->aspect = (float) width/height;
}

void Scene::update(float deltaTime) {
    static float time = 0.0f;

    Scene::program->use();

    // 2. Implement Phong lighting
    time += deltaTime;
    Scene::light->direction = vec3(1.0f * sin(time), 0.6f, 1.0f * cos(time));


    // 4. Implement alpha blending
    // Compute current alpha blending state and assign it to uniform variable

    GLint blendLoc = glGetUniformLocation(program->get(), "blendState");

    if(fmod(0.5f *time, 3.0f) < 1.0){
        glUniform1i(blendLoc, 0);
    }
    else if(fmod(0.5f * time, 3.0f) > 2.0){
        glUniform1i(blendLoc, 2);
    }
    else{
        glUniform1i(blendLoc, 1);
    }



    //GLint lightDirLoc = glGetUniformLocation(program->get(), "lightDir");
    //set
    //if (lightDirLoc >= 0) glUniform3fv(lightDirLoc, 1, value_ptr(direction));
    //else LOG_PRINT_ERROR("Fail to find uniform location: %s", "lightDir");

    Scene::camera->update();
    Scene::light->update();

    Scene::teapot->draw();
}