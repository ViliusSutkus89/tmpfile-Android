# tmpfile
POSIX C library provides [`FILE *tmpfile(void)`](https://linux.die.net/man/3/tmpfile) function.
> The **tmpfile()** function opens a unique temporary file in binary read/write (w+b) mode.  
> The file will be automatically deleted when it is closed or the program terminates.  
> The standard does not specify the directory that **tmpfile()** will use.  
> Glibc will try the path prefix P_tmpdir defined in <stdio.h>, and if that fails the directory /tmp.

Bionic (Android's equivalent of glibc) also provides this function.

## The problem - /tmp folder

Bionic's *tmpfile*, just like glibc's *tmpfile*, attempts to create temporary files in */tmp*.
This is problematic, because */tmp* does not exist on Android and cannot be created by non-root users.
Lack of */tmp* results in failure of *tmpfile* function.

Numerous libraries rely on said function. None of them can be used on Android successfully.
Porting such libraries would require either:
* Updating them to not use this particular standard C library function,
* Implementing an alternative *tmpfile* function.
