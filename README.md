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

## Workaround
**Some** devices have */data/local/tmp/* available. Emphasis on the word some.  
Proper storage location is per-application cache folder, obtained using
[`context.getCacheDir()`](https://developer.android.com/reference/android/content/Context.html#getCacheDir()).

This library provides alternative *tmpfile* implementation based on [```mkstemp(char *template)```](https://linux.die.net/man/3/mkstemp).
By default temporary files are created in */data/local/tmp*.
Function `Tmpfile::set_tmpfile_dir(const char *tmpfile_dir)` allows temporary file creation in cache folder.

## Usage
On Java side:
1. Load tmpfile library.
2. Obtain application's cache directory using [`context.getCacheDir()`](https://developer.android.com/reference/android/content/Context.html#getCacheDir()).

On C++ side:
1. Pass cache path to `Tmpfile::set_cache_dir(const char *cache_dir)`.
2. Create temporary files using `FILE *tmpfile(void)`.

## Loading tmpfile library
Tmpfile library needs to be loaded before the library that uses *tmpfile* function.
```Java
public class MainActivity extends Activity {
    static {
        System.loadLibrary("tmpfile");
        System.loadLibrary("my-library");
    }
    ...
}
```

Alternatively, your library could be linked against *tmpfile*.
```CMake
add_library(my-library sources.cpp)
find_package(tmpfile REQUIRED)
target_link_libraries(my-library tmpfile)
```

## Downside
Temporary file created by glibc's *tmpfile* is automatically deleted on file closure.  
This library does not subscribe to such events, meaning that temporary file deletion happens when:
* Program terminates.
* Function `Tmpfile::cleanup_tmpfiles()` is called.

## @TODO:
Subscribe to FILE closure events using
https://developer.android.com/reference/android/os/FileObserver.html#CLOSE_WRITE
