// rlpbr (c) Nikolas Wipper 2021

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0, with exemptions for Ramon Santamaria and the
 * raylib contributors who may, at their discretion, instead license
 * any of the Covered Software under the zlib license. If a copy of
 * the MPL was not distributed with this file, You can obtain one
 * at https://mozilla.org/MPL/2.0/. */

#include "rlpbr.h"

int main() {
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(1080, 720, "rlpbr");
    InitPBR();

    SetTargetFPS(60);

    Camera3D cam = {
        .position = (Vector3) {1.5f, 1.5f, 1.5f}, .target = (Vector3) {0, 0.75f, 0}, .up = (Vector3) {
            0, 1, 0
        }, .fovy = 90, .projection = CAMERA_PERSPECTIVE
    };
    SetCameraMode(cam, CAMERA_ORBITAL);

    Model helmet = LoadModel("pbr/model/trooper.obj");
    helmet.materials[0] = LoadPBRMaterial("pbr/model/trooper_albedo.png",
                                          "pbr/model/trooper_ao.png",
                                          "pbr/model/trooper_metalness.png",
                                          "pbr/model/trooper_normals.png",
                                          "pbr/model/trooper_roughness.png",
                                          TEXTURE_FILTER_ANISOTROPIC_16X);

    AddLight((Light) {
        .pos = (Vector3) {0, 1, 3}, .color = RED, .target = (Vector3) {0}, .intensity = 20, .type = POINT, .on = 1
    });

    AddLight((Light) {
        .pos = (Vector3) {-2, 5, 0}, .color = YELLOW, .target = (Vector3) {0, 1, 0}, .intensity = 1, .type = SPOT, .on = 1
    });

    AddLight((Light) {
        .pos = (Vector3) {0, 1, 3}, .color = WHITE, .target = (Vector3) {1, -1, 1}, .intensity = 1, .type = SUN, .on = 1
    });

    while (!WindowShouldClose()) {
        UpdateCamera(&cam);

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(cam);
        UpdatePBR(cam);

        DrawModel(helmet, (Vector3) {0, 0, 0}, 1, WHITE);
        DrawGrid(10, 1);

        EndMode3D();

        DrawFPS(0, 0);

        EndDrawing();
    }
    UnloadPBRModel(helmet);

    ClosePBR();
    CloseWindow();

    return 0;
}
