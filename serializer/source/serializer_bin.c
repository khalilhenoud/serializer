/**
 * @file serializer_bin.c
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2023-07-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <assert.h>
#include <serializer/utils.h>
#include <serializer/serializer_bin.h>


////////////////////////////////////////////////////////////////////////////////
static
void
serialize_metadata(
  file_handle_t file, 
  serializer_scene_data_t* scene)
{
  assert(scene != NULL && "'scene' params are NULL!");
  
  write_buffer(
    file,
    &scene->metadata.player_start,
    sizeof(scene->metadata.player_start), 1);
  write_buffer(
    file,
    &scene->metadata.player_angle,
    sizeof(scene->metadata.player_angle), 1);
}

static
void
serialize_lights(
  file_handle_t file, 
  serializer_scene_data_t* scene)
{
  assert(scene != NULL && "'scene' params are NULL!");

  write_buffer(file, &scene->light_repo.used, sizeof(uint32_t), 1);
  {
    serializer_light_data_t *data = scene->light_repo.data;
    for (uint32_t i = 0; i < scene->light_repo.used; ++i, ++data) {
      write_buffer(
        file,
        data->name.data,
        sizeof(data->name.data[0]), sizeof(data->name.data));
      write_buffer(
        file, data->position.data, sizeof(data->position.data[0]), 3);
      write_buffer(
        file, data->direction.data, sizeof(data->direction.data[0]), 3);
      write_buffer(
        file, data->up.data, sizeof(data->up.data[0]), 3);
      write_buffer(file, &data->inner_cone, sizeof(data->inner_cone), 1);
      write_buffer(file, &data->outer_cone, sizeof(data->outer_cone), 1);
      write_buffer(
        file, 
        &data->attenuation_constant, 
        sizeof(data->attenuation_constant), 
        1);
      write_buffer(
        file, 
        &data->attenuation_linear, 
        sizeof(data->attenuation_linear), 
        1);
      write_buffer(
        file, 
        &data->attenuation_quadratic, 
        sizeof(data->attenuation_quadratic), 
        1);
      write_buffer(
        file, data->diffuse.data, sizeof(data->diffuse.data[0]), 4);
      write_buffer(
        file, data->specular.data, sizeof(data->specular.data[0]), 4);
      write_buffer(
        file, data->ambient.data, sizeof(data->ambient.data[0]), 4);
      write_buffer(file, &data->type, sizeof(data->type), 1);
    }
  }
}

static
void
serialize_textures(
  file_handle_t file, 
  serializer_scene_data_t* scene)
{
  assert(scene != NULL && "'scene' params are NULL!");

  write_buffer(file, &scene->texture_repo.used, sizeof(uint32_t), 1);
  {
    serializer_texture_data_t *data = scene->texture_repo.data;
    for (uint32_t i = 0; i < scene->texture_repo.used; ++i, ++data) {
      write_buffer(
        file,
        data->path.data,
        sizeof(data->path.data[0]), sizeof(data->path.data));
    }
  }
}

static
void
serialize_materials(
  file_handle_t file, 
  serializer_scene_data_t* scene)
{
  assert(scene != NULL && "'scene' params are NULL!");

  // serialize material data.
  write_buffer(file, &scene->material_repo.used, sizeof(uint32_t), 1);
  {
    serializer_material_data_t* data = scene->material_repo.data;
    for (uint32_t i = 0; i < scene->material_repo.used; ++i, ++data) {
      write_buffer(
        file, 
        data->name.data, 
        sizeof(data->name.data[0]), sizeof(data->name.data));

      write_buffer(
        file, data->ambient.data, sizeof(data->ambient.data[0]), 4);
      write_buffer(
        file, data->diffuse.data, sizeof(data->diffuse.data[0]), 4);
      write_buffer(
        file, data->specular.data, sizeof(data->specular.data[0]), 4);
      write_buffer(file, &data->shininess, sizeof(float), 1);
      write_buffer(file, &data->opacity, sizeof(float), 1);

      // serialize the texture properties array.
      write_buffer(file, &data->textures.used, sizeof(uint32_t), 1);
      {
        serializer_texture_properties_t* texture_data = data->textures.data;
        for (uint32_t j = 0; j < data->textures.used; ++j, ++texture_data) {
          // index into the texture repo.
          write_buffer(file, &texture_data->index, sizeof(uint32_t), 1);
          write_buffer(
            file, 
            texture_data->name.data,
            sizeof(texture_data->name.data[0]), 
            sizeof(texture_data->name.data));
          write_buffer(
            file, 
            texture_data->type.data,
            sizeof(texture_data->type.data[0]), 
            sizeof(texture_data->type.data));
          write_buffer(file, &texture_data->u, sizeof(float), 1);
          write_buffer(file, &texture_data->v, sizeof(float), 1);
          write_buffer(file, &texture_data->u_scale, sizeof(float), 1);
          write_buffer(file, &texture_data->v_scale, sizeof(float), 1);
          write_buffer(file, &texture_data->angle, sizeof(float), 1);
        }
      }
    }
  }
}

static
void
serialize_meshes(
  file_handle_t file, 
  serializer_scene_data_t* scene)
{
  assert(scene != NULL && "'scene' params are NULL!");

  write_buffer(file, &scene->mesh_repo.used, sizeof(uint32_t), 1);
  {
    serializer_mesh_data_t* data = scene->mesh_repo.data;
    for (uint32_t i = 0; i < scene->mesh_repo.used; ++i, ++data) {
      write_buffer(
        file, 
        data->name.data, 
        sizeof(data->name.data[0]), sizeof(data->name.data));
      // serialize matrix transform.
      write_buffer(
        file, 
        data->local_transform.data, 
        sizeof(data->local_transform.data[0]), 16);
      // serialize vertices data.
      write_buffer(file, &data->vertices_count, sizeof(uint32_t), 1);
      write_buffer(
        file, data->vertices, sizeof(float), data->vertices_count * 3);
      write_buffer(
        file, data->normals, sizeof(float), data->vertices_count * 3);
      write_buffer(
        file, data->uvs, sizeof(float), data->vertices_count * 3);
      // serialize faces data.
      write_buffer(file, &data->faces_count, sizeof(uint32_t), 1);
      write_buffer(
        file, data->indices, sizeof(uint32_t), data->faces_count * 3);

      // serializing material indices.
      write_buffer(file, &data->materials.used, sizeof(uint32_t), 1);
      write_buffer(
        file, 
        data->materials.indices, 
        sizeof(uint32_t), (size_t)data->materials.used);
    }
  }
}

static
void
serialize_models(
  file_handle_t file, 
  serializer_scene_data_t* scene)
{
  assert(scene != NULL && "'scene' params are NULL!");

  write_buffer(file, &scene->model_repo.used, sizeof(uint32_t), 1);
  {
    serializer_model_data_t* data = scene->model_repo.data;
    for (uint32_t i = 0; i < scene->model_repo.used; ++i, ++data) {
      write_buffer(
        file, 
        data->name.data, 
        sizeof(data->name.data[0]), sizeof(data->name.data));
      write_buffer(
        file, 
        data->transform.data, 
        sizeof(data->transform.data[0]), 16);

      // write meshes indices, model indices, light indices.
      write_buffer(file, &data->meshes.used, sizeof(uint32_t), 1);
      write_buffer(
        file, data->meshes.indices, sizeof(uint32_t), data->meshes.used);
      write_buffer(file, &data->models.used, sizeof(uint32_t), 1);
      write_buffer(
        file, data->models.indices, sizeof(uint32_t), data->models.used);
    }
  }
}

static
void
serialize_cameras(
  file_handle_t file, 
  serializer_scene_data_t* scene)
{
  assert(scene != NULL && "'scene' params are NULL!");

  write_buffer(file, &scene->camera_repo.used, sizeof(uint32_t), 1);
  if (scene->camera_repo.used) {
    serializer_camera_t* data = scene->camera_repo.data;
    for (uint32_t i = 0; i < scene->camera_repo.used; ++i, ++data) {
      write_buffer(
        file, 
        data->position.data, 
        sizeof(data->position.data[0]), 3);
      write_buffer(
        file, 
        data->lookat_direction.data, 
        sizeof(data->lookat_direction.data[0]), 
        3);
      write_buffer(
        file, 
        data->up_vector.data, 
        sizeof(data->up_vector.data[0]), 3);
    }
  }
}

static
void
serialize_bvhs(
  file_handle_t file,
  serializer_scene_data_t* scene)
{
  assert(scene != NULL && "'scene' params are NULL!");

  write_buffer(file, &scene->bvh_repo.used, sizeof(uint32_t), 1);
  {
    serializer_bvh_t* data = scene->bvh_repo.data;
    for (uint32_t i = 0; i < scene->bvh_repo.used; ++i, ++data) {
      // serialize the face data.
      write_buffer(file, &data->count, sizeof(uint32_t), 1);
      write_buffer(
        file,
        data->faces, 
        sizeof(data->faces[0]), data->count);
      write_buffer(
        file, 
        data->normals, 
        sizeof(data->normals[0]), data->count);
      write_buffer(
        file, 
        data->bounds, 
        sizeof(data->bounds[0]), data->count);
      // serialize the node data.
      write_buffer(file, &data->nodes_used, sizeof(uint32_t), 1);
      write_buffer(
        file, 
        data->nodes, 
        sizeof(data->nodes[0]), data->nodes_used);
    }
  }
}

void
serialize_bin(
  const char* path,
  serializer_scene_data_t* scene)
{  
  file_handle_t file;
  assert(scene != NULL && "'scene' params are NULL!");

  file = open_file(path, FILE_OPEN_MODE_WRITE | FILE_OPEN_MODE_BINARY);
  assert((void *)file != NULL);

  serialize_metadata(file, scene);
  serialize_lights(file, scene);
  serialize_textures(file, scene);
  serialize_materials(file, scene);
  serialize_meshes(file, scene);
  serialize_models(file, scene);
  serialize_cameras(file, scene);
  serialize_bvhs(file, scene);

  close_file(file);
}

////////////////////////////////////////////////////////////////////////////////
void
deserialize_metadata(
  file_handle_t file, 
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(allocator != NULL && "allocator is NULL!");
  assert(scene != NULL && "scene is NULL!");
  
  read_buffer(
    file,
    &scene->metadata.player_start,
    sizeof(scene->metadata.player_start), 1);
  read_buffer(
    file,
    &scene->metadata.player_angle,
    sizeof(scene->metadata.player_angle), 1);
}

static
void
deserialize_lights(
  file_handle_t file, 
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(allocator != NULL && "allocator is NULL!");
  assert(scene != NULL && "scene is NULL!");

  read_buffer(file, &scene->light_repo.used, sizeof(uint32_t), 1);
  if (scene->light_repo.used) {
    scene->light_repo.data = 
    (serializer_light_data_t*)allocator->mem_cont_alloc(
      scene->light_repo.used, 
      sizeof(serializer_light_data_t));
    {
      serializer_light_data_t* data = scene->light_repo.data;
      for (uint32_t i = 0; i < scene->light_repo.used; ++i, ++data) {
        read_buffer(
          file,
          data->name.data,
          sizeof(data->name.data[0]), sizeof(data->name.data));
        read_buffer(
          file, data->position.data, sizeof(data->position.data[0]), 3);
        read_buffer(
          file, data->direction.data, sizeof(data->direction.data[0]), 3);
        read_buffer(
          file, data->up.data, sizeof(data->up.data[0]), 3);
        read_buffer(file, &data->inner_cone, sizeof(data->inner_cone), 1);
        read_buffer(file, &data->outer_cone, sizeof(data->outer_cone), 1);
        read_buffer(
          file, 
          &data->attenuation_constant, 
          sizeof(data->attenuation_constant), 
          1);
        read_buffer(
          file, 
          &data->attenuation_linear, 
          sizeof(data->attenuation_linear), 
          1);
        read_buffer(
          file, 
          &data->attenuation_quadratic, 
          sizeof(data->attenuation_quadratic), 
          1);
        read_buffer(
          file, data->diffuse.data, sizeof(data->diffuse.data[0]), 4);
        read_buffer(
          file, data->specular.data, sizeof(data->specular.data[0]), 4);
        read_buffer(
          file, data->ambient.data, sizeof(data->ambient.data[0]), 4);
        read_buffer(file, &data->type, sizeof(data->type), 1);
      }
    }
  }
}

static
void
deserialize_textures(
  file_handle_t file, 
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(allocator != NULL && "allocator is NULL!");
  assert(scene != NULL && "scene is NULL!");

  read_buffer(file, &scene->texture_repo.used, sizeof(uint32_t), 1);
  if (scene->texture_repo.used) {
    scene->texture_repo.data =
      (serializer_texture_data_t*)allocator->mem_cont_alloc(
        scene->texture_repo.used,
        sizeof(serializer_texture_data_t));
    {
      serializer_texture_data_t* data = scene->texture_repo.data;
      for (uint32_t i = 0; i < scene->texture_repo.used; ++i, ++data) {
        read_buffer(
          file,
          data->path.data,
          sizeof(data->path.data[0]), sizeof(data->path.data));
      }
    }
  }
}

static
void
deserialize_materials(
  file_handle_t file, 
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(allocator != NULL && "allocator is NULL!");
  assert(scene != NULL && "scene is NULL!");

  read_buffer(file, &scene->material_repo.used, sizeof(uint32_t), 1);
  if (scene->material_repo.used) {
    scene->material_repo.data =
      (serializer_material_data_t*)allocator->mem_cont_alloc(
        scene->material_repo.used,
        sizeof(serializer_material_data_t));
    {
      serializer_material_data_t* data = scene->material_repo.data;
      for (uint32_t i = 0; i < scene->material_repo.used; ++i, ++data) {
        read_buffer(
          file,
          data->name.data,
          sizeof(data->name.data[0]), sizeof(data->name.data));

        read_buffer(
          file, data->ambient.data, sizeof(data->ambient.data[0]), 4);
        read_buffer(
          file, data->diffuse.data, sizeof(data->diffuse.data[0]), 4);
        read_buffer(
          file, data->specular.data, sizeof(data->specular.data[0]), 4);
        read_buffer(file, &data->shininess, sizeof(float), 1);
        read_buffer(file, &data->opacity, sizeof(float), 1);

        // deserialize the texture properties array.
        read_buffer(file, &data->textures.used, sizeof(uint32_t), 1);
        {
          serializer_texture_properties_t* texture_data = data->textures.data;
          for (uint32_t j = 0; j < data->textures.used; ++j, ++texture_data) {
            // index into the texture repo.
            read_buffer(file, &texture_data->index, sizeof(uint32_t), 1);
            read_buffer(
              file,
              texture_data->name.data,
              sizeof(texture_data->name.data[0]),
              sizeof(texture_data->name.data));
            read_buffer(
              file,
              texture_data->type.data,
              sizeof(texture_data->type.data[0]),
              sizeof(texture_data->type.data));
            read_buffer(file, &texture_data->u, sizeof(float), 1);
            read_buffer(file, &texture_data->v, sizeof(float), 1);
            read_buffer(file, &texture_data->u_scale, sizeof(float), 1);
            read_buffer(file, &texture_data->v_scale, sizeof(float), 1);
            read_buffer(file, &texture_data->angle, sizeof(float), 1);
          }
        }
      }
    }
  }
}

static
void
deserialize_meshes(
  file_handle_t file, 
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(allocator != NULL && "allocator is NULL!");
  assert(scene != NULL && "scene is NULL!");

  read_buffer(file, &scene->mesh_repo.used, sizeof(uint32_t), 1);
  scene->mesh_repo.data = 
    (serializer_mesh_data_t*)allocator->mem_cont_alloc(
      scene->mesh_repo.used, 
      sizeof(serializer_mesh_data_t));
  {
    serializer_mesh_data_t* data = scene->mesh_repo.data;
    memset(data, 0, sizeof(serializer_mesh_data_t));
    for (uint32_t i = 0; i < scene->mesh_repo.used; ++i, ++data) {
      read_buffer(
        file, 
        data->name.data, 
        sizeof(data->name.data[0]), sizeof(data->name.data));
      // deserialize matrix transform.
      read_buffer(
        file, 
        data->local_transform.data, 
        sizeof(data->local_transform.data[0]), 16);
      // deserialize vertices data.
      read_buffer(file, &data->vertices_count, sizeof(uint32_t), 1);
      data->vertices = 
        (float*)allocator->mem_cont_alloc(
          data->vertices_count * 3, sizeof(float));
      data->normals = 
        (float*)allocator->mem_cont_alloc(
          data->vertices_count * 3, sizeof(float));
      data->uvs = 
        (float*)allocator->mem_cont_alloc(
          data->vertices_count * 3, sizeof(float));
      read_buffer(
        file, data->vertices, sizeof(float), data->vertices_count * 3);
      read_buffer(
        file, data->normals, sizeof(float), data->vertices_count * 3);
      read_buffer(
        file, data->uvs, sizeof(float), data->vertices_count * 3);
      // deserialize faces data.
      read_buffer(file, &data->faces_count, sizeof(uint32_t), 1);
      data->indices = 
        (uint32_t*)allocator->mem_cont_alloc(
          data->faces_count * 3, sizeof(uint32_t));
      read_buffer(file, data->indices, sizeof(uint32_t), data->faces_count * 3);
      // deserialize material indices.
      read_buffer(file, &data->materials.used, sizeof(uint32_t), 1);
      read_buffer(
        file, 
        data->materials.indices, 
        sizeof(uint32_t), (size_t)data->materials.used);
    }
  }
}

static
void
deserialize_models(
  file_handle_t file, 
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(allocator != NULL && "allocator is NULL!");
  assert(scene != NULL && "scene is NULL!");

  read_buffer(file, &scene->model_repo.used, sizeof(uint32_t), 1);
  scene->model_repo.data = 
    (serializer_model_data_t*)allocator->mem_cont_alloc(
      scene->model_repo.used, 
      sizeof(serializer_model_data_t));
  {
    serializer_model_data_t* data = scene->model_repo.data;
    for (uint32_t i = 0; i < scene->model_repo.used; ++i, ++data) {
      read_buffer(
        file, 
        data->name.data, 
        sizeof(data->name.data[0]), sizeof(data->name.data));
      read_buffer(
        file, 
        data->transform.data, 
        sizeof(data->transform.data[0]), 16);

      // write meshes indices, and models indices;
      read_buffer(file, &data->meshes.used, sizeof(uint32_t), 1);
      read_buffer(
        file, data->meshes.indices, sizeof(uint32_t), data->meshes.used);
      read_buffer(file, &data->models.used, sizeof(uint32_t), 1);
      read_buffer(
        file, data->models.indices, sizeof(uint32_t), data->models.used);
    }
  }
}

static
void
deserialize_cameras(
  file_handle_t file, 
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(allocator != NULL && "allocator is NULL!");
  assert(scene != NULL && "scene is NULL!");

  read_buffer(file, &scene->camera_repo.used, sizeof(uint32_t), 1);
  if (scene->camera_repo.used) {
    scene->camera_repo.data = 
    (serializer_camera_t*)allocator->mem_cont_alloc(
      scene->camera_repo.used, 
      sizeof(serializer_camera_t));
    {
      serializer_camera_t* data = scene->camera_repo.data;
      for (uint32_t i = 0; i < scene->camera_repo.used; ++i, ++data) {
        read_buffer(
          file, data->position.data, sizeof(data->position.data[0]), 3);
        read_buffer(
          file, 
          data->lookat_direction.data, 
          sizeof(data->lookat_direction.data[0]), 
          3);
        read_buffer(
          file, 
          data->up_vector.data, 
          sizeof(data->up_vector.data[0]), 3);
      }
    }
  }
}


static
void
deserialize_bvhs(
file_handle_t file, 
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(allocator != NULL && "allocator is NULL!");
  assert(scene != NULL && "scene is NULL!");

  read_buffer(file, &scene->bvh_repo.used, sizeof(uint32_t), 1);
  if (scene->bvh_repo.used) {
    scene->bvh_repo.data = 
    (serializer_bvh_t*)allocator->mem_cont_alloc(
      scene->bvh_repo.used, 
      sizeof(serializer_bvh_t));

    {
      serializer_bvh_t* data = scene->bvh_repo.data;
      for (uint32_t i = 0; i < scene->bvh_repo.used; ++i, ++data) {
        // deserialize the face data.
        read_buffer(file, &data->count, sizeof(uint32_t), 1);
        data->faces = 
          (face_t*)allocator->mem_cont_alloc(data->count, sizeof(face_t));
        data->normals = 
          (vector3f*)allocator->mem_cont_alloc(data->count, sizeof(vector3f));
        data->bounds = 
          (serializer_aabb_t*)allocator->mem_cont_alloc(
            data->count, sizeof(serializer_aabb_t));
        read_buffer(
          file,
          data->faces, 
          sizeof(data->faces[0]), data->count);
        read_buffer(
          file, 
          data->normals, 
          sizeof(data->normals[0]), data->count);
        read_buffer(
          file, 
          data->bounds, 
          sizeof(data->bounds[0]), data->count);
        // deserialize the node data.
        read_buffer(file, &data->nodes_used, sizeof(uint32_t), 1);
        data->nodes = 
          (serializer_bvh_node_t*)allocator->mem_cont_alloc(
            data->nodes_used, sizeof(serializer_bvh_node_t));
        read_buffer(
          file, 
          data->nodes, 
          sizeof(data->nodes[0]), data->nodes_used);
      }
    }
  }
}

serializer_scene_data_t*
deserialize_bin(
  const char* path,
  const allocator_t* allocator)
{
  file_handle_t file;
  assert(allocator != NULL && "allocator is NULL!");

  file = open_file(path, FILE_OPEN_MODE_READ | FILE_OPEN_MODE_BINARY);
  assert((void *)file != NULL);

  {
    serializer_scene_data_t* scene = NULL;
    scene = (serializer_scene_data_t*)allocator->mem_alloc(
      sizeof(serializer_scene_data_t));
    assert(scene != NULL && "failed to allocate scene");
    memset(scene, 0, sizeof(serializer_scene_data_t));

    deserialize_metadata(file, scene, allocator);
    deserialize_lights(file, scene, allocator);
    deserialize_textures(file, scene, allocator);
    deserialize_materials(file, scene, allocator);
    deserialize_meshes(file, scene, allocator);
    deserialize_models(file, scene, allocator);
    deserialize_cameras(file, scene, allocator);
    deserialize_bvhs(file, scene, allocator);

    close_file(file);
    return scene;
  }
}

////////////////////////////////////////////////////////////////////////////////
static
void
free_serializer_mesh(
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(scene != NULL && "scene is NULL!");
  assert(allocator != NULL && "allocator is NULL!");

  if (scene->mesh_repo.used) {
    serializer_mesh_data_t* data = scene->mesh_repo.data;
    for (uint32_t i = 0; i < scene->mesh_repo.used; ++i, ++data) {
      if (data->vertices)
        allocator->mem_free(data->vertices);
      if (data->normals)
        allocator->mem_free(data->normals);
      if (data->uvs)
        allocator->mem_free(data->uvs);
      if (data->indices)
        allocator->mem_free(data->indices);
    }
    allocator->mem_free(scene->mesh_repo.data);
  }
}

static
void
free_serializer_bvh(
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(scene != NULL && "scene is NULL!");
  assert(allocator != NULL && "allocator is NULL!");

  if (scene->bvh_repo.used) {
    serializer_bvh_t* data = scene->bvh_repo.data;
    for (uint32_t i = 0; i < scene->bvh_repo.used; ++i, ++data) {
      if (data->bounds)
        allocator->mem_free(data->bounds);
      if (data->faces)
        allocator->mem_free(data->faces);
      if (data->nodes)
        allocator->mem_free(data->nodes);
      if (data->normals)
        allocator->mem_free(data->normals);
    }
    allocator->mem_free(scene->bvh_repo.data);
  }
}

void
free_bin(
  serializer_scene_data_t* scene,
  const allocator_t* allocator)
{
  assert(scene != NULL && "scene is NULL!");
  assert(allocator != NULL && "allocator is NULL!");

  if (scene->font_repo.used)
    allocator->mem_free(scene->font_repo.data);
  if (scene->camera_repo.used)
    allocator->mem_free(scene->camera_repo.data);
  if (scene->texture_repo.used)
    allocator->mem_free(scene->texture_repo.data);
  if (scene->material_repo.used)
    allocator->mem_free(scene->material_repo.data);
  if (scene->light_repo.used)
    allocator->mem_free(scene->light_repo.data);
  free_serializer_mesh(scene, allocator);
  free_serializer_bvh(scene, allocator);
  allocator->mem_free(scene->model_repo.data);
  allocator->mem_free(scene);
}