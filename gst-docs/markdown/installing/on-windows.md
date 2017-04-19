# Installing on Windows

## Supported platforms

* Windows 7
* Windows 8
* Windows 8.1
* Windows 10

## Prerequisites

To develop applications using GStreamer for Windows we recommend using
[Windows 7](http://windows.microsoft.com/en-us/windows7/products/home)
or later. Windows Vista may also work but it is not supported.

GStreamer binaries includes C headers (`.h`) and library files (`.lib`)
valid for any version of [Microsoft Visual
Studio](http://www.microsoft.com/visualstudio). For convenience,
property pages (`.props`) are also included which extremely simplify
creating new projects. These property pages, though, only work with
[Microsoft Visual
Studio 2010](http://www.microsoft.com/visualstudio/en-us/products/2010-editions)
(including the free [Visual C++ Express
edition](http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-cpp-express)).

The recommended system is
[Windows 7](http://windows.microsoft.com/en-us/windows7/products/home)
with [Microsoft Visual
Studio 2010](http://www.microsoft.com/visualstudio/en-us/products/2010-editions) (Take
a look at its [system
requirements](http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-cpp-express)).

Installing GStreamer for 32-bits platforms requires approximately 286MB of
free disk space for the runtime and 207MB for the development files.

Installing GStreamer for 64-bits platforms requires approximately 340MB of
free disk space for the runtime and 216MB for the development files.

## Download and install GStreamer binaries

There are 3 sets of files in GStreamer binaries:

  - The runtime files are needed to run GStreamer applications. You
    probably want to distribute these files with your application (or
    the installer below).
  - The development files are **additional** files you need to create
    GStreamer applications.
  - The [Merge
    Modules](http://msdn.microsoft.com/en-us/library/windows/desktop/aa369820%28v=vs.85%29.aspx)
    files are **additional** files you can use to deploy GStreamer binaries
    alongside your application (see [Windows
    deployment](deploying/windows.md)).

Get  **the Runtime and Development files** installers appropriate for
your architecture from here:

[https://gstreamer.freedesktop.org/data/pkg/windows/](https://gstreamer.freedesktop.org/data/pkg/windows/)

Execute the installers and choose an installation folder. The suggested
default is usually OK.

> ![Warning](images/icons/emoticons/warning.png)
>`If you plan to use Visual Studio, **close it before installing GStreamer**. The installer will define new environment variables which will not be picked up by Visual Studio if it is open.

> On **Windows 8** and **Windows 10**, it might be necessary to log out and log back in to your account after the installation for the newly defined environment variables to be picked up by Visual Studio.

It is the application's responsibility to ensure that, at runtime,
GStreamer can access its libraries and plugins. It can be done by adding
`%GSTREAMER_ROOT_X86%\bin` to the `%PATH%` environment variable, or
by running the application from this same folder.

At runtime, GStreamer will look for its plugins in the following
folders:

  - `%HOMEDRIVE%%HOMEFOLDER%/.gstreamer-1.0/plugins`
  - `C:\gstreamer\1.0\x86\lib\gstreamer-1.0`
  - `<location of libgstreamer-1.0-0.dll>\..\lib\gstreamer-1.0`
  - `%GST_PLUGIN_PATH%`

So, typically, if your application can find `libgstreamer-1.0-0.dll`,
it will find the GStreamer plugins, as long as the installation folder
structure is unmodified. If you do change this structure in your
application, then you can use the `%GST_PLUGIN_PATH%` environment
variable to point GStreamer to its plugins. The plugins are initially
found at `%GSTREAMER_ROOT_X86%\lib\gstreamer-1.0`.

Additionally, if you want to prevent GStreamer from looking in all the
default folders listed above, you can set the
`%GST_PLUGIN_SYSTEM_PATH%` environment variable to point where the
plugins are located.

## Configure your development environment

### Building the tutorials


The tutorials code, along with project files and a solution file for
Visual Studio 2010, are in the
[gst-docs](https://cgit.freedesktop.org/gstreamer/gst-docs/) in the
`examples/tutorials` folder.

In order to prevent accidental modification of the original code, and to
make sure Visual Studio has the necessary permissions to write the
output files, copy the entire `tutorials` folder to a place of your
liking, and work from there.

> ![Information](images/icons/emoticons/information.png)
> **64-bit Users**
>
>Use `%GSTREAMER_ROOT_X86_64%` if you have installed GStreamer binaries for 64-bit platforms. Both GStreamer binariess (32 and 64-bit) can be installed simultaneously, and hence the separate environment variables.
>
>Make sure you select the Solution Configuration that matches GStreamer that you have installed: `Win32` for 32 bits or `x64` for 64 bits.
>
> ![Windows Install Configuration](images/WindowsInstall-Configuration.png)

You can fire up Visual Studio 2010 and load your copy of the
`tutorials.sln` solution file (Click on the screen shots to enlarge
them).

![](images/WindowsInstall2.png)

![](images/WindowsInstall1.png)

Hit **F7**, press the Build Solution button
![](images/WindowsInstall-BuildSolution.png) or go to Build →
Build Solution. All projects should build without problems.

### Running the tutorials

In order to run the tutorials, we will set the current working directory
to `%GSTREAMER_ROOT_X86%`\\`bin` in the Debugging section of the
project properties. **This property is not stored in the project files,
so you will need to manually add it to every tutorial you want to run
from within Visual Studio**. Right click on a project in the Solution
Explorer, Properties → Debugging → Working Directory, and type
`$(GSTREAMER_ROOT_X86)`\\`bin`

(The `$(...)` notation is required to access environment variables
from within Visual Studio. You use the `%...%` notation from Windows
Explorer)

You should now be able to run the tutorials.

### Creating new projects manually

**If you want to create 64-bit applications, remember also to create x64
Solution and Project configurations as
explained [here](http://msdn.microsoft.com/en-us/library/9yb4317s\(v=vs.100\).aspx).**

#### Include the necessary GStreamer Property Sheet

The included property sheets make creating new projects extremely easy.
In Visual Studio 2010 create a new project (Normally a `Win32
Console` or `Win32 Application`). Then go to the Property Manager
(View→Property Manager), right-click on your project and select “Add
Existing Property Sheet...”. Navigate to
`%GSTREAMER_ROOT_X86%`\\`share\vs\2010\libs` and
load `gstreamer-1.0.props `

This property sheet contains the directories where the headers and
libraries are located, and the necessary options for the compiler and
linker, so you do not need to change anything else in your project.

If you cannot find the Property Manager, you might need to enable Expert
Settings. Go to Tools → Settings → Expert Settings. Upon first
installation of Visual Studio, Expert Settings are disabled by
default.

![](images/WindowsInstall10.png)

> ![Warning](images/icons/emoticons/warning.png)
> **Depending on the GStreamer libraries you need to use, you will have to add more property pages, besides `gstreamer-1.0`**  (each property page corresponds to one GStreamer library).
>
> The tutorial's project files already contain all necessary property pages. When developing your own applications, the GStreamer documentation will tell you what library a function belongs to, and therefore, what property pages you need to add.

#### Remove the dependency with the Visual Studio runtime

At this point, you have a working environment, which you can test by
running the tutorials. However, there is a last step remaining.

Applications built with Visual C++ 2010 depend on the Visual C++ 2010
Runtime, which is a DLL that gets installed when you install Visual
Studio. If you were to distribute your application, you would need to
distribute this DLL with it (What is known as the [Visual C++ 2010
Redistributable
Package](http://www.microsoft.com/download/en/details.aspx?id=5555)).
This happens with every version of Visual Studio, and the Runtime DLL is
different for every version of Visual Studio.

Furthermore, GStreamer itself is built using a “basic” C runtime which
comes in every Windows system since Windows XP, and is named
`MSVCRT.DLL`. If your application and GStreamer do not use the same C
Runtime, problems are bound to crop out.

In order to avoid these issues you must instruct your application to use
the system's C Runtime. First install the [Windows Device Driver Kit
Version 7.1.0](http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=11800) (DDK).
When the installer asks about the features, select only “Build
Environments”. Accept the suggested location for the installation, which
is usually `C:\WinDDK\7600.16385.1`. This download is an ISO file, you
can either burn a DVD with it (as recommended in the Microsoft site. You
will need DVD burning software), mount the file in a virtual DVD device
(you will need DVD virtualization software) or unpack the file as if it
was a regular compressed file (you will need decompression software that
understands the ISO format).

Then, add the `x86.props` or `x86_64.props` (for 32 or 64 bits) property
sheet found in `%GSTREAMER_ROOT_X86%``\``share\vs\2010\msvc` to your
project. This will make your application use the ubiquitous
`MSVCRT.DLL` saving you some troubles in the future.

> ![Information](images/icons/emoticons/information.png)
> If you did not install the WinDDK to the standard path `C:\WinDDK\7600.16385.1`, you will need to tell Visual Studio where it is. Unfortunately, there is no automated way to do this. Once you have added the `x86.props` or `x86_64.props` to your project, go to the Property Manager, expand your project and its subfolders until you find the property sheet called `config`. Double click to edit it, and select the section called “User Macros” in the list on the left. You should see a macro called `WINDOWS_DRIVER_KIT`. Double click to edit it, and set its value to the root folder where you installed the DDK. This is the folder containing a file called `samples.txt`.
>
>That's it. Accept the changes, right click on the `config` property sheet and select “Save”. The path to the DDK is now stored in `config.props` and you do not need to perform this operation anymore.

### Creating new projects using the wizard

Go to File → New → Project… and you should find a template
named **GStreamer Project**. It takes no parameters, and sets all
necessary project settings, both for 32 and 64 bits architectures.

The generated project file includes the two required Property Sheets
described in the previous section, so, in order to link to the correct
`MSVCRT.DLL`, **you still need to install the Windows Device Driver
Kit** and change the appropriate property sheets.
