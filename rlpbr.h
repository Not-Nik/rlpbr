// rlpbr (c) Nikolas Wipper 2021

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0, with exemptions for Ramon Santamaria and the
 * raylib contributors who may, at their discretion, instead license
 * any of the Covered Software under the zlib license. If a copy of
 * the MPL was not distributed with this file, You can obtain one
 * at https://mozilla.org/MPL/2.0/. */

#ifndef RLPBR_SRC_RLPBR_H_
#define RLPBR_SRC_RLPBR_H_

#include <raylib.h>

typedef struct Light {
    Vector3 pos;
    Color color;
    float intensity;
    int on;
} Light;

void InitPBR();
void ClosePBR();

void BeginPBR(Camera3D camera);
void EndPBR();

Material LoadPBRMaterial(const char *albedo_path,
                         const char *ao_path,
                         const char *metallic_path,
                         const char *normals_path,
                         const char *roughness_path,
                         TextureFilter filter_mode);

void *AddLight(Light light);
void RemoveLight(void *light);

void SetLight(void *light, Light new);
Light GetLight(void *light);

void EnableLight(void *light);
void DisableLight(void *light);

void UnloadPBRModel(Model pbr);

#endif //RLPBR_SRC_RLPBR_H_
