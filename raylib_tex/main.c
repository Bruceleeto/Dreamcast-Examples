#include <kos.h>
#include <raylib.h>
#include "gl_png.h"


// Model Sourced:
// https://www.models-resource.com/ds_dsi/backatthebarnyardslopbucketgames/model/78565/

#define ATTR_DREAMCAST_WIDTH 640
#define ATTR_DREAMCAST_HEIGHT 480

bool flag = true;
maple_device_t *cont;
cont_state_t *pad_state;
float cameraDistance = 10.0f;

void updateController() {
    cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if(cont) {
        pad_state = (cont_state_t *)maple_dev_status(cont);
        if(!pad_state) {
            printf("Error reading controller\n");
        }
        if(pad_state->buttons & CONT_START) {
            flag = false;
        }
        if(pad_state->buttons & CONT_A) {
            cameraDistance -= 0.2f;
            if(cameraDistance < 1.0f) cameraDistance = 1.0f;
        }
        if(pad_state->buttons & CONT_B) {
            cameraDistance += 0.2f;
        }
    }
}

int main(int argc, char **argv) {
    InitWindow(ATTR_DREAMCAST_WIDTH, ATTR_DREAMCAST_HEIGHT, " Texture Demo");

    // Load just one model and texture
    Model model = LoadModel("/rd/otis.obj");
    Texture2D texture = LoadTextureDTEX("/rd/texture.tex");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    
    Camera3D camera = { 0 };
    camera.position = (Vector3){ cameraDistance, cameraDistance, cameraDistance };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);
    float rotation = 0.0f;

    while (flag) {
        updateController();

        rotation += 0.5f;

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);
            DrawGrid(10, 1.0f);
            DrawModel(model, (Vector3){ 0.0f, 0, 0}, 1.0f, WHITE);
        EndMode3D();

        DrawRectangle(0, 0, ATTR_DREAMCAST_WIDTH, 40, Fade(RAYWHITE, 0.6f));
        DrawText(" Texture Demo", 10, 10, 20, DARKGRAY);
        DrawFPS(ATTR_DREAMCAST_WIDTH - 70, 10);

        EndDrawing();

        camera.position.x = sinf(rotation * DEG2RAD) * cameraDistance;
        camera.position.z = cosf(rotation * DEG2RAD) * cameraDistance;
        camera.position.y = cameraDistance / 2;
    }

    UnloadTexture(texture);
    UnloadModel(model);
    CloseWindow();
    return 0;
}