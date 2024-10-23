//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-23 14:27:42
//

#include <cgltf.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "model.h"
#include "util.h"
#include "bitmap.h"
#include "cmd_list.h"

#define cgltf_call(call) if (call != cgltf_result_success) { throw_error("Mesh Error", #call, -1); }

void cgltf_process_primitive(cgltf_primitive* cgltf_primitive, u32* primitive_index, model_t* m, HMM_Mat4 transform, gpu_t *gpu)
{
    if (cgltf_primitive->type != cgltf_primitive_type_triangles)
        return;

    submesh_t* submesh = &m->submeshes[(*primitive_index)++];
    submesh->transform = transform;

    cgltf_attribute* position_attribute = 0;
    cgltf_attribute* texcoord_attribute = 0;
    cgltf_attribute* normal_attribute = 0;

    for (i32 attribute_index = 0; attribute_index < cgltf_primitive->attributes_count; attribute_index++) {
        cgltf_attribute* attribute = &cgltf_primitive->attributes[attribute_index];

        if (strcmp(attribute->name, "POSITION") == 0) position_attribute = attribute;
        if (strcmp(attribute->name, "TEXCOORD_0") == 0) texcoord_attribute = attribute;
        if (strcmp(attribute->name, "NORMAL") == 0) normal_attribute = attribute;
    }

    if (!position_attribute || !texcoord_attribute || !normal_attribute) {
        throw_error("GLTF Error", "Failed to find all needed attributes!", -1);
    }

    u32 vertex_count = (u32)position_attribute->data->count;
    submesh->vertex_count = vertex_count;
    u64 vertices_size = vertex_count * sizeof(vertex_t);
    vertex_t* vertices = (vertex_t*)malloc(vertices_size);
    memset(vertices, 0, vertices_size);

    submesh->index_count = (u32)cgltf_primitive->indices->count;
    u32 index_size = submesh->index_count * sizeof(u32);
    u32* indices = (u32*)malloc(index_size);
    memset(indices, 0, index_size);

    for (u32 vi = 0; vi < vertex_count; vi++) {
        cgltf_accessor_read_float(position_attribute->data, vi, vertices[vi].position.Elements, 4);
        cgltf_accessor_read_float(texcoord_attribute->data, vi, vertices[vi].uv.Elements, 4);
        cgltf_accessor_read_float(normal_attribute->data, vi, vertices[vi].normals.Elements, 4);
    }

    for (i32 i = 0; i < submesh->index_count; i++) {
        indices[i] = cgltf_accessor_read_index(cgltf_primitive->indices, i);
    }

    buffer_init(&submesh->vertex_buffer, &gpu->data_heap, vertices_size, sizeof(vertex_t));
    buffer_upload(&submesh->vertex_buffer, vertices, vertices_size);

    buffer_init(&submesh->index_buffer, &gpu->data_heap, index_size, sizeof(u32));
    buffer_upload(&submesh->index_buffer, indices, index_size);

    for (i32 i = 0; i < DEFAULT_GPU_FB_COUNT; i++) {
        buffer_init(&submesh->user_buffer[i], &gpu->uniform_heap, 512, 0);
    }

    // @note(ame): textures
    if (cgltf_primitive->material) {
        submesh->mat_index = m->material_count;
        material_t *mat = &m->materials[submesh->mat_index];

        char albedo_path[512];
        
        if (cgltf_primitive->material->pbr_metallic_roughness.base_color_texture.texture) {
            sprintf(albedo_path, "%s%s", m->directory, cgltf_primitive->material->pbr_metallic_roughness.base_color_texture.texture->image->uri);
            mat->has_albedo = true;
        }

        bitmap_t albedo;
        buffer_t albedo_staging;

        bitmap_load(&albedo, albedo_path);
        texture_init(&mat->albedo, albedo.width, albedo.height, DkImageFormat_RGBA8_Unorm, &gpu->image_heap, gpu->device, 0);
        
        buffer_init(&albedo_staging, &gpu->data_heap, mat->albedo.size, 0);
        buffer_upload(&albedo_staging, albedo.pixels, mat->albedo.size);

        mat->set = descriptor_set_init(&gpu->descriptor_heap, 1);

        cmd_list_single_use_t cmd = cmd_list_begin_single_use(gpu->device, &gpu->cmd_heap);
        cmd_list_copy_buffer_to_texture(cmd.buf, &albedo_staging, &mat->albedo);
        cmd_list_dset_write_texture(cmd.buf, &mat->set, &mat->albedo, 0);
        cmd_list_end_single_use(&cmd, gpu->queue);
        gpu_wait(gpu);

        buffer_free(&albedo_staging);
        bitmap_free(&albedo);

        m->material_count++;
    }

    m->vertex_count += submesh->vertex_count;
    m->index_count += submesh->index_count;

    free(vertices);
    free(indices);
}

