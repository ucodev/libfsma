/**
 * @file mm.c
 * @brief Fast Scalable Memory Allocator Library (libfsma)
 *        Memory Management Interface
 *
 * Date: 13-03-2015
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


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#ifndef LIBFSMA_NO_THREADS
 #include <pthread.h>
#else
 #ifdef _REENTRANT
  #undef _REENTRANT
 #endif
#endif

#include "mm.h"

#ifdef _REENTRANT
static char *main_pool = NULL;
static pthread_key_t _mm_map_tlskey_mm_pool, _mm_map_tlskey_cur_pool;
static pthread_once_t _mm_map_key_once = PTHREAD_ONCE_INIT;
static pthread_mutex_t _mm_map_tmutex = PTHREAD_MUTEX_INITIALIZER;
void *(*fsma_map_alloc) (size_t) = _fsma_map_alloc_once;
#else
static char **mm_pool = NULL;
static char *cur_pool = NULL;
void *(*fsma_map_alloc) (size_t) = _fsma_map_alloc_many;
#endif

enum mm_map_ranks {
	MM_MAP_RANK_FREE,
	MM_MAP_RANK_TBF,
	MM_MAP_RANK_USED
};

#ifdef _REENTRANT
static void _mm_map_key_init(void) {
	pthread_key_create(&_mm_map_tlskey_mm_pool, _mm_map_destroy);
	pthread_key_create(&_mm_map_tlskey_cur_pool, NULL);
}
#endif

static void *_mm_map_init(void) {
#ifdef _REENTRANT
	char **mm_pool, *cur_pool;
	size_t mm_pool_next;

	pthread_mutex_lock(&_mm_map_tmutex);

	if (main_pool) {
		memcpy(&mm_pool_next, &main_pool[sizeof(size_t)], sizeof(size_t));
		mm_pool = (char **) main_pool;
		main_pool = (char *) mm_pool_next;
		pthread_mutex_unlock(&_mm_map_tmutex);
	} else {
		pthread_mutex_unlock(&_mm_map_tmutex);
#endif

		if ((mm_pool = _map((MM_POOL_INIT_ELEMS + 2) * sizeof(size_t))) == (void *) -1)
			return NULL;

		memcpy(mm_pool, (size_t [1]) { MM_POOL_INIT_ELEMS }, sizeof(size_t));
		memset(&mm_pool[2], 0, MM_POOL_INIT_ELEMS * sizeof(size_t));

		if ((mm_pool[2] = _map(MM_POOL_DEFAULT_SIZE)) == (void *) -1)
			return NULL;
#ifdef _REENTRANT
	}
#endif

	cur_pool = mm_pool[2];
	memset(cur_pool, 0, MM_POOL_DEFAULT_SIZE);

	memcpy(cur_pool, (size_t [1]) { MM_POOL_DEFAULT_SIZE - (sizeof(size_t) << 1) }, sizeof(size_t));
	memcpy(&cur_pool[sizeof(size_t)], (size_t [1]) { MM_POOL_DEFAULT_SIZE }, sizeof(size_t));

#ifdef _REENTRANT
	pthread_setspecific(_mm_map_tlskey_mm_pool, mm_pool);
	pthread_setspecific(_mm_map_tlskey_cur_pool, cur_pool);
#endif
	return cur_pool;
}

#ifdef _REENTRANT
static void _mm_map_destroy(void *arg) {
	char **mm_pool = arg;

	pthread_mutex_lock(&_mm_map_tmutex);

	memcpy(&mm_pool[1], &main_pool, sizeof(size_t));

	main_pool = (char *) mm_pool;

	pthread_mutex_unlock(&_mm_map_tmutex);
}
#endif

static void *_mm_map_new_pool(
		char **mm_pool,
		char *cur_pool,
		size_t size,
		size_t mm_pool_elems,
		size_t index_hint)
{
	size_t size_new = 0;

	for ( ; index_hint < (mm_pool_elems + 2); index_hint ++) {
		if (!mm_pool[index_hint])
			break;
	}

	if (size > MM_POOL_DEFAULT_SIZE) {
		size_new = (size + (MM_POOL_DEFAULT_SIZE - 1)) & ~(MM_POOL_DEFAULT_SIZE - 1);
	} else {
		size_new = MM_POOL_DEFAULT_SIZE;
	}

	mm_pool[index_hint] = _map(size_new);

	if (mm_pool[index_hint] == (void *) -1) {
		if ((mm_pool[index_hint] = _map(size)) == (void *) -1)
			return NULL;

		size_new = size;
	}

	cur_pool = mm_pool[index_hint];
#ifdef _REENTRANT
	pthread_setspecific(_mm_map_tlskey_cur_pool, cur_pool);
#endif
	memset(cur_pool, 0, size_new);
	memcpy(cur_pool, (size_t [1]) { size_new - (sizeof(size_t) << 1) }, sizeof(size_t));
	memcpy(&cur_pool[sizeof(size_t)], &size_new, sizeof(size_t));

	return cur_pool;
}

static size_t _mm_map_update_pool(char *pool) {
	char *cur;
	size_t length, rank, bused, pool_size, bfree, mcfs;

	memcpy(&pool_size, &pool[sizeof(size_t)], sizeof(size_t));

	for (cur = &pool[sizeof(size_t) << 1], bused = 0, mcfs = 0, bfree = 0; ; cur += length + (sizeof(size_t) << 1)) {
		memcpy(&rank, cur, sizeof(size_t));
		memcpy(&length, &cur[sizeof(size_t)], sizeof(size_t));

		if (!length) {
			if ((bfree = pool_size - bused - (sizeof(size_t) << 1)) > mcfs)
				mcfs = bfree;

			break;
		}

		if (rank == MM_MAP_RANK_USED) {
			bused += length + (sizeof(size_t) << 1);
			continue;
		} else if (rank == MM_MAP_RANK_TBF) {
			memcpy(cur, (size_t [1]) { MM_MAP_RANK_FREE }, sizeof(size_t));
		}

		if (length > mcfs)
			mcfs = length;

		if (&cur[(sizeof(size_t) << 1) + length] >= (pool + pool_size)) {
			bfree = pool_size - bused - (sizeof(size_t) << 1);
			break;
		}
	}

	memcpy(pool, &bfree, sizeof(size_t));

	return mcfs;
}

static void *_mm_map_change_pool(size_t size, char *cur_pool) {
	size_t i, mm_pool_elems;
	char **mm_pool_tmp;
#ifdef _REENTRANT
	char **mm_pool;

	mm_pool = pthread_getspecific(_mm_map_tlskey_mm_pool);
#endif

	memcpy(&mm_pool_elems, mm_pool, sizeof(size_t));

	for (i = 2; i < (mm_pool_elems + 2); i ++) {
		if (!mm_pool[i])
			return _mm_map_new_pool(mm_pool, cur_pool, size, mm_pool_elems, i);

		if (mm_pool[i] == cur_pool)
			continue;

		if (_mm_map_update_pool(mm_pool[i]) >= (size + (sizeof(size_t) << 1))) {
			cur_pool = mm_pool[i];
#ifdef _REENTRANT
			pthread_setspecific(_mm_map_tlskey_cur_pool, cur_pool);
#endif
			return cur_pool;
		}
	}

	mm_pool_elems <<= 1;

	if ((mm_pool_tmp = _map((mm_pool_elems + 2) * sizeof(size_t))) == (void *) -1)
		return NULL;

	memcpy(mm_pool_tmp, &mm_pool_elems, sizeof(size_t));
	memcpy(&mm_pool_tmp[2], &mm_pool[2], (mm_pool_elems >> 1) * sizeof(size_t));
	memset(&mm_pool_tmp[2 + ((mm_pool_elems >> 1) * sizeof(size_t))], 0, (mm_pool_elems >> 1) * sizeof(size_t));

	mm_pool = mm_pool_tmp;

#ifdef _REENTRANT
	pthread_setspecific(_mm_map_tlskey_mm_pool, mm_pool);
#endif

	return _mm_map_new_pool(mm_pool, cur_pool, size, mm_pool_elems, i);
}

static inline size_t _mm_map_defrag(
		char *cur_pool,
		size_t cur_pool_size,
		char *cur,
		size_t size,
		size_t length,
		size_t *bfree)
{
	size_t erank, elen;

	for (;;) {
		memcpy(&erank, &cur[(sizeof(size_t) << 1) + length], sizeof(size_t));
		memcpy(&elen, &cur[(sizeof(size_t) << 1) + length + sizeof(size_t)], sizeof(size_t));

		if (!elen) {
			if (&cur[(sizeof(size_t) << 1) + size] <= (cur_pool + cur_pool_size)) {
				length = size;
			} else {
				length += cur_pool_size - (&cur[(sizeof(size_t) << 1) + length] - cur_pool);
			}

			break;
		} else if (erank == MM_MAP_RANK_USED) {
			break;
		} else if (erank == MM_MAP_RANK_TBF) {
			*bfree += elen + (sizeof(size_t) << 1);
			memcpy(&cur[(sizeof(size_t) << 1) + length], (size_t [1]) { MM_MAP_RANK_FREE }, sizeof(size_t));
		}

		length += (elen + (sizeof(size_t) << 1));

		if (&cur[(sizeof(size_t) << 1) + length] >= (cur_pool + cur_pool_size))
			break;
	}

	return length;
}

#ifdef _REENTRANT
static void *_fsma_map_alloc_once(size_t size) {
	(void) pthread_once(&_mm_map_key_once, _mm_map_key_init);

	fsma_map_alloc = _fsma_map_alloc_many;

	return _fsma_map_alloc_many(size);
}
#endif

static void *_fsma_map_alloc_many(size_t size) {
	char *cur;
	size_t bfree;
	size_t rank, length, remain, cur_pool_size; 
#ifdef _REENTRANT
	char *cur_pool;

	cur_pool = pthread_getspecific(_mm_map_tlskey_cur_pool);
#endif

	if (!cur_pool) {
		if (!(cur_pool = _mm_map_init()))
			return NULL;
	}

	if (!size)
		return NULL;

	size = (size + (sizeof(void *) << 3) - 1) & ~((sizeof(void *) << 3) - 1);

	memcpy(&bfree, cur_pool, sizeof(size_t));
	memcpy(&cur_pool_size, &cur_pool[sizeof(size_t)], sizeof(size_t));

	if (bfree < (size + (sizeof(size_t) << 1))) {
		if (_mm_map_update_pool(cur_pool) < (size + (sizeof(size_t) << 1))) {
			if (!(cur_pool = _mm_map_change_pool(size, cur_pool)))
				return NULL;

			memcpy(&cur_pool_size, &cur_pool[sizeof(size_t)], sizeof(size_t));
		}

		memcpy(&bfree, cur_pool, sizeof(size_t));
	}

	if (bfree == (cur_pool_size - (sizeof(size_t) << 1))) {
		memcpy(&cur_pool[(sizeof(size_t) << 1)], (size_t [1]) { MM_MAP_RANK_USED }, sizeof(size_t));
		memcpy(&cur_pool[sizeof(size_t) * 3], &size, sizeof(size_t));
		bfree -= ((sizeof(size_t) << 1) + size);
		memcpy(cur_pool, &bfree, sizeof(size_t));

		return &cur_pool[sizeof(size_t) << 2];
	}

	for (cur = &cur_pool[sizeof(size_t) << 1]; ; cur += length + (sizeof(size_t) << 1)) {
		memcpy(&rank, cur, sizeof(size_t));
		memcpy(&length, &cur[sizeof(size_t)], sizeof(size_t));

		if (rank == MM_MAP_RANK_USED) {
			continue;
		} else if (rank == MM_MAP_RANK_TBF) {
			bfree += length + (sizeof(size_t) << 1);
			memcpy(cur, (size_t [1]) { MM_MAP_RANK_FREE }, sizeof(size_t));
		}

		if (!length) {
			if (&cur[(sizeof(size_t) << 1) + size] > (cur_pool + cur_pool_size)) {
				memcpy(cur_pool, &bfree, sizeof(size_t));

				if (!(cur_pool = _mm_map_change_pool(size, cur_pool)))
					return NULL;

				memcpy(&bfree, cur_pool, sizeof(size_t));
				memcpy(&cur_pool_size, &cur_pool[sizeof(size_t)], sizeof(size_t));

				cur = cur_pool;

				continue;
			}

			length = size;

			break;
		} else if (length < size) {
			length = _mm_map_defrag(cur_pool, cur_pool_size, cur, size, length, &bfree);

			if (length >= size)
				break;

			memcpy(cur, &length, sizeof(size_t));

			if (&cur[(sizeof(size_t) << 1) + length] >= (cur_pool + cur_pool_size)) {
				memcpy(cur_pool, &bfree, sizeof(size_t));

				if (!(cur_pool = _mm_map_change_pool(size, cur_pool)))
					return NULL;

				memcpy(&bfree, cur_pool, sizeof(size_t));
				memcpy(&cur_pool_size, &cur_pool[sizeof(size_t)], sizeof(size_t));

				cur = cur_pool - length;
			}
		} else {
			break;
		}
	}

	if ((remain = (length - size)) > (sizeof(size_t) << 2)) {
		remain -= sizeof(size_t) << 1;
		length = size;
		memcpy(&cur[(sizeof(size_t) << 1) + size], (size_t [1]) { MM_MAP_RANK_FREE }, sizeof(size_t));
		memcpy(&cur[(sizeof(size_t) << 1) + size + sizeof(size_t)], &remain, sizeof(size_t));
	}

	memcpy(cur, (size_t [1]) { MM_MAP_RANK_USED }, sizeof(size_t));
	memcpy(&cur[sizeof(size_t)], &length, sizeof(size_t));
	bfree -= ((sizeof(size_t) << 1) + length);
	memcpy(cur_pool, &bfree, sizeof(size_t));

	return &cur[sizeof(size_t) << 1];
}

void *fsma_map_realloc(void *region, size_t size) {
	char *new;
	size_t length;

	if (!region)
		return fsma_map_alloc(size + (size >> 1));

	memcpy(&length, &((char *) region)[-sizeof(size_t)], sizeof(size_t));

	if (length >= size)
		return region;

	if (!(new = fsma_map_alloc(size + (length >> 1)))) {
		if (!(new = fsma_map_alloc(size)))
			return NULL;
	}

	memcpy(new, region, length);

	memcpy(&((char *) region)[-(sizeof(size_t) << 1)], (size_t [1]) { MM_MAP_RANK_TBF }, sizeof(size_t));

	return new;
}

void *fsma_map_calloc(size_t nmemb, size_t size) {
	size_t tsize = nmemb * size;
	void *region = fsma_map_alloc(tsize);

	if (region)
		memset(region, 0, tsize);

	return region;
}

int fsma_map_memalign(void **memptr, size_t alignment, size_t size) {
	char *ptr_start, *ptr_cur;
	size_t length;

	if (!size) {
		*memptr = NULL;
		return 0;
	}

	if ((alignment & (alignment - 1)) || (alignment & (sizeof(void *) - 1)))
		return EINVAL;

	if (!(ptr_start = fsma_map_alloc(size + (alignment << 1))))
		return ENOMEM;

	ptr_cur = ptr_start;

	memcpy(&ptr_cur, (uintptr_t [1]) { ((((uintptr_t) ptr_cur) + (alignment - 1)) & ~(alignment - 1)) + alignment }, sizeof(uintptr_t));

	memcpy(&length, &ptr_start[-sizeof(size_t)], sizeof(size_t));
	memcpy(&ptr_start[-(sizeof(size_t) << 1)], (size_t [1]) { MM_MAP_RANK_TBF }, sizeof(size_t));
	memcpy(&ptr_start[-sizeof(size_t)], (size_t [1]) { ptr_cur - ptr_start - (sizeof(size_t) << 1) }, sizeof(size_t));
	memcpy(&ptr_cur[-(sizeof(size_t) << 1)], (size_t [1]) { MM_MAP_RANK_USED }, sizeof(size_t));
	memcpy(&ptr_cur[-sizeof(size_t)], (size_t [1]) { length - (ptr_cur - ptr_start) }, sizeof(size_t));

	*memptr = ptr_cur;

	return 0;
}

void fsma_map_free(void *region) {
	if (region)
		memcpy(&((char *) region)[-(sizeof(size_t) << 1)], (size_t [1]) { MM_MAP_RANK_TBF }, sizeof(size_t));
}

#ifdef LIBC_OVERRIDE

void *malloc(size_t size) {
	return fsma_map_alloc(size);
}

void *calloc(size_t nmemb, size_t size) {
	size_t tsize = nmemb * size;
	void *region = fsma_map_alloc(tsize);

	if (region)
		memset(region, 0, tsize);

	return region;
}

void *realloc(void *ptr, size_t size) {
	return fsma_map_realloc(ptr, size);
}

void free(void *ptr) {
	fsma_map_free(ptr);
}

#endif

