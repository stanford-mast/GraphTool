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

#endif

#ifdef __PLATFORM_LINUX

#define strnicmp                                strncasecmp

#endif
