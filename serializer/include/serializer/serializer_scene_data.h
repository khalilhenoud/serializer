/**
 * @file serializer_scene_data.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2023-07-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef SERIALIZER_MESH_DATA_H
#define SERIALIZER_MESH_DATA_H

#include <stdint.h>
#include <library/string/fixed_string.h>
#include <math/c/matrix4f.h>
#include <math/c/vector3f.h>
#include <math/c/face.h>


typedef
struct {
  point3f min_max[2];
} serializer_aabb_t;

typedef
struct {
  serializer_aabb_t bounds;
  uint32_t left_first, tri_count;
} serializer_bvh_node_t;

typedef
struct {
  face_t* faces;
  vector3f* normals;
  serializer_aabb_t* bounds;
  uint32_t count;
  serializer_bvh_node_t* nodes;
  uint32_t nodes_used;
} serializer_bvh_t;


typedef
struct {
  fixed_str_t path;
} serializer_texture_data_t;

typedef
struct {
  uint32_t index;               // index into texture repo.
  fixed_str_t name;
  fixed_str_t type;
  float u, v;
  float u_scale, v_scale;
  float angle;
} serializer_texture_properties_t;

// limit is 8 textures per material.
typedef
struct {
  uint32_t used;
  serializer_texture_properties_t data[8];
} serializer_texture_properties_array_t;

typedef 
struct {
  float data[4];
} serializer_color_data_t;

typedef 
struct {
  fixed_str_t name;
  serializer_color_data_t ambient;
  serializer_color_data_t diffuse;
  serializer_color_data_t specular;
  float shininess;
  float opacity;
  serializer_texture_properties_array_t textures;
} serializer_material_data_t;

typedef
struct {
  uint32_t used;
  uint32_t indices[1024];
} fixed_idx_to_target_t; 

typedef 
struct {
  fixed_str_t name;
  matrix4f local_transform;
  float *vertices;   // 3
  float *normals;    // 3
  float *uvs;        // 3 
  uint32_t vertices_count;
  uint32_t *indices;
  uint32_t faces_count;
  fixed_idx_to_target_t materials;
} serializer_mesh_data_t;

typedef
enum {
  SERIALIZER_LIGHT_TYPE_POINT,
  SERIALIZER_LIGHT_TYPE_SPOT,
  SERIALIZER_LIGHT_TYPE_DIRECTIONAL,
  SERIALIZER_LUGHT_TYPE_COUNT
} serializer_light_type_t;

typedef
struct {
  fixed_str_t name;
  vector3f position;
  vector3f direction;
  vector3f up;
  float inner_cone;
  float outer_cone;
  float attenuation_constant;
  float attenuation_linear;
  float attenuation_quadratic;
  serializer_color_data_t diffuse;
  serializer_color_data_t specular;
  serializer_color_data_t ambient;
  serializer_light_type_t type;
} serializer_light_data_t;

typedef 
struct {
  fixed_str_t name;
  matrix4f transform;
  fixed_idx_to_target_t meshes;
  fixed_idx_to_target_t models;
} serializer_model_data_t;

typedef
struct {
  fixed_str_t image_file;
  fixed_str_t data_file;
} serializer_font_t;

typedef
struct {
  vector3f position;
  vector3f lookat_direction;
  vector3f up_vector;
} serializer_camera_t;

typedef
struct {
  uint32_t used;
  serializer_font_t *data;
} serializer_font_repo_t;

typedef
struct {
  uint32_t used;
  serializer_camera_t *data;
} serializer_camera_repo_t;

typedef 
struct {
  uint32_t used;
  serializer_light_data_t *data;
} serializer_light_repo_t;

typedef
struct {
  uint32_t used;
  serializer_texture_data_t *data;
} serializer_texture_repo_t;

typedef 
struct {
  uint32_t used;
  serializer_material_data_t *data;
} serializer_material_repo_t;

typedef
struct {
  uint32_t used;
  serializer_mesh_data_t *data;
} serializer_mesh_repo_t;

typedef
struct {
  uint32_t used;
  serializer_model_data_t *data;
} serializer_model_repo_t;

typedef
struct {
  uint32_t used;
  serializer_bvh_t *data;
} serializer_bvh_repo_t;

typedef
struct serializer_scene_data_t {
  serializer_model_repo_t model_repo;   // root = model_repo.data[0]
  serializer_light_repo_t light_repo;
  serializer_mesh_repo_t mesh_repo;
  serializer_material_repo_t material_repo;
  serializer_texture_repo_t texture_repo;
  serializer_camera_repo_t camera_repo;
  serializer_bvh_repo_t bvh_repo;
  // TODO(khalil): font repos are not being serialized, do so.
  serializer_font_repo_t font_repo;
} serializer_scene_data_t;

#endif