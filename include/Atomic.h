/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Atomic.h
 *   Platform-specific atomic function definitions.
 *****************************************************************************/

#pragma once

#include "VersionInfo.h"


// -------- PLATFORM-SPECIFIC MACROS --------------------------------------- //

#ifdef __PLATFORM_WINDOWS

#include <intrin.h>

#define AtomicAnd64                             _InterlockedAnd64
#define AtomicAnd32                             _InterlockedAnd
#define AtomicAnd16                             _InterlockedAnd16
#define AtomicAnd8                              _InterlockedAnd8

#define AtomicOr64                              _InterlockedOr64
#define AtomicOr32                              _InterlockedOr
#define AtomicOr16                              _InterlockedOr16
#define AtomicOr8                               _InterlockedOr8

#define AtomicXor64                             _InterlockedXor64
#define AtomicXor32                             _InterlockedXor
#define AtomicXor16                             _InterlockedXor16
#define AtomicXor8                              _InterlockedXor8

#endif

#ifdef __PLATFORM_LINUX

#define AtomicAnd64                             __sync_fetch_and_and
#define AtomicAnd32                             __sync_fetch_and_and
#define AtomicAnd16                             __sync_fetch_and_and
#define AtomicAnd8                              __sync_fetch_and_and

#define AtomicOr64                              __sync_fetch_and_or
#define AtomicOr32                              __sync_fetch_and_or
#define AtomicOr16                              __sync_fetch_and_or
#define AtomicOr8                               __sync_fetch_and_or

#define AtomicXor64                             __sync_fetch_and_xor
#define AtomicXor32                             __sync_fetch_and_xor
#define AtomicXor16                             __sync_fetch_and_xor
#define AtomicXor8                              __sync_fetch_and_xor

#endif
