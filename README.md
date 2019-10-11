# tmpfile
POSIX C library provides [`FILE *tmpfile(void)`](https://linux.die.net/man/3/tmpfile) function.
> The **tmpfile()** function opens a unique temporary file in binary read/write (w+b) mode.  
> The file will be automatically deleted when it is closed or the program terminates.  
> POSIX.1-2001 specifies: an error message may be written to stdout if the stream cannot be opened.  
> The standard does not specify the directory that **tmpfile()** will use.  
> Glibc will try the path prefix P_tmpdir defined in <stdio.h>, and if that fails the directory /tmp.

Bionic (Android's equivalent of glibc) also provides this function.

## The problem - /tmp folder

Bionic's *tmpfile*, just like glibc's *tmpfile*, attempts to create temporary files in */tmp*.
This is problematic, because */tmp* does not exist on Android and cannot be created by non-root users.
Lack of */tmp* results in failure of *tmpfile* function.

Bionic's *tmpfile* interprets the word "may" as "may or may not" and does not print any error messages to *stdout* alerting the user that such an error occurred.

Numerous libraries rely on said function. None of them can be used on Android successfully.
Porting such libraries would require either:
* Updating them to not use this particular standard C library function,
* Implementing an alternative *tmpfile* function.

## Workaround
This library provides alternative *tmpfile* implementation based on [```mkstemp(char *template)```](https://linux.die.net/man/3/mkstemp).

**Some** devices have */data/local/tmp/* available. Emphasis on the word some.
Directory */data/local/tmp* is used as a hardcoded default location.

Proper storage location is per-application cache directory, obtained at runtime in Java side using [`context.getCacheDir()`](https://developer.android.com/reference/android/content/Context.html#getCacheDir()).  
[Tmpfile.java](/android/tmpfile/src/main/java/com/viliussutkus89/tmpfile/Tmpfile.java) implements a *ContentProvider*,
which executes automatically on application startup with a goal to obtain said directory, to be used in subsequent *tmpfile* function calls.

Glibc's *tmpfile* automatically deletes created temporary files when they are closed.
This library provides such functionality by using *FileObserver* (in *Tmpfile.java*) to observe "file close" events of created temporary files and garbage collect them.

## Requirements for users
This library requires Android API level 16 (Jelly Bean 4.1.x) or newer.
Such a requirement is imposed by the current versions of Android Native Development Kit.

## Usage

Library provides a proof of concept [sample application](/sampleapp).

Loading tmpfile library is done in three (read: four) steps:
* Downloading the library to app/libs/tmpfile.aar.
* Including app/libs/tmpfile.aar as a dependency in Gradle. This step bundles Tmpfile.java and libtmpfile.so into your application's APK.
* Linking your native (C / C++) binaries against *tmpfile* library (libtmpfile.so). This step, in effect, redirects *tmpfile* function calls to libtmpfile.so

#### Dependency in Gradle
[sampleapp/app/build/build.gradle](sampleapp/app/build/build.gradle) contains code to load the library as a dependency in Gradle.
```gradle
dependencies {
    implementation files('libs/tmpfile.aar')
    ...
}
```

#### Linking native binaries against libtmpfile.so

Sadly, native code (through CMake) is compiled and linked before Gradle extracts native libraries from dependencies.
This makes linking against native libraries (.so files), delivered as Android libraries (.aar) through Gradle, somewhat problematic.

As a workaround, sample application implements a Gradle task named *extractLibtmpfileSoForLinkingInCMake*, which is executed before *preBuild* task.
Task extractLibtmpfileSoForLinkingInCMake extracts native libraries (.so files) from tmpfile.aar into application's build directory, so that they could be linked against in CMake.
```gradle
task extractLibtmpfileSoForLinkingInCMake(type: Copy) {
    from zipTree("${project.rootDir}/app/libs/tmpfile.aar")
    into "${project.buildDir}/tmpfile/"
    include "jni/**/libtmpfile.so"
}
preBuild.dependsOn extractLibtmpfileSoForLinkingInCMake
```

Once libtmpfile.so is available, it can be used to link your binaries against it.

Sample application uses [CMakeLists.txt](sampleapp/app/src/main/cpp/CMakeLists.txt) to build a native library called "native-lib".

To link against libtmpfile.so, it needs to be defined as an imported target (read: provided as a ready to use binary):
```CMake
add_library(tmpfile SHARED IMPORTED)
set_property(TARGET tmpfile PROPERTY IMPORTED_LOCATION
  # libtmpfile.so will be extracted in this directory by
  # task extractLibtmpfileSoForLinkingInCMake in sampleapp/app/build.gradle
  ${CMAKE_CURRENT_LIST_DIR}/../../../build/tmpfile/jni/${ANDROID_ABI}/libtmpfile.so
)
```

Once a CMake target is available, it can be linked against.
```CMake
target_link_libraries(native-lib tmpfile)
```

## Issues
Glibc's *tmpfile* automatically deletes temporary files when they are closed or on program termination.
Temporary files created by this library are automatically deleted when they are closed.
Garbage collection on program termination is not currently available.

To work around this limitation, close your temporary files when you are done using them:
```C++
FILE * fh = tmpfile();
....
fclose(fh);
```
