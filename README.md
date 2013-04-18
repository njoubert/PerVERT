# PerVERT: Performance Visualization and Error Remediation Toolkit

**Authors:**
 
- Niels Joubert [@njoubert](http://github.com/njoubert)
- Eric Schkufza [@eschkufz](https://github.com/eschkufz)

This is a class project for Prof. Jeff Heer's CS448B class at Stanford. 

[Paper Link](https://github.com/njoubert/PerVERT/blob/master/paper/PerVERT_writeup.pdf?raw=true)

## Abstract

Performance tuning is an important step in the development large software systems. Examples include web-servers which routinely handle thousands of simultaneous content requests, and petaflop su- percomputers which perform physical simulations that span tens of thousands of cpu cores. As improvements in clock frequency slow and hardware trends continue towards increased parallelism, the runtime performance of these and similar systems will become ever more a function of memory efficiency. Unfortunately, the ability to effectively reason about this phenomenon using existing tools such as valgrind [Nethercote and Seward 2007], gprof [Graham et al. 2004], or gdb [Stallman and Pesch 1991], through a text-based in- terface, is limited, and tedious at best.

We present PerVERT, an instrumentation framework for logging a process’s virtual memory traffic and a visualization suite for rea- soning about common memory performance bugs: Are memory accesses organized coherently in both spatial and temporal dimen- sions? To what extent do these patterns differ based on program inputs or changes in source code?

## Running Code

The webserver is built in C/C++ on top of mongoose (included in this distribution).

First, compile the backend:

    cd ./backend
    make

To see one of our example analysis, you can run:

    ./debug_all

This will start up a backend server with many of our examples from /testdata loaded.
Now point your web browser to:

    http://localhost:8083/?exec=vector1

and hit the play button!

## Code Organization

First, a pintool plugin instruments an arbitrary executable to trace its memory usage, and dumps out log files. These log files are consumed by the web backend, and published to the web frontend where the user is provided with a visualization of memory activity.


## /Pintool

This is a plugin for the pintool binary analysis and instrumentation tool. 

## /Backend

This runs a mongoose webserver with a connect.js-like Layered Server Middleware layer. It serves the /frontend directory as static files.

The webserver backend reads the log files from our pintool plugin, 

## /Frontend

This contains all the client-side HTML, CSS and JavaScript that creates the interface for visualizing the results of a performance trace.

## /Examples

Contains snippets of code we analyze for memory usage

## /testdata

Contains the resulting pintool log files from analyzing the snippets of code in /examples