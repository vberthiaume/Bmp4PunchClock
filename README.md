# Bmp4PunchClock
Awesome free command time punch clock! Allows user to capture time for different projects.
Times are written in a file for later reference, e.g., 

    --------------------------------
    Project0 - 2015-03-24
    20:54:55 - 20:54:58, 0:0:2
    TOTAL 0:0:2
    --------------------------------
    Project2 - 2015-03-24
    TOTAL 0:0:0
    --------------------------------
    Project1 - 2015-03-24
    20:55:23 - 20:55:27, 0:0:3
    TOTAL 0:0:3
    --------------------------------
    Project1 - 2015-03-24
    20:56:01 - 20:56:03, 0:0:2
    20:56:03 - 20:56:11, 0:0:7
    TOTAL 0:0:9


#Installation note
To compile with visual studio, you need to install the Dirent API:
http://www.softagalleria.net/dirent.php

Also, punchclock now uses the current folder for creating new files, reading current times, and summing them.


#The MIT License (MIT)

Copyright (c) 2015 Vincent Berthiaume

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
