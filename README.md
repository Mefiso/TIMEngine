# TIME
## Description
Custom VideoGame engine that tries to mimic Unity and the likes features. It is developed mostly for educational purposes and it will be used for developing and delivering several assignments for the Master's Degree in [Advanced Programming for AAA Vide Games](https://www.talent.upc.edu/ing/estudis/formacio/curs/201200/master-degree-advanced-programming-aaa-videogames/) (UPC School).

All the code and the releases can be found in the [github page](https://github.com/Mefiso/TIMEngine). As mentioned below, the code source code for the  and second assignments are in main and release/first-assignment branches.

## Name
The name of this engine is **TIME** (_/taɪm/_) a.k.a **TIMEngine** (_/taɪm ˈɛnʤɪn/_). It is an acronym for The Incredible Mefiso's Engine. But more information _may_ be revealed by the console inside the application.

## How to use it
The main window will be filled with the rendered scene. A main menu is displayed at the top, with several options, such as quit, deactivate and activate UI windows and "about information". To close the application it can be used the aforementioned "Quit" button on the menu or the cross at the top right. In the Menu most of the buttons inside File section are dummies, but **Save** and **Open** are actually saving and loading the scene in a json (for the moment it only saves to a specific filename and loads from that one only).

The main window is resizable, but resizing and other main window actions are preferably done in the "Window" header inside the **"Configuration"** UI window.
In this **Configuration** window other actions for every core module in the engine:
- Inside **Window**: brightness, width and height modifications, enable or disable vsync, fullscreen and borders.
- Inside **Renderer**: Enable/Disable depth testing and face culling, change background color, and draw or not draw the debug grid, bounding boxes and octree.
- Inside **Camera**: Move the camera, modify movement parameters (like speed), modify frustum parameters (planes, VFOV, etc.).

Apart from the described above, the **console** window also allows some actions: clear its contents, copy, filter words, and execute commands (very few commands implemented at the moment).

The Engine features Unity-like camera controls:
1. “WASD” fps-like movement and arrows free look around. 
2. While Right clicking, free look around.
3. Mouse wheel zooms in and out.
4. Alt+Left click orbits the object.
5. Pressing “f” focuses the camera around the geometry.
6. Holding SHIFT duplicates movement speed.

As far as the scene is concerned, **drag&drop** feature for models is implemented. Droping an _FBX_ model will load it and import it in our **Custom file format**.
As mentioned above, the scene can be serialized in a json file. And it can also be edited manually, that is, apart from being able to move objects around, it is also possible to create and empty game object and add components to it. In the specific case of mesh and material components, a **dialog window will be opened to select the appropriate custom file containing the data**. The material component can also be modified (one can add or delete textures attached to it).

Of course docking/resizing/moving the GUI windows can be performed freely.

#### Second Assignment Features
To explicitly specify the features available in the second assigment release:

1. Load and Save scenes, as well as saving models and textures in our custom file format.
2. Add mesh and material components, data loaded from custom file formats.
3. The active camera performs Frustum Culling accelerated with an Octree structure.
  - A camera can be selected to be the one performing the culling without being the active (in order to see if the culling is done correctly).
4. The scene contains editable Gizmos that are displayed when an object is selected, via inspector or via Mouse Picking, to move the GameObjects around.
5. Shading algorithm handles models with diffuse and specular maps, and several Lights in ths scene, including Directional, Point and Spot Lights, with BDRF Phong algorithms.
5. FPS control via VSYNC and Delays.
6. Play, Stop, and Pause buttons that save and load scene states and control the game clock. A time scale editor remains to be added.
7. Skybox.

## First assignment
The first assignment delivery can be found on the release section, the Engine has all the functionalities specified on the requirements section of the assignment.

The code source code for the first assignment is in main and release/first-assignment branches.

_Note that the fox texture is a DDS file but the Totoro one is not due to the difficulty of finding models with DDS texture attached. Since the fox can be perfectly loaded is already proven that the specified requirement is met. You can also drag and drop DDS textures without any inconvenient._

## Second assignment
The second assignment delivery can be found on the release section, the Engine has all the functionalities specified on the requirements section of the assignment. **It must be mentioned that the scene for the assigment is purposedly not loaded when the Engine starts. It can be loaded using the _Open_ button in the _File_ menu**. Unfortunately, due to a bad time management by the authors, the scene was not able to be correctly constructed in time; so it can be completed manually importing the remaining models and loading the missing textures.

## Authors
Name: **Pol Pérez Granero**

Github username: [**Mefiso**](https://github.com/Mefiso)

Name: **Francesc Porta Solsona**

Github username: [**FrancPS**](https://github.com/FrancPS)
