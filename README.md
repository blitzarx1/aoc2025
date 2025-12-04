Advent of Code 2025 — C Solutions
=================================

This repository contains my solutions for Advent of Code 2025, implemented in C. Each puzzle day has its own folder with a standalone main.c and optional helper library file.

Project Structure

./
├── day1/
│   ├── main.c     # Day 1 solution entry point
│   └── lib.h      # Optional helper header
├── day2/
│   ├── main.c
│   └── lib.h
├── ...
├── lib/           # Shared utilities for all days
├── bin/           # Compiled executables (generated)
└── Makefile       # Build & run logic

Usage

Build a specific day:

make build day=2 log_level=4

- day — folder number (day2, day3, ...)
- log_level — value passed as -DLOG_LEVEL=<n> to control logging

The executable will appear under:

./bin/day2

Run a specific day:

make run day=2

This automatically builds the day (if needed) and runs:

./bin/day2

Clean build artifacts:

make clean

Removes the bin/ directory and all compiled executables.

Notes

- Each day is independent and contains its own entry point.
- Common helper functions should be placed in the lib/ directory.
- Logging level is configurable via LOG_LEVEL, allowing for debug output during development.

Happy Coding & Happy Advent of Code!

Feel free to fork, improve, or compare your solutions with mine.
[aoc2025](https://adventofcode.com/2025)
