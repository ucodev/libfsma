/**
 * @file fsma.h
 * @brief Fast Scalable Memory Allocator Library (libfsma)
 *        Main Interface Header
 *
 * Date: 16-06-2013
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

#ifndef LIBFSMA_FSMA_H_
#define LIBFSMA_FSMA_H_

/* 
 * Refer to malloc(3), realloc(3), calloc(3),
 * free(3) and posix_memalign(3) man pages.
 *
 */

/* Macros */
#define fsma_malloc(size)			fsma_map_alloc(size)
#define fsma_calloc(nmemb, size)		fsma_map_calloc(nmemb, size)
#define fsma_realloc(ptr, size)			fsma_map_realloc(ptr, size)
#define fsma_memalign(memptr, alignment, size)	fsma_map_memalign(memptr, alignment, size)
#define fsma_free(ptr)				fsma_map_free(ptr)

/* External */
extern void *(*fsma_map_alloc) (size_t);

/* Prototypes */
void *fsma_map_calloc(size_t nmemb, size_t size);
void *fsma_map_realloc(void *ptr, size_t size);
int fsma_map_memalign(void **memptr, size_t alignment, size_t size);
void fsma_map_free(void *ptr);

#endif

