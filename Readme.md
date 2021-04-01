# SqueLib


Thanks for taking a look at ***SqueLib***, an undergraduate thesis project oriented to help building **C++** multiplatform graphic applications on Windows, Linux and Android easily, by only writing **C++** and a small bit of **CMake** (and whatever else you decide to add).

For that reason it will be fully available for anyone to use and fork, although I will be the sole maintainer of this project as it will be my own testing grounds for anything I want to learn along the dying language of **C++** (maybe someday this will be ported or remade **Rust**).

But why did I really decided on this project, I hate going into IDE's. There are too many steps in configuring the projects and doing things. I prefer to get all things together, build and then just use any available IDE to debug what's happening, which **CMake** is great at. 

The project has been debugged using MSVS, GDB and Android Studio through APK Debugging. Both GDB and Android Studio have been a learning experience by themselves on just getting them to work coming from only ever having used MSVS, the final documentation will include a setup and the most basic debugging (where I am) going.

What does SqueLib provide?
===

**Easy C++ Android:**</br>
As long as you are using up to C++17 that matches **[LLVM's LibC++](https://libcxx.llvm.org/)** support, you can build with it and get an android app easily.
ImGui support on **Android** is included with ***SqueLib***.

**Easy Logging:**</br>
Maybe not that fast and memory efficient, but very practical log management that retrieves where it is called (very practical if you are a mad log-debugger like me).

**Input Management:**</br>
Because the library wraps over both _GLFW_ and the _Android NDK_, input is a bit jumbled with Display Management, for now, it is required the use of them together.
You can setup callback to any key, mouse event and even default gestures provided!

**Display Management:**</br>
Controlling the main window on desktop platforms (sorry _Android_), through GLFW. The code base is prepared to go into multiple windows but that still might be a long way away.

**OpenGL'esque rendering:**</br>
Crossplatform wrapping of _OpenGL_ (up to latest Core without extensions) and _OpenGLES_ 3.2, some simple types are provided that makes it easier to get things going. The focus is towards first declaring and then performing rather than having to setup each time verbosely like plain _OpenGL_.

A long term interest is having support for other APIs to learn how they work and how to have a more comforming code, but that will have to wait until this library proves useful by itself.

**Easy file management:**</br>
An easy way to load and write files and assets for the provided platforms.

**Fast Precision Timer:**</br>
Simple, accurate and efficient timer using _<chrono>_.

What does SqueLib lack?
===
**Cross-Compiling**</br>
One of the intentions of ***SqueLib*** is to make things easier to have things running quickly on the supported platforms. For lack of experience, I have not been able to setup cross-compilation _Linux<->Windows_ or _Windows->Android_.
Through WSL or [GWSL](https://opticos.github.io/gwsl/) it is probable that you could compile _Windows->Android_, although untested it looks promising

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
## Dependencies

Although most dependencies are pulled by the building process (GLFW, GLMath,...) there are some platform specific dependencies that require specific setup, besides **CMAKE** which is a given.

### Linux
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ bash
   - libx11, lixrandr, libxinerama, libxcursor, libxi
   Pull them from the distro package manager.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Android (from Linux)
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   1. Download Android Studio
   2. Extract it into a folder, go to AndroidStudio -> bin
   3. Run studio.sh
   4. Default options work well unless you want different install paths
   5. Go to Configure -> SDK Manager
   6. Select the Android versions you want/can test, according to GLES3.2 versioning 24 is the minimum supported.
   7. Go tot SDK Tools and select: Android NDK and Android SDK Command-line Tools
   8. Accepth the T&A and after downloading you’re done

   OR

   Download from the Google Developer site
   ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Creating first app and a basic loop
===
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~C++
static bool on_background = false;
void OnGoBackground()
{
   on_background = true;
}
void OnResume()
{
   on_background = false;
}

int main()
{
   SQUELIB_Init("application_name");
   SQUE_AddOnGoBackgroundCallback(OnGoBackground);
   SQUE_AddOnResumeCallback(OnResume);

   while(!SQUE_DISPLAY_ShouldWindowClose())
   {
      SQUE_INPUT_Process();
      SQUE_RENDER_Clear(ColorRGBA(1.f,1.f,1.f,1.f);
      if(on_background)
      {
         SQUE_Sleep(100);
         continue;
      }
      
      SQUE_DISPLAY_SwapAllBuffers();
   }

   SQUELIB_Close();
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Further examples can be found under the Examples folder or under Examples Chapter in Documentation.

Building With SqueLib
===

You can basically copy the style of any example found, but the main parts will be the following:

***In your CMakeLists.txt***

 - Add SqueLib as a subdirectory, specifying any Binary Directory:
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    add_subdirectory(${path_to_SqueLib} ${path_to_binary_dir})
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 - Setup the install targets process
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SqueLib_PrepareBuld(target organization_name ${source_files})
    SqueLib_Package(asset_folder resource_folder)
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
***In the Command Line***
 - Build with cmake adding the target platform - command line
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    cmake <dir> ( -DToWindows=1 OR -DToLinux=1 OR -DToAndroid=1 ) -DCMAKE_BUILD_TYPE=(Release OR Debug)
    cmake --build <dir>
    # Optional for Android, install into android device from Linux only... 
    make uninstall push # This will remove the app if exists, create the apk and install on device
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 - You can also check/use the examples/all command-line executables (FromLinux_BuildAll.sh / FromWin_BuildAll.batch) to have an even easier build experience 
For getting into building with CMake, I strongly recommend **[Jeff Preshing's introduction](https://preshing.com/20170522/learn-cmakes-scripting-language-in-15-minutes/)**.
There is a lot of info available and the documentation is extensive, but this carried me heavily starting out. The posts on plywood and arch80 engine have been also really interesting!

Resources
===
When packaging apps and building, you'd want to have your data properly packaged together. Until ***SqueLib*** supports proper packaging , currently the only option is to have assets and files under separate folders for _AppAssets_ and _AppResources_, when building it will be packaged directly as long as the folders are provided on the SqueLib_Package() call.

For android it is required to have a folder _AppResources/icon/mipmap.png_ which will be used as the app icon when installing. On desktop platofmrs, the options for using a window icon is also provided through Display part of the library, but not yet part of the main init.

In any case, you can go through the code and change it however you want to, have as much fun as I am having!

Debugging
===
The step of going from Windows to Linux and Linux to Android, have been as difficult as expected at first. As ***SqueLib*** provides what it can platform agnostically, you can debug your code that uses ***SqueLib*** (and no platform dependant extra code) on your preferred IDE, or that's what I would like to say.

Currently I have only tried and get to work on **GDB** and **Android Studio**, besides MSVS which just plain works. None of these introduction steps are great tutorials but will get you going fast.

## GDB
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   1 - Open your console
   2 - gdb <path_to/executable>
   3 - layout next
   4 - b 1 # Put a breakpoint at first available point of execution (or wherever you need in your main)
   5 - Run

   Yep, that is how I have been debugging and going to lines of crashes which is shown.
   n -> next line
   s -> step in
   c -> continue

   For more commands refer to a proper tutorial or the actual [documentation](https://www.gnu.org/software/gdb/documentation/)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Android Studio
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   1 - Compile and install your app on your device
   2 - Open Android Studio
   3 - File -> Profile or Debug APK
   4 - Go to Project -> cpp: There will appear the necessary debug library binaries
   4.1 - At firs they will all be missing, so double clean on each .so file and add the path
         The files will be under ./app_dir/build/makecapk/lib/arm64-v8a
   5 - Click on green ladybug utton at the top right (or shift+F9)
   6 - Wait to load and you are debugging
   7 - If you want to look at specific code, drag and drop used files and drop breakpoints when needed
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Documentation]("./doc_index.md.html")
===
All pages are still being worked on, including the thesis which will be completed by the 24th of June 2021.
 
 * [Documentation Index]() 
 * [Examples]("./doc_index.md.html") 

 * [Thesis]("./SqueLib-Thesis/SqueLib_Bachelor_Thesis_Marc_Torres_Jimenez.md.html")
 

License
===
Copyright 2021 Marc Torres Jimenez

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
