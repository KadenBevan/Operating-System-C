# Operating-System-C
This is an operating system that can run processes concurrently using POSIX threads. c 99

# Usage
This program was developed for Linux dantooine 2.6.32-754.2.1.el6.x86_64 #1 SMP Tue Jul 3 16:37:52 EDT 2018 x86_64 x86_64 x86_64 GNU/Linux

# Compiling
To compile this program use something make -f "PA04_mf.make" or something similar.
    The PA04_mf.make applies the following flags: -g -std=c99 -Wall -pedantic -pthreads.
    The make -f "PA04_mf.make" clean command can be use to get rid of .o & .exe files.
    The PA04_mf.make file outputs an application named PA04
# Running
To run the program type ./PA04 "config file" where the config file is a valid config file as per the specifications.
The metadata file must be in the same directory as the config file and application.

# For More Information
Read the OS_Specifications_05Nov2018_v01.pdf in the Docs dir.
