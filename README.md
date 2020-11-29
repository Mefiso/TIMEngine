# TIME
## Description
Custom video game engine that tries to mimic Unity and the likes features. It is developed mostly for educational purposes and it will be used for developing and delivering several assignments for the Master's Degree in [Advanced Programming for AAA Vide Games](https://www.talent.upc.edu/ing/estudis/formacio/curs/201200/master-degree-advanced-programming-aaa-videogames/) (UPC School).

All the code and the releases can be found in the [github page](https://github.com/Mefiso/TIMEngine). As mentioned below, the code source code for the first assignment is in main and release/first-assignment branches.

## Name
The name of this engine is **TIME** (_/taɪm/_) a.k.a **TIMEngine** (_/taɪm ˈɛnʤɪn/_). It is an acronym for The Incredible Mefiso's Engine. But more information may be revealed by the console inside the application.

## How to use it
The main window will be filled with the rendered scene. A main menu is displayed at the top, with several options, such as quit, deactivate and activate UI windows and "about information". To close the application it can be used the aforementioned "Quit" button on the menu or the cross at the top right. 

The main window is resizable, but resizing and other main window actions are preferably done in the "Window" header inside the **"Configuration"** UI window.
In this **Configuration** window other actions for every core module in the engine:
- Inside **Window**: brightness, width and height modifications, enable or disable vsync, fullscreen and borders.
- Inside **Input** only mouse position and mouse and keyboard detection are displayed.
- Inside **Renderer**: Enable/Disable depth testing and face culling, change background color, and draw or not draw the debug grid.
- Inside **Texture**: Enable/Disable mipmaps and forced image flip when loading an image as a texture, and modify the filter and wrap parameters for the current loaded textures.
- Inside **Camera**: Move the camera, modify movement parameters (like speed), modify frustum parameters (planes, VFOV, etc.).

Apart from the described above, the **console** window also allows some actions: clear its contents, copy, filter words, and execute commands (very few commands implemented at the moment).

As far as the scene are concerned, **drag&drop** feature for both models and textures is implemented. Droping an _FBX_ model will load it and replace the current geometry. While dropping an image file will load it as a texture for the current model.

Finally, the Engine features Unity-like camera controls:
1. “WASD” fps-like movement and arrows free look around. 
2. While Right clicking, free look around.
3. Mouse wheel zooms in and out.
4. Alt+Left click orbits the object.
5. Pressing “f” focuses the camera around the geometry.
6. Holding SHIFT duplicates movement speed.

Of course docking/resizing/moving the GUI windows can be performed freely.

## First assignment
The first assignment delivery can be found on the release section, the Engine has all the functionalities specified on the requirements section of the assignment.

The code source code for the first assignment is in main and release/first-assignment branches.

_Note that the fox texture is a DDS file but the Totoro one is not due to the difficulty of finding models with DDS texture attached. Since the fox can be perfectly loaded is already proven that the specified requirement is met. You can also drag and drop DDS textures without any inconvenient._

## Author
Name: **Pol Pérez Granero**

Github username: [**Mefiso**](https://github.com/Mefiso)
