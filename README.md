# tmpfile-Android

![Build](https://github.com/ViliusSutkus89/tmpfile-Android/workflows/build/badge.svg)
[![Download](https://api.bintray.com/packages/viliussutkus89/maven-repo/tmpfile-android/images/download.svg)](https://bintray.com/viliussutkus89/maven-repo/tmpfile-android/_latestVersion)

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
Directory */data/local/tmp* is used as a hardcoded default location for the creation of temporary files.

Proper storage location is per-application cache directory, obtained at runtime in Java using
[`context.getCacheDir()`](https://developer.android.com/reference/android/content/Context.html#getCacheDir()).
    
[Tmpfile.java](/lib/src/main/java/com/viliussutkus89/android/tmpfile/Tmpfile.java) implements a [ContentProvider](https://developer.android.com/reference/android/content/ContentProvider)
to automatically obtain application's cache directory on application start up.

Standalone console programs (.exe's) don't care about ContentProviders. Tmpfile overload in standalone programs make use of TMPDIR environment value.

Glibc's *tmpfile* creates temporary files and automatically deletes them whey they are closed or the program terminates.  
This library:
1) Uses [```mkstemp(char *template)```](https://linux.die.net/man/3/mkstemp) to create the temporary file,
2) Obtains the ```FILE *``` handle,
3) Removes the created temporary file,
4) Returns the handle to be used by the caller.

POSIX allows deleting open files.
Deleted file is still available as long as you still have the handle.
It cannot be opened again and is not shown in it's parent directory.  
Deleted file is actually deleted from disk and used space reclaimed when all the handles are closed.  
Program termination, including crashing the program, closes the handles owned by the process.

## Usage

#### Requirements
This library requires Android API level 16 (Jelly Bean 4.1.x) or newer.
Such a requirement is imposed by the current versions of Android Native Development Kit.

#### Example
Library provides a proof of concept [sample application](/sampleapp).

Loading tmpfile library is done in two steps:
* Including app/libs/tmpfile-android-1.1.4.aar as a dependency in Gradle.  
This step bundles Tmpfile.java and libtmpfile.so into your application's APK.
* Linking your native (C / C++) binaries against *tmpfile* library (libtmpfile.so).  
This step, in effect, redirects *tmpfile* function calls to libtmpfile.so

#### Dependency in Gradle
[sampleapp/app/build.gradle](sampleapp/app/build.gradle) contains code to load the library as a dependency in Gradle.
```gradle
dependencies {
    implementation 'com.viliussutkus89:tmpfile-android:1.1.4'
}
```

tmpfile-Android is distributed using [JCenter](https://jcenter.bintray.com) Maven repository.  
It needs be added to [top level build.gradle](sampleapp/build.gradle)
```gradle
allprojects {
    repositories {
        jcenter()
    }
}
```

Older versions of Gradle (or Android Gradle plugin?) require JCenter url.
```gradle
allprojects {
    repositories {
        maven() {
            url  "https://jcenter.bintray.com"
        }
    }
}
```

#### Linking native binaries against libtmpfile.so

It is not possible to link against objects inside .aar archives (as far as I know). libtmpfile.so needs to be extracted first.

As a workaround, sample application implements a Gradle task named *extractLibtmpfileSoForLinkingInCMake*, which is executed before *preBuild* task.
Task extractLibtmpfileSoForLinkingInCMake extracts native libraries (.so files) from tmpfile-android-1.1.4.aar into application's build directory, so that they could be linked against in CMake.
```gradle
// Extract shared .so libraries to build directory
// So that they could be linked against in CMake
task extractLibtmpfileSoForLinkingInCMake {
    doLast {
        def tmpfileandroid = configurations.releaseRuntimeClasspath.getResolvedConfiguration().getResolvedArtifacts().find {
            it.name.startsWith("tmpfile-android")
        }
        copy {
            from zipTree(tmpfileandroid.getFile())
            into "${project.buildDir}/tmpfile/"
            include "jni/**/libtmpfile.so"
        }
    }
}

preBuild.dependsOn extractLibtmpfileSoForLinkingInCMake
```

Once libtmpfile.so is extracted, it can be used to link your binaries against it.

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
