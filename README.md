# tmpfile-Android

[![Maven Central](https://img.shields.io/maven-central/v/com.viliussutkus89/tmpfile-android.svg?label=Maven%20Central)](https://search.maven.org/search?q=g:com.viliussutkus89%20AND%20a:tmpfile-android)
[![privilegedBuild](https://github.com/ViliusSutkus89/tmpfile-Android/actions/workflows/privilegedBuild.yml/badge.svg)](https://github.com/ViliusSutkus89/tmpfile-Android/actions/workflows/privilegedBuild.yml)

POSIX C library provides [`FILE *tmpfile(void)`](https://linux.die.net/man/3/tmpfile) function.
> The **tmpfile()** function opens a unique temporary file in binary read/write (w+b) mode.  
> The file will be automatically deleted when it is closed or the program terminates.  
> POSIX.1-2001 specifies: an error message may be written to stdout if the stream cannot be opened.  
> The standard does not specify the directory that **tmpfile()** will use.  
> Glibc will try the path prefix P_tmpdir defined in <stdio.h>, and if that fails the directory /tmp.

Bionic (Android's equivalent of Glibc) also provides this function.

## The problem - /tmp directory

Bionic's *tmpfile*, just like Glibc's *tmpfile*, attempts to create temporary files in */tmp*.
This is problematic, because */tmp* does not exist on Android and cannot be created by regular applications.
Lack of */tmp* results in failure of *tmpfile* function.

Bionic's *tmpfile* interprets the word "may" as "may or may not" and does not print any error messages to *stdout* alerting the user that an error occurred.

Numerous libraries rely on said function. None of them can be used on Android successfully.

Porting such libraries would require either to:
* Patch them to not use this particular function,
* Implement an alternative *tmpfile* implementation.

## Workaround

This library provides an alternative *tmpfile* implementation.

**Some** devices have */data/local/tmp/* available. Emphasis on the word some.  
Directory */data/local/tmp* is used as a hardcoded default when `tmpfile` is used without initialization.

Proper storage location is within application's cache directory, which is obtained at runtime in Java using
[`context.getCacheDir()`](https://developer.android.com/reference/android/content/Context.html#getCacheDir()).

tmpfile-Android needs to be initialized before usage. Initialization is done by calling static method `Tmpfile.init(getApplicationContext().getCacheDir());`.
[Application class](/sampleapp/app/src/main/java/com/viliussutkus89/android/tmpfile/sampleapp/MyApplication.java) is an exemplar place to make the call.

Standalone console programs (.exe's) don't call Java init code. `tmpfile()` overload in standalone programs make use of *TMPDIR* environment value.

Glibc's *tmpfile* creates temporary files and automatically deletes them whey they are closed or the program terminates.  
This library:
1) Uses [```mkstemp(char *template)```](https://linux.die.net/man/3/mkstemp) to create the temporary file,
2) Obtains the ```FILE *``` handle,
3) Removes the created temporary file,
4) Returns the handle to be used by the caller.

POSIX allows deleting open files.
Deleted file is available as long as you still have the handle.
It cannot be opened again and is not shown in it's parent directory.  
Deleted file is actually deleted from disk and used space reclaimed when all the handles are closed.  
Program termination, including crashing the program, closes the handles owned by the process.

## Usage

A proof of concept [sample application](sampleapp) is provided.

#### Requirements - Runtime
Current versions of Android Native Development Kit requires Android API level 16 (Jelly Bean 4.1.x) or newer.

#### Requirements - Compile-time
For easier linking tmpfile-Android delivers a [Prefab package](https://developer.android.com/studio/build/native-dependencies).
Prefab support requires Android Gradle Plugin 4 or newer.

#### Dependency in Gradle
[project level build.gradle](sampleapp/app/build.gradle) contains code to load the library as a dependency in Gradle.
```gradle
dependencies {
    implementation 'com.viliussutkus89:tmpfile-android:3.0.4'
}
```

#### Linking native binaries against libtmpfile.so

Library is linked against in [CMake](sampleapp/app/src/main/cpp/CMakeLists.txt):
```CMake
find_package(tmpfile REQUIRED CONFIG)
target_link_libraries(YOUR_LIBRARY_NAME tmpfile::tmpfile)
```

Previously mentioned `find_package(tmpfile REQUIRED CONFIG)` requires Prefab feature to be enabled in [project level build.gradle](sampleapp/app/build.gradle):
```groovy
android {
    buildFeatures {
        prefab true
    }
}
```
