# pyinstaller_pixbuf_and_path_issue

(1) Download and install MSYS2 from "https://www.msys2.org/".

(2) This will install both MSYS2 and MinGW to the msys2 directory on your local disk. Go into the msys2 folder and
open a MinGW64 shell as administrator.    

(3) In the MinGW 64-bit shell, type in the following commands to install the required packages using the Pacman package manager:

    pacman -Syu		(NOTE: You will have to perform this call and close the shell several times for it to complete!)
    pacman -S mingw-w64-x86_64-python-pip
    pacman -S mingw-w64-x86_64-python-matplotlib
    pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-python3-gobject
    pacman -S git make nano pkg-config gcc

(4) Clone the GitHub repository with the example showing the error in question:

    git clone https://github.com/LeighKorbel/pyinstaller_pixbuf_and_path_issue.git
    cd pyinstaller_pixbuf_and_path_issue

(5) Download PyInstaller using Python's pip package manager:

    pip install pyinstaller

(6) Set the following environment variable:

    export PKG_CONFIG_PATH="/mingw64/lib/pkgconfig:$PKG_CONFIG_PATH"

(7) Now we are ready to start compiling. The makefile will freeze the python script using PyInstaller and compile the C GTK
    program that will execute the python script. Run:

    make

(8) After PyInstaller finishes, an executable file will now be located in "/dist/plot". This can be run from the 
    command line by:

    ./dist/plot/plot.exe

(9) We can see this doesn't work because some gdk-pixbuf loaders are missing.

    Gtk:ERROR:../gtk+-3.24.10/gtk/gtkiconhelper.c:494:ensure_surface_for_gicon: assertion failed (error == NULL): Failed
    to load C:\msys64\home\Cytocybernetics\pyinstaller_pixbuf_and_path_issue\dist\plot/share\icons\Adwaita\24x24/status\
    image-missing.png: Unable to load image-loading module: C:\msys64\home\Cytocybernetics\pyinstaller_pixbuf_and_path_issue\
    dist\plot\lib\gdk-pixbuf\loaders\libpixbufloader-png.dll: 'C:\msys64\home\Cytocybernetics\pyinstaller_pixbuf_and_path_issue\
    dist\plot\lib\gdk-pixbuf\loaders\libpixbufloader-png.dll': The specified module could not be found. (gdk-pixbuf-error-quark, 5)

    I have several GTK applications that I compile in Windows and this doesn't always happen, so I have the 'loaders'
    directory that is created by PyInstaller on the successful compiles. That being said, WHY DOES THIS HAPPEN AND HOW
    DO I PREVENT IT FROM OCCURRING? This missing 'loaders' directory is located in 'resources' and can be copied to '/dist/plot/lib/gdk-pixbuf'
    to get this to work.

(10) Now that the 'pixbuf' folder has been added, run the plot executable line from (8) again. We can see that it runs and
     that the icons for the matplotlib navigation toolbar all exist (bottom left corner). Now that we have verified that this works,
     we want to compile a very basic GTK application in C that only has a button that will spawn the frozen script using GLib's
     g_spawn() function. Run the following command:

     ./launch.exe

     Now click the button to launch the python script. The script launches and everything looks fine. However, now if you
     simply double click the 'launch.exe' from Windows File Explorer, then click the button, you will see that there are 
     now broken link icons for all of the buttons in the matplotlib navigation toolbar. Since this works in the terminal but
     not outside of it, it makes me think that there is a path issue here (which probably does not have anything to do with
     PyInstaller). The icons for the navigation toolbar are located in:

     /dist/plot/matplotlib/mpl-data/images

     All of the '*-symbolic.svg' images are what is being used, since removing those cause the broken links to occur in the
     terminal as well. WHY DOES THIS HAPPEN AND HOW DO I PREVENT IT FROM OCCURRING? Any clues regarding solving one or both
     of these problems would be greatly appreciated.
