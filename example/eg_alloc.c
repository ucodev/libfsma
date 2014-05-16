/**
 * @file eg_alloc.c
 * @brief Fast Scalable Memory Allocator Library (libfsam)
 *        Example - eg_alloc.c
 *
 * Date: 14-06-2013
 * 
 * Copyright 2013 Pedro A. Hortas (pah@ucodev.org)
 *
 * This file is part of libfsam.
 *
 * libfsam is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libfsam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libfsam.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <stdio.h>
#include <string.h>

#include "fsma.h"

int main(int argc, char *argv[]) {
	char *ptr = NULL;

	if (argc != 2) {
		fprintf(stderr, "Syntax: %s <string>\n", argv[0]);
		return 1;
	}

	if (!(ptr = fsma_malloc(strlen(argv[1]) + 1))) {
		fprintf(stderr, "Failed to allocate memory\n");
		return 1;
	}

	strcpy(ptr, argv[1]);

	printf("Alloc'd memory contents: %s\n", ptr);

	fsma_free(ptr);

	return 0;
}

