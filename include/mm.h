/**
 * @file mm.h
 * @brief Fast Scalable Memory Allocator Library (libfsma)
 *        Memory Management Interface Header
 *
 * Date: 21-06-2013
 * 
 * Copyright 2013 Pedro A. Hortas (pah@ucodev.org)
 *
 * This file is part of libfsma.
 *
 * libfsma is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libfsma is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libfsma.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LIBFSMA_MM_H_
#define LIBFSMA_MM_H_

#include <sys/mman.h>

/* Definitions */
#ifndef MAP_ANONYMOUS
 #ifdef MAP_ANON
  #define MAP_ANONYMOUS MAP_ANON
 #else
  #error "Anonymous mapping isn't supported"
 #endif
#endif

#define MM_POOL_DEFAULT_SIZE	65536
#define MM_POOL_INIT_ELEMS	64

/* Macros */
#define _map(size) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)

/* Prototypes */
static void *_fsma_map_alloc_many(size_t size);
#ifdef _REENTRANT
static void _mm_map_destroy(void *arg);
static void *_fsma_map_alloc_once(size_t size);
#endif

#endif

