/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file VersionInfo.h
 *   Definitions that provide information on the version of this program.
 *   Includes platform, compiler, and optimizations.
 *****************************************************************************/

#pragma once


// -------- CONSTANTS ------------------------------------------------------ //

#define __PROGRAM_NAME                          "GraphTool"


// -------- PLATFORM-SPECIFIC CHECKS AND MACROS ---------------------------- //

#if defined(_WIN64)

#define __PLATFORM_NAME                         "Windows/msvcrt x64"
#define __PLATFORM_WINDOWS

#ifndef _MSC_BUILD
#error "Your compiler is not supported. Compile with Microsoft Visual Studio when targetting Windows."
#endif

#elif defined(__gnu_linux__) && defined(__amd64__)

#define __PLATFORM_NAME                         "Linux/glibc x64"
#define __PLATFORM_LINUX

#else
#error "Your platform is not supported. This program is intended for Windows or GNU-compatible Linux on x86-64."
#endif


// -------- HARDWARE-SPECIFIC CHECKS AND MACROS ---------------------------- //

#ifndef __AVX2__
#error "Your target CPU is not supported. This program requires AVX2-generation instructions."
#endif
