/**
 * @file main.cpp
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2023-07-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <library/allocator/allocator.h>
#include <serializer/serializer_bin.h>
#include <malloc.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>


std::vector<uintptr_t> allocated;

void* allocate(size_t size)
{
  void* block = malloc(size);
  allocated.push_back(uintptr_t(block));
  return block;
}

void* container_allocate(size_t count, size_t elem_size)
{
  void* block = calloc(count, elem_size);
  allocated.push_back(uintptr_t(block));
  return block;
}

void free_block(void* block)
{
  allocated.erase(
    std::remove_if(
      allocated.begin(), 
      allocated.end(), 
      [=](uintptr_t elem) { return (uintptr_t)block == elem; }), 
    allocated.end());
  free(block);
}

int main(int argc, char *argv[])
{
  assert(argc >= 2 && "provide path to bin file!");
  const char* bin_file = argv[1];

  allocator_t allocator;
  allocator.mem_alloc = allocate;
  allocator.mem_cont_alloc = container_allocate;
  allocator.mem_free = free_block;
  allocator.mem_alloc_alligned = NULL;
  allocator.mem_realloc = NULL;

  std::cout << "attempting to deserialize media\test01.bin: " << std::endl;
  auto* bin_data = ::deserialize_bin(bin_file, &allocator);
  std::cout << "printing all texture path" << std::endl;
  for (uint32_t i = 0; i < bin_data->texture_repo.used; ++i)
    std::cout << '\t' << bin_data->texture_repo.data[i].path.data << std::endl;
  ::free_bin(bin_data, &allocator);

  assert(allocated.size() == 0);

  return 0;
}