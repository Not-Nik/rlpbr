// rlpbr (c) Nikolas Wipper 2021

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0, with exemptions for Ramon Santamaria and the
 * raylib contributors who may, at their discretion, instead license
 * any of the Covered Software under the zlib license. If a copy of
 * the MPL was not distributed with this file, You can obtain one
 * at https://mozilla.org/MPL/2.0/. */

#include "rlpbr.h"

#include "shaders.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Shader pbr_shader;

typedef struct pbr_internal_light {
    float pos[3];
    float color[3];
    float intensity;
    int on;
    struct pbr_internal_light *next, *prev;
} pbr_internal_light;

pbr_internal_light *lights = NULL;
pbr_internal_light empty = {0};

void InitPBR() {

    #ifdef BUNDLE_SHADERS
    pbr_shader = LoadShaderFromMemory(pbr_vs, pbr_fs);
    #else
    pbr_shader = LoadShader("pbr/shader/pbr.vs", "pbr/shader/pbr.fs");
    #endif

    pbr_shader.locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(pbr_shader, "albedoMap");
    pbr_shader.locs[SHADER_LOC_MAP_NORMAL] = GetShaderLocation(pbr_shader, "normalMap");
    pbr_shader.locs[SHADER_LOC_MAP_METALNESS] = GetShaderLocation(pbr_shader, "metallicMap");
    pbr_shader.locs[SHADER_LOC_MAP_ROUGHNESS] = GetShaderLocation(pbr_shader, "roughnessMap");
    pbr_shader.locs[SHADER_LOC_MAP_OCCLUSION] = GetShaderLocation(pbr_shader, "aoMap");

    pbr_shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(pbr_shader, "model");
    pbr_shader.locs[SHADER_LOC_MATRIX_VIEW] = GetShaderLocation(pbr_shader, "view");
    pbr_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(pbr_shader, "camPos");
    pbr_shader.locs[SHADER_LOC_MATRIX_PROJECTION] = GetShaderLocation(pbr_shader, "projection");
}

void ClosePBR() {
    UnloadShader(pbr_shader);
}

Material LoadPBRMaterial(const char *albedo_path,
                         const char *ao_path,
                         const char *metallic_path,
                         const char *normals_path,
                         const char *roughness_path,
                         TextureFilter filter_mode) {
    Material mat = LoadMaterialDefault();
    mat.shader = pbr_shader;

    mat.maps[MATERIAL_MAP_ALBEDO].texture = LoadTexture(albedo_path);
    mat.maps[MATERIAL_MAP_OCCLUSION].texture = LoadTexture(ao_path);
    mat.maps[MATERIAL_MAP_METALNESS].texture = LoadTexture(metallic_path);
    mat.maps[MATERIAL_MAP_NORMAL].texture = LoadTexture(normals_path);
    mat.maps[MATERIAL_MAP_ROUGHNESS].texture = LoadTexture(roughness_path);

    SetTextureFilter(mat.maps[MATERIAL_MAP_ALBEDO].texture, filter_mode);
    SetTextureFilter(mat.maps[MATERIAL_MAP_NORMAL].texture, filter_mode);
    SetTextureFilter(mat.maps[MATERIAL_MAP_METALNESS].texture, filter_mode);
    SetTextureFilter(mat.maps[MATERIAL_MAP_ROUGHNESS].texture, filter_mode);
    SetTextureFilter(mat.maps[MATERIAL_MAP_OCCLUSION].texture, filter_mode);

    mat.maps[MATERIAL_MAP_ALBEDO].color = (Color) {255, 255, 255, 255};
    mat.maps[MATERIAL_MAP_NORMAL].color = (Color) {255, 255, 255, 255};
    mat.maps[MATERIAL_MAP_METALNESS].value = 1.0f;
    mat.maps[MATERIAL_MAP_ROUGHNESS].value = 1.0f;
    mat.maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
    mat.maps[MATERIAL_MAP_EMISSION].value = 0.5f;
    mat.maps[MATERIAL_MAP_HEIGHT].value = 0.5f;

    return mat;
}

void UpdateLightAt(pbr_internal_light *light, int index) {
    char loc_str[32];

    sprintf(loc_str, "lights[%i].pos", index);
    SetShaderValue(pbr_shader, GetShaderLocation(pbr_shader, loc_str), light->pos, SHADER_UNIFORM_VEC3);

    sprintf(loc_str, "lights[%i].color", index);
    SetShaderValue(pbr_shader, GetShaderLocation(pbr_shader, loc_str), light->color, SHADER_UNIFORM_VEC3);

    sprintf(loc_str, "lights[%i].intensity", index);
    SetShaderValue(pbr_shader, GetShaderLocation(pbr_shader, loc_str), &light->intensity, SHADER_UNIFORM_FLOAT);

    sprintf(loc_str, "lights[%i].on", index);
    SetShaderValue(pbr_shader, GetShaderLocation(pbr_shader, loc_str), &light->on, SHADER_UNIFORM_INT);
}

void UpdateLight(pbr_internal_light *light) {
    pbr_internal_light *cur = lights;
    int i = 0;
    while (cur && cur != light) {
        cur = cur->next;
        i++;
    }
    if (cur) UpdateLightAt(cur, i);
}

void UpdateAllLights(bool clear_last) {
    pbr_internal_light *cur = lights;
    int i = 0;
    while (cur) {
        UpdateLightAt(cur, i);
        cur = cur->next;
        i++;
    }

    if (clear_last) {
        UpdateLightAt(&empty, i);
    }
}

void *AddLight(Light light) {
    pbr_internal_light *cur = NULL;
    int i = 0;

    if (!lights) {
        lights = RL_CALLOC(1, sizeof(pbr_internal_light));
        cur = lights;
    } else {
        while (cur->next) {
            cur = cur->next;
            i++;
        }

        cur->next = RL_CALLOC(1, sizeof(pbr_internal_light));
        cur->next->prev = cur;
        cur = cur->next;
    }

    cur->pos[0] = light.pos.x;
    cur->pos[1] = light.pos.y;
    cur->pos[2] = light.pos.z;

    cur->color[0] = (float) light.color.r / 255.f;
    cur->color[1] = (float) light.color.g / 255.f;
    cur->color[2] = (float) light.color.b / 255.f;

    cur->intensity = light.intensity;
    cur->on = 1;

    UpdateLightAt(cur, i);
    return cur;
}

void RemoveLight(void *_light) {
    pbr_internal_light *light = _light;
    light->prev->next = light->next;
    light->next->prev = light->prev;

    UpdateAllLights(true);
}

void SetOn(void *_light, int on) {
    pbr_internal_light *light = _light;
    light->on;
    UpdateLight(light);
}

void EnableLight(void *_light) {
    SetOn(_light, 1);
}

void DisableLight(void *_light) {
    SetOn(_light, 0);
}

void UnloadPBRModel(Model pbr) {
    // NOTE: Not unloading shader, because it's shared across models

    UnloadTexture(pbr.materials[0].maps[MATERIAL_MAP_ALBEDO].texture);
    UnloadTexture(pbr.materials[0].maps[MATERIAL_MAP_OCCLUSION].texture);
    UnloadTexture(pbr.materials[0].maps[MATERIAL_MAP_METALNESS].texture);
    UnloadTexture(pbr.materials[0].maps[MATERIAL_MAP_NORMAL].texture);
    UnloadTexture(pbr.materials[0].maps[MATERIAL_MAP_ROUGHNESS].texture);

    UnloadModel(pbr);
}
