## MyLib

This repository contains code samples for my program that I have been written.

It is licensed under the [MIT license](http://opensource.org/licenses/mit-license.php).

Feel free to modify the code and bug reports are welcome.

### How to build

```
cd MyLib/script
```

For Linux:

+ Edit cmake command in build.sh

```
./build.sh
```

For Windows MinGW:

+ Edit MINGW and CMAKE path in mingw.bat
+ Edit cmake command in cmake.bat

```
cmake.bat
make.bat
```

### How about the library

+ common

> I use Log and Exception most frequencyly and other classes.

+ encoding

> I know there are many libraries, but not all are in C++. There are base64, CRC, SHA, unicode convertion, traditional and simplified chinese convertion in UTF-8.

+ file

> The folder includes classes for file format:

> Csv, Mp3's Id3v2, Windows's IniFile, Bmp and Wav are written by myself.

> Xml and Json are my practices for interpreter pattern.

> Jpg and Png need dependency libraries (libjpeg-turbo, libpng and libz).

> BufferedFile solves slow file I/O operation for small data size between application and system.

+ memory

> Memory management is very important. There are smart pointer and small object pattern (ObjectPool).

> The ObjectPool is implemented by embedded pointer, thus it does not consume extra memory for link list. It also can be used as STL allocator (std::list and std::map, but not std::vector. Because my ObjectPool is segment, not support std::RandomAccessIterator).

+ network

> Some classes about network programing for easy using: (TCP, UDP and RAW) blocking/non-blocking socket, ICMP, HTTP(s) (need openssl), HTTP header, cookie.

> Memory management is very important. There are smart pointer and small object pattern (ObjectPool).

+ pattern

> This folder contains some classes about design pattern. Although some of them relates to threading: Runnable, EventLoop, ThreadPool (WorkerThread), Future.

+ system

> Includes Clipboard, FileExplorer (some directory operations) and ProcessTimes (measure CPU us/sy time).

+ thread

> There are some classes for multi-threading: atomic operation, semaphore, mutex, condition variable, thread and timer.

+ type

> Some data type of classes usually used.

+ test

> This library does not be well documented, but there are some helpful testing codes.
