/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Mario Luzeiro <mrluzeiro@ua.pt>
 * Copyright (C) 1992-2015 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file  xv3d_types.h
 * @brief
 */

#ifndef XV3D_TYPES_H
#define XV3D_TYPES_H

#define GLM_FORCE_INLINE
#define GLM_FORCE_RADIANS

// Disable SIMD detection
//#define GLM_FORCE_PURE

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_precision.hpp>

using SFVEC2UI      = glm::uvec2;
using SFVEC2I       = glm::ivec2;
using SFVEC2UI64    = glm::u64vec2;
using SFVEC2I64     = glm::i64vec2;
using SFVEC2F       = glm::vec2;
using SFVEC2D       = glm::dvec2;
using SFVEC3F       = glm::vec3;
using SFVEC3D       = glm::dvec3;
using SFVEC4F       = glm::vec4;
using SFVEC3UI      = glm::uvec3;
using SFVEC3D       = glm::dvec3;

#define CLASS_ALIGNMENT 16

#endif // XV3D_TYPES_H
