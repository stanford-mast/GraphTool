###############################################################################
# GraphTool
#   Tools for manipulating graphs
###############################################################################
# Authored by Samuel Grossman
# Department of Electrical Engineering, Stanford University
# Copyright (c) 2016-2017
###############################################################################
# Makefile
#   Build script for GNU-compatible Linux operating systems.
###############################################################################


# --------- PROJECT PROPERTIES ------------------------------------------------

PROJECT_NAME                = graphtool
PLATFORM_NAME               = linux

LIBRARY_DEPENDENCIES        = libspindle libsilo libtopo libhwloc libpthread libnuma libpciaccess libxml2

SOURCE_DIR                  = source
INCLUDE_DIR                 = include

OUTPUT_BASE_DIR             = output
OUTPUT_DOCS_DIR             = $(OUTPUT_BASE_DIR)/docs
OUTPUT_DIR                  = $(OUTPUT_BASE_DIR)/$(PLATFORM_NAME)
OUTPUT_FILE                 = $(PROJECT_NAME)

C_SOURCE_SUFFIX             = .c
CXX_SOURCE_SUFFIX           = .cpp


# --------- TOOL SELECTION AND CONFIGURATION ----------------------------------

CC                          = gcc
CXX                         = g++
AS                          = as
LD                          = g++

CCFLAGS                     = -g -O3 -Wall -std=c11 -march=core-avx2 -masm=intel -mno-vzeroupper -pthread -I$(INCLUDE_DIR) -D_GNU_SOURCE
CXXFLAGS                    = -g -O3 -Wall -std=c++11 -march=core-avx2 -masm=intel -mno-vzeroupper -pthread -I$(INCLUDE_DIR)
LDFLAGS                     = -g


# --------- FILE ENUMERATION --------------------------------------------------

OBJECT_FILE_SUFFIX          = .o
DEP_FILE_SUFFIX             = .d

C_SOURCE_FILES              = $(wildcard $(SOURCE_DIR)/*$(C_SOURCE_SUFFIX))
CXX_SOURCE_FILES            = $(wildcard $(SOURCE_DIR)/*$(CXX_SOURCE_SUFFIX))
ALL_SOURCE_FILES            = $(C_SOURCE_FILES) $(CXX_SOURCE_FILES)

OBJECT_FILES_FROM_SOURCE    = $(patsubst $(SOURCE_DIR)/%, $(OUTPUT_DIR)/%$(OBJECT_FILE_SUFFIX), $(ALL_SOURCE_FILES))
DEP_FILES_FROM_SOURCE       = $(patsubst $(SOURCE_DIR)/%, $(OUTPUT_DIR)/%$(DEP_FILE_SUFFIX), $(ALL_SOURCE_FILES))

LINK_LIBRARIES              = $(patsubst lib%, -l%, $(LIBRARY_DEPENDENCIES))


# --------- TOP-LEVEL RULE CONFIGURATION --------------------------------------

.PHONY: graphtool help clean


# --------- TARGET DEFINITIONS ------------------------------------------------

graphtool: $(OUTPUT_DIR)/$(OUTPUT_FILE)

docs: | $(OUTPUT_DOCS_DIR)
	@doxygen

help:
	@echo ''
	@echo 'Usage: make [target]'
	@echo ''
	@echo 'Targets:'
	@echo '    graphtool'
	@echo '        Default target. Builds GraphTool.'
	@echo '    docs'
	@echo '        Builds HTML and LaTeX documentation using Doxygen.'
	@echo '    clean'
	@echo '        Removes all output files, including binary and documentation.'
	@echo '    help'
	@echo '        Shows this information.'
	@echo ''


# --------- BUILDING AND CLEANING RULES ---------------------------------------

$(OUTPUT_DIR)/$(OUTPUT_FILE): $(OBJECT_FILES_FROM_SOURCE)
	@echo '   LD        $@'
	@$(LD) $(LDFLAGS) -o $@ $(OBJECT_FILES_FROM_SOURCE) $(LINK_LIBRARIES)
	@echo 'Build completed: $(PROJECT_NAME).'

clean:
	@echo '   RM        $(OUTPUT_BASE_DIR)'
	@rm -rf $(OUTPUT_BASE_DIR)
	@echo 'Clean completed: $(PROJECT_NAME).'


# --------- COMPILING AND ASSEMBLING RULES ------------------------------------

$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR)

$(OUTPUT_DOCS_DIR):
	@mkdir -p $(OUTPUT_DOCS_DIR)

$(OUTPUT_DIR)/%$(C_SOURCE_SUFFIX)$(OBJECT_FILE_SUFFIX): $(SOURCE_DIR)/%$(C_SOURCE_SUFFIX) | $(OUTPUT_DIR)
	@echo '   CC        $@'
	@$(CC) $(CCFLAGS) -MD -MP -c -o $@ $<

$(OUTPUT_DIR)/%$(CXX_SOURCE_SUFFIX)$(OBJECT_FILE_SUFFIX): $(SOURCE_DIR)/%$(CXX_SOURCE_SUFFIX) | $(OUTPUT_DIR)
	@echo '   CXX       $@'
	@$(CXX) $(CXXFLAGS) -MD -MP -c -o $@ $<

-include $(DEP_FILES_FROM_SOURCE)
