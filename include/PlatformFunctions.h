/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file PlatformFunctions.h
 *   Definitions that abstract platform specifics for certain function calls.
 *****************************************************************************/

#pragma once

#include "VersionInfo.h"


// -------- PLATFORM-SPECIFIC MACROS --------------------------------------- //

#ifdef __PLATFORM_WINDOWS

#define strnicmp                                _strnicmp
#define _mm256_extract_epi64(v256, idx)         v256.m256i_u64[idx]
#define _mm256_insert_epi64(v256, val, idx)     v256.m256i_u64[idx] = val

#endif

#ifdef __PLATFORM_LINUX

#define strnicmp                                strncasecmp

#endif
