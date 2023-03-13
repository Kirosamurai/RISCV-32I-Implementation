# RISCV-32I-Implementation
--------------------------------------------------------
A C++ implementation of RISCV-32I Single cycle processor
--------------------------------------------------------

README

Table of contents:
1. Developer Details
2. Directory Structure
3. How to build
4. How to execute

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
  |- Project Statement
  |- bin
    |- RISCV
  |- doc
    |- Design Doc.docx
  |- include
    |- RISCV.h
  |- src
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

For single cycle processor:
  $cd src
  $make all

For cleaning the project:
  $cd src
  $make clean

How to execute
--------------

To execute with default value of N (=5):
	./RISCV test/<file name>.mc

To execute with specific value of N:
	./RISCV test/<file name>.mc <N>

Note:
	<file name> cane be replaced with:
		1. array_sum
		2. bubblesort
		3. fibonacci
	<N> can be replaced with:
		any positive integer
