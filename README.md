# mcc
Mike's C Preprocessor from the good ole days

## Purpose of the original program:

In May of 1985 I took a new job at a large international software firm. The team I joined did not have experience in the C language, but I did.  I developed a C Programmer's Handbook and taught that material to the team.   Most of them had experience in assembler code an PL/1, probably Fortran too. 

The team had been assembled to port a large product from PL/1 to C.   We did one port to a particular supercomputer architecture.  The code base being ported was the output of a very large project at corporate HQ.  They were in the midst of converting all their code from PL/1 to C and targeted to IBM PCs running Windows at first. Even though the target machine was small, the code base was enormous with many dynamically loaded pieces. The large team was thinking this new C code would be running on a very limited CPU and Memory architecture (remember nobody will ever need more than 640KB of memory).  But we were trying to host that code on a super computer with 64 bit processor and an enormous memory space implemented as virtual memory backed by large disk volumes.   

It worked and we took it all the way through full release testing.   However, that product never was shipped because of market size. 

So, we signed up for a new project to do the same thing for a new RISC architecture running UNIX.  The new architecture was pretty powerful and being UNIX the OS was very amenable to development.   Their C compiler was pretty good, but because of the code architecture of many separately compiled dynamically loaded modules the use of the C preprocessor was very significant.  During compilation the header file expansion and conditional compilation took a very long time.  We received the latest code updates on several 9 track mag tape reels once a week.   It took more than a week to do the preprocessing.   We needed to speed it up massively.  There were several hundreds of header files nexted about 12 levels deep.  There were a large number of predefined constants that controlled a complex set of conditional header inclusions for different hardware architectures and operating systems.  

## Introducing MCC - Mike C Compiler. 
The year was early 1986, a full 40 years ago.    
It wasn't a full compiler yet (and never will), but it is a very powerful preprocessor.   

### Precompilation. 

The reason for that power was that I had implemented "pre-compiled header files".  

On the first pass of compilation on a particular platform the vast majority of the source code was eliminated by condtional definitions.  A handful of key header files were determined to be strategic.  We would preprocess those parts of the tree and saved the final output as a type of header file with the extension of .pch rather than .h.   Safeguards were in place to ensure that future compilations exactly fit the criteria under which the .pch file had been built. 

### Other features:

The compiled source listing had a number of extra features.  The one I found most useful is the alphabetic index of all symbols, especially #defined constants.   If you were reading a C source and encounter a #define symbol, you could refer to the index and immediately find the exact line of a specific source file where that symbol was defined.   This avoided the problem where a symbol was defined in many header files and you could not tell which was the effective definition.   It also had a listing of each of the header files and their full pathname so you could jump straight to the individual header when there were numerous variations in the whole code base. 

### The end results:
I don't remember the specific numbers but MCC reduced a full compile from over a week into a couple of days. 

## Why am I working on MCC again?
As I said the MCC version 1 was written 40 years ago.  
I'm now retired and I just turned 70.  I'm looking back over my career and the massive changes in hardware and software development.  Even though my first project mentioned above was a fairly massive super computer which was very powerful, the first model we had ran at 1 million instructions per second and required a raised floor with serious air conditioning and power input.  

Today I am doing development on an M1 MacBookPro, and also on a 13' M4 iPadPro that is less than 6 mm thick.  I can carry it anywhere and do software development for many hours.  It has 2TB of solid state disk, and a 5G cellular modem.   

For quite a while developing C and Node.js etc was not an option on a tablet because of Apple's control rules.  I don't disagree with them in general.  

However, since all Mac, iPhone, and iPadOS applications are tightly sandboxed.  Basically every app seems to be an entire computer (virtual machine) with it's own copy of the OS.  And that OS is the Darwin version of UNIX.

There are now several Apps that are essentially full featured UNIX boxes.  They have shells, full UNIX and Node package managers.   Now, I can have far more powerful hardware than 40 super computers of 40 years ago and still have the good old days C development environments (plus a lot more).  

So, it is time for me to drag my old C programs into the new millenium.  

## C standards have evolved massively in the last 40 years.

When I was working on the original MCC version 1, I had frequent correspondence with my company's ANSI C expert that sat on the ANSI standard committee.   

Now, lots of things have changed (for the better), but the things I got away with (the old ways) are only tolerated by compilers if I suppress all manner of warnings and errors.   

## MCC 2.0 

### Step 1
I have originally populated this repository from 3.5" diskettes with old UPPERCASE 8.3 style filenames.  My first step is to rename MCCMAIN.C to mccmain.c.    

### Step 2
Organize the repository to work with modern IDE tools and new compilers like Clang (the C compiler for MacOS and iOS).   I am going to try to make one layout that I can use in these environments:

1. my a-Shell command line on my iPad running iPadOS 
2. my iSH shell command line on my iPad runing Ubuntu
3. command line Zsh on my Mac 
4. VS Code IDE on my Mac
5. XCode IDE on my Mac

XCode has a build system, but the old Makefile approach works as well. 