void cgltf_process_node(cgltf_node *node, u32 *pi, model_t *m, gpu_t *gpu)
{
    if (node->mesh) {
        HMM_Mat4 pri_transform = HMM_M4D(1.0f);

        if (node->has_translation) {
            HMM_Vec3 translation = HMM_V3(node->translation[0], node->translation[1], node->translation[2]);
            pri_transform = HMM_MulM4(pri_transform, HMM_Translate(translation));
        }
        if (node->has_rotation) {
            HMM_Quat rotation = HMM_Q(node->rotation[3], node->rotation[2], node->rotation[1], node->rotation[0]);
            pri_transform = HMM_MulM4(pri_transform, HMM_QToM4(rotation));
        }
        if (node->has_scale) {
            HMM_Vec3 scale = HMM_V3(node->scale[0], node->scale[1], node->scale[2]);
            pri_transform = HMM_MulM4(pri_transform, HMM_Scale(scale));
        }

        for (i32 p = 0; p < node->mesh->primitives_count; p++) {
            cgltf_process_primitive(&node->mesh->primitives[p], pi, m, pri_transform, gpu);
            m->submesh_count++;
        }
    }

    for (i32 c = 0; c < node->children_count; c++)
        cgltf_process_node(node->children[c], pi, m, gpu);
}

void model_load(model_t *model, gpu_t *gpu, const char *path)
{
    memset(model, 0, sizeof(model_t));

    model->gpu = gpu;
    model->submeshes = malloc(sizeof(submesh_t) * MAX_SUBMESHES);
    model->materials = malloc(sizeof(material_t) * MAX_MATERIALS);
    model->submesh_count = 0;
    model->material_count = 0;
    
    memset(model->submeshes, 0, sizeof(submesh_t) * MAX_SUBMESHES);
    memset(model->materials, 0, sizeof(material_t) * MAX_MATERIALS);

    cgltf_options options;
    memset(&options, 0, sizeof(options));
    cgltf_data* data = NULL;

    cgltf_call(cgltf_parse_file(&options, path, &data));
    cgltf_call(cgltf_load_buffers(&options, data, path));
    cgltf_scene* scene = data->scene;

    char* ptr = strrchr(path, '/');
    strncpy(model->directory, path, strlen(path) - (strlen(ptr) - 1));

    u32 primitive_index = 0;
    for (u32 i = 0; i < scene->nodes_count; i++) {
        cgltf_process_node(scene->nodes[i], &primitive_index, model, gpu);
    }

    cgltf_free(data);
}

void model_free(model_t *model)
{
    for (i32 i = 0; i < model->submesh_count; i++) {
        for (i32 j = 0; j < DEFAULT_GPU_FB_COUNT; j++) {
            buffer_free(&model->submeshes[i].user_buffer[j]);
        }
        buffer_free(&model->submeshes[i].vertex_buffer);
        buffer_free(&model->submeshes[i].index_buffer);
    }
    for (i32 i = 0; i < model->material_count; i++) {
        if (model->materials[i].has_albedo) {
            descriptor_set_free(&model->materials[i].set);
            texture_free(&model->materials[i].albedo);
        }
    }

    free(model->submeshes);
}
