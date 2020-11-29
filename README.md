# TIME
## Description
Custom video game engine that tries to mimic Unity and the likes features. It is developed mostly for educational purposes and it will be used for the developing and delivering several assignments for the Master's Degree in [Advanced Programming for AAA Vide Games](https://www.talent.upc.edu/ing/estudis/formacio/curs/201200/master-degree-advanced-programming-aaa-videogames/) (UPC School).

All the code and the releases can be found in the [github page](https://github.com/Mefiso/TIMEngine).

## Name
The name of this engine is **TIME** (_/taɪm/_) a.k.a **TIMEngine** (_/taɪm ˈɛnʤɪn/_). It is an acronym for The Incredible Mefiso's Engine. But more information may be revealed by the console inside the application.

## How to use it
The main window will be filled with the rendered scene. A main menu is displayed at the top, with several options, such as quit, deactivate and activate UI windows and "about information". To close the application it can used the aforementioned "Quit" button on the menu or the cross at the top right. 

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
a. “WASD” fps-like movement and arrows free look around. 
b. While Right clicking, free look around.
b. Mouse wheel zooms in and out.
c. Alt+Left click orbits the object.
d. Pressing “f” focuses the camera around the geometry.
e. Holding SHIFT duplicates movement speed.
