#SqueLib


Thanks for taking a look at ***SqueLib***, an end of degree project built to help building **C++** multiplatform applications on Windows, Linux and Android easily and you having to write _only_  **C++** and a touch of **CMake** (and whatever else you decide to add)

For that reason it will be fully available for anyone to use and fork, although I will be the sole maintainer of this project as it will be my own testing grounds for anything I want to learn along the dying language of **C++** (maybe someday this will be ported or remade **Rust**).

But why did I really decided on this project, I hate going into IDE's. There are too many steps in configuring the projects and doing things. I prefer to get all things together, build and then just use any available IDE to debug what's happening, which **CMake** is great at. 

The project has been debugged using MSVS, GDB and Android Studio through APK Debugging. Both GDB and Android Studio have been a learning experience by themselves on just getting them to work coming from only ever having used MSVS, the final documentation will include a setup and the most basic debugging (where I am) going.

What does SqueLib provide?
===

**Easy C++ Android:**</br>
As long as you are using up to C++17 that matches **[LLVM's LibC++](https://libcxx.llvm.org/)** support, you can build with it and get an android app easily.
And ImGui support on **Android** is included with ***SqueLib***

**Easy Logging:**</br>
Maybe not that fast and memory efficient, but very practical log management that retrieves where it is called (very practical if you are a mad log-debugger like me).

**Input Management:**</br>
Because the library wraps over both _GLFW_ and the _Android NDK_, input is a bit jumbled with Display Management, for now, it si required the use of them together.
You can setup callback to any key, mouse event and even default gestures provided!

**Display Management:**</br>
Althought currently not supporting multiple displays on _Android_, _SqueLib_ wraps over _GLFW_ to provide an easier experience that will not require a whole rewrite on _Android_ when trying to use multiple windows.

**OpenGL'esque rendering:**</br>
Crossplatform wrapping of _OpenGL_ (up to latest Core without extensions) and _OpenGLES_ 3.2, some simple types are provided that makes it easier to get things going. I focus more on first declaring and then performing rather than having to setup each time verbosely like plain _OpenGL_

A long term interest is having support for other APIs to learn how they work and how to have a more comforming code, but that will have to wait until this library proves useful by itself.

**Easy file management:**</br>
An easy way to load and write files and assets for the provided platforms.

**Fast Precision Timer:**</br>
Using _chrono_, a simple and efficient timer is provided.

What does SqueLib lack?
===
**Cross-Compiling**</br>
One of the intentions of ***SqueLib*** is to make things easier to have things running quickly on the supported platforms. For lack of experience, I have not been able to setup cross-compilation _Linux<->Windows_ or _Windows->Android_.

**Examples and Use-cases**</br>
Althought the base is done, it is not properly tested beside making sure things work as intended. As the project advances, more examples will popup, each with its own repo (besides the *All_Example*).

**Performance**</br>
I am sure it is not *good* as a standalone project, the idea was never to have it be *the best* but to actually get it to *work* which it does*.
<sub><sub>*even with all the faults that have not been found</sub></sub>

**Documentation**</br>
WIP

**Continuous Build / Testing**</br>
Have never read or got into it, please forward me any good introduction on those and I can implement it as part of the project as well! 

Setup
===
##Dependencies

Although most dependencies are pulled by the building process (GLFW, GLMath,...) there are some platform specific dependencies that require specific setup, besides **CMAKE** which is a given.

###Linux
- libx11, libxrandr,  libxinerama, libxcursor, libxi
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ bash
   Pull them from the distro package manager.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

###Android (from Linux)
 - Android NDK
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   1. Download Android Studio
   2. Untar it into a folder, go to AndroidStudio -> bin
   3. Run studio.sh
   4. Default options work well unless you want different install paths
   5. Go to Configure -> SDK Manager
   6. Select the Android versions you want/can test (For me its 29)
   7. Go tot SDK Tools and select: Android NDK and Android SDK Command-line Tools
   8. Accepth the T&A and after downloading you’re done

   OR

   Download from the Google Developer site
   ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Building With SqueLib
===

You can basically copy the style of any example found, but the main parts will be the following:

***In your CMakeLists.txt***
 - Add SqueLib as a subdirectory, specifying any Binary Directory:
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    add_subdirectory(${path_to_SqueLib} ${paht_to_binary_dir})
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 - Setup the install targets process
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SqueLib_PrepareBuld(target org_name ${source_files})
    SqueLib_Package(asset_folder resource_folder)
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
***In the Command Line***
 - Build with cmake adding the target platform - command line
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    cmake <dir> ( -DToWindows=1 || -DToLinux=1 || -DToAndroid ) -DCMAKE_BUILD_TYPE=(Release||Debug)
    cmake --build <dir>
    # Optional for Android, install into android device from Linux only... 
    make uninstall push # This will remove the app if exists, create the apk and install on device
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 - You can also check/use the examples/all command-line executables (FromLinux_BuildAll.sh / FromWin_BuildAll.batch) to have an even easier build experience 
For getting into building with CMake, I strongly recommend **[Jeff Preshing's](https://preshing.com/)** **[introduction](https://preshing.com/20170522/learn-cmakes-scripting-language-in-15-minutes/)**.
There is a lot of info available and the documentation is extensive, but this carried me heavily starting out. The posts on plywood and arch80 engine have been also really interesting!

Resources
===
When packaging apps and building, you'd want to have your data properly packaged together. Until ***SqueLib*** supports proper packaging , currently the only option is to have assets and files under separate folders for _AppAssets_ and _AppResources_, when building it will be packaged directly as long as the folders are provided on the SqueLib_Package() call.

For android it is required to have a folder _AppResources/icon/mipmap.png_ which will be used as the app icon when installing. When things get more advance, it will be usign this folder to setup also desktop icons and other data following the **Android** structure. If things change in the future, it will be probably when adding support for other platforms.

In any case, you can go through the code and change it however you want to, have as much fun as I did!

Debugging
===
The step of going from Windows to Linux and Linux to Android, have been as difficult as expected at first. As ***SqueLib*** provides what it can platform agnostically, you can debug your code that uses ***SqueLib*** (and no platform dependant extra code) on your preferred IDE, or that's what I would like to say.

Currently I have only tried and get to work on **GDB** and **Android Studio**, besides MSVS which is just easy.

License
===
Copyright 2021 Marc Torres Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Acknowledgements
===
 - **[Raysan5](https://github.com/raysan5/)** for a short but deep introductory talk on libraries and about write more agnostic and reusable code, and obviously for **[_RayLib_](https://www.raylib.com/)** as a whole.
 - **[CNLohr](https://github.com/cnlohr)** for **[_RawDraw_](https://github.com/cntools/rawdraw/tree/27f05afe1747e3dc7a5dd02eaf2b761ef3624762)** and specifically **[_RawDrawAndroid_](https://github.com/cnlohr/rawdrawandroid)**,.
 - **[Morgan McGuire](https://casual-effects.com/)** for providing **[_Markdeep_](https://casual-effects.com/markdeep)** in an open source way, writing this readme has been a pleasure.

 The full project documentation and bibliography will be available as the project documentation nears its completion.

 Contact
 ===

[<i class="fab fa-discord"></i> MarcTorresDev#0001](https://discord.com)
[<i class="fab fa-twitter-square"></i> MarcTorres_Dev](https://twitter.com/MarcTorres_Dev)
[mtorres.form@gmail.com](https://mail.google.com/mail/?view=cm&fs=1&to=mtorres.form@gmail.com)

