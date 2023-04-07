# RISCV-32I-Implementation

--------------------------------------------------------
A C++ implementation of RISCV-32I Processor
--------------------------------------------------------

README

Table of contents:
1. Developer Details
2. Directory Structure
3. How to build
4. How to execute
5. How to view output

Developer Details
-----------------

Developer's Name: Rhitvik Anand
Developer's Entry Number: 2021csb1127
Developer's Mail: rhitvik2000anand@gmail.com / 2021csb1127@iitrpr.ac.in

Developer's Name: Niti Shyamsukha
Developer's Entry Number: 2021csb1118
Developer's Mail: niti200005@gmail.com

Developer's Name: Ihita Sinha
Developer's Entry Number: 2021csb1095
Developer's Mail: 2021csb1095@iitrpr.ac.in

Directory Structure
-------------------

RISCV-32I-Implementation
  |
  |- bin
    |- RISCV
  |- doc
    |- Design Doc.docx
  |- include
    |- Core.h
    |- Instruction_Packet.h
    |- Pipeline.h
    |- RISCV.h
  |- src
    |- Core.cpp
    |- Instruction_Packet.cpp
    |- Pipeline.cpp
    |- main.cpp
    |- Makefile
    |- RISCV.cpp
  |- test
    |- array_sum.s
    |- array_sum.mc
    |- bubblesort.s
    |- bubblesort.mc
    |- fibonacci.s
    |- fibonacci.mc

How to build
------------

For compiling:
  $cd src
  $make

For cleaning the project:
  $cd src
  $make clean

How to execute
--------------

Command to execute program:
../bin/RISCV <knobs>

<knobs> --
          | -test <filename.mc>
          | -n <value of N>
          | -pipeline
          | -forward
          | -trace [<instruction number>]
          | -registers
          | -h (or) -help

(compulsory knob: -test <filename.mc>)
(all other knobs are optional:
          | -h (or) -help will show how to execute
          | default value of N is 5
          | default setting of pipelining, forwarding, tracing, and showing register values is off
          | if forward/trace is called without calling pipeline, it doesn't do anything
          | providing instruction number after -trace knob will activate tracing for that particular instruction only, otherwise for all instructions
          | -registers knob will show values in register file after every cycle)

How to view output
------------------

All data memory is stored in file ../src/Data Memory.mc
