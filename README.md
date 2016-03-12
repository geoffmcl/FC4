# FC4 20160312

### Summary

A personal File Compare utility. There may be a lot of better utilities out there, like th eunix `diff`, but this was my own personal project to write such a utility, with the options I thought best, before I had even heard of `diff`...

It has the idea to compare the files line by line, until it finds a difference... Then it will proceed down file 2 searching for the same line, and come back and search down file 1 trying to find the same as the changed line. If both these succeed it will report the shortest. If only one or the other succeeds, then it will report that.

If neither succeed then will go down one line in each file, and again do the line compare... and so on...

This type of rocking back and forth was the algorithm chosen for this compare...

Of course there is also a pure binary compare, that just does a byte by byte compare...

It suits my purpose, but most of the time these days I use the venerable unix `diff` utility, which also has lots of options...

This was developed as a Windows (WIN32) app, and the primary purpose of this repo is so that I can clone it in linux and work on a unix port. Will see how successful that is...

### Building

This projects uses [CMake](https://cmake.org/download/) native build file generator. Typically it is only required to do -

```
cd build # get into the build directory
cmake .. [options] # generate the default build files
cmake --build . --config Release    # proceed to compile and link.
If in unix, this will typically be `make`
```

As simple as that...

The license is GNU GPL versio 2 - See LICENSE.txt

Enjoy...

Geoff  
20160312 - 20040902

; eof
