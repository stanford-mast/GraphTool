GraphTool is a utility for manipulating graphs and converting them between representation formats.

GraphTool is implemented using C++.


# Requirements

To build and link with GraphTool, the following are required.

- 64-bit x86-compatible processor with support for AVX2 instructions
  
  GraphTool has been tested with Intel processors of the Haswell generation and newer.
  Non-Intel processors may also be compatible if they support the required instructions.
  Do not attempt to use it on older-generation processors, at the risk of encountering "Illegal Instruction" errors.
  
- Windows 10 Pro or Ubuntu 14.04
  
  64-bit versions of these operating systems are required.
  Other Linux distributions are likely compatible, but they have not been tested.

- [**Spindle**](https://github.com/stanford-mast/Spindle) and its dependencies
  
  Spindle provides the multi-threading support that GraphTool uses internally.

- [**Silo**](https://github.com/stanford-mast/Silo) and its dependencies
  
  Silo provides NUMA awareness functionality that GraphTool uses internally.
  
- [**Parutil**](https://github.com/stanford-mast/Parutil) and its dependencies
  
  Parutil provides some parallel utility functions that GraphTool uses internally.


# Building

The Windows build system is based on Visual Studio 2015 Community Edition. Compilation is known to work from the graphical interface, but command-line build is also likely possible.

To build on Linux, just type `make` from within the repository directory.


# Using

General usage information is available by typing `graphtool --help` at the command line.

GraphTool reads a graph from a single input file and is capable of generating multiple outputs in different formats and with different settings all from this single input.  As a result, the options `--inputfile`, `--inputformat`, `--outputfile`, and `--outputformat` are all required.  Other output options may be specified as needed.

Options that control the input may be specified at most once and in any order.  Options that control the outputs can be specified many times.  All options that control the same setting are enqueued onto a queue within GraphTool, and each queue is popped once per output file.  This means, for example, that specifying `--outputfile=out1 --outputfile=out2 --outputfile=out3 --outputformat=ligra --outputformat=grazelle --outputformat=xstream` would produce three output files: `out1` using `ligra` format, `out2` using `grazelle` format, and `out3` using `xstream` format.  The options `outputfile` and `outputformat` can be interspersed; it is the order of each option with respect to other options of the same type that matters.  As a result, the same functionality can be obtained by specifying `--outputfile=out1 --outputformat=ligra --outputfile=out2 --outputformat=grazelle --outputfile=out3 --outputformat=xstream`.  If an optional setting that controls an output is specified fewer times than there are output files, then it is applied to the first several outputs and then the default is used for the remainder.

`--inputformat` specifies the representation format of the graph being read as input.  Supported values are `grazelle` (Grazelle's binary edge list format).

`--outputformat` specifies the representation format of one of the output files.  Supported values are `grazelle` (Grazelle's binary edge list format), `ligra` (Ligra's text-based adjacency list format), `polymer` (same as `ligra`), `graphmat` (matrix format used by GraphMat), and `xstream` (binary edge list format used by X-Stream, including the additional metadata file).

`--inputweights` is used to control whether or not GraphTool reads edge weights from the input file and, if so, the data type of these weights.  Supported values are `none` (unweighted), `int` (64-bit unsigned integers), and `float` (double-precision floating-point), with unweighted being the default.

`--outputweights` is used to control the type of edge weights written for each output file.  The graph itself must be weighted, either from having edge weights read from the file or generated internally by GraphTool.  A weighted graph can be used to produce an unweighted output.

`--outputgroup` is used to specify whether the output file should be grouped by source or by destination vertex.  This can be useful for optimizing the output to be processed by a pull-based engine or a push-based engine.  Supported values are `source` and `dest`, the former being the default.

`--outputoptions` is intended to pass options that can fine-tune output generation.  Currently it is not implemented.

`--transform` selects one or more transformation operations to apply to the graph, in the order specified on the command-line, after the graph is read from input and before any outputs are produced.  Currently this can be used only for the purpose of generating edge weights.  Supported values are `nullintedgedata` (generate integer-typed edge data of value 0), `nullfloatedgedata` (generate float-typed edge data of value 0.0), and `hashedgedata` (generate integer-typed edge data using a multiplicative hash).


# Modifying

GraphTool uses a class hierarchy to encapsulate the format-specific logic for input, output, and graph transformations.  To add support for additional formats, simply introduce a class derived from a suitable base class (`GraphReader`, `GraphWriter`, or `GraphTransform`) and implement the required methods.


# Source Code Documentation

Documentation is available and can be built using Doxygen.

On Linux, type `make docs` to compile the documentation. On Windows, run the Doxygen tool using the repository directory as the working directory (it may be necessary to create the output directory manually first).


# Copyright

GraphTool is licensed under BSD 3-clause (see "LICENSE" in the top-level source code directory).

Copyright (c) 2016-2017 Stanford University, Department of Electrical Engineering.
Authored by Samuel Grossman.
