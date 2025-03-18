/*
 * Copyright 2025 8dcc
 *
 * This file is part of dump-fat.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stddef.h>

#include "include/util.h"

bool is_mem_zero(const void* ptr, size_t size) {
    const char* arr = ptr;
    while (size-- > 0)
        if (*arr++ != 0)
            return false;
    return true;
}
