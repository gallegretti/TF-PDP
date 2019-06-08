# **Guilds: Frontiers**


### Screenshots
![](/screenshots/01.png)
![](/screenshots/02.png)

 
### Building

* Clone this repo into the root of your drive. Windows dosen't like long paths

### Windows
* Download Visual Studio Community Edition 
* Open the Game solution
* Run the game with the 'Local Windows Debugger'
    * When running in the debugger mode, the game will use the /Dependencies as the root folder

### Android
* Download Android studio
* Download [Android ndk r16b](https://developer.android.com/ndk/downloads/)
* Extract to a folder and make Android Studio that directory (File -> Project Structure -> Android NDK Location)
* [Download](https://drive.google.com/open?id=1uqE6Eg3KzDvaYA2SKkxdvycg8WD2jrxo) SFML libs, and extract into ndk root/sources/third_party [Or build from source] (https://github.com/SFML/SFML/wiki/Tutorial:-Building-SFML-for-Android)
* Open the project with Android Studio
* Install everything else that Android Studio requests
* To run on the emulator, select the x86 build variant
* If the debugger dosen't work, go to Run -> Edit Configurations -> Debugger and set Debug Type to Native

### Developing

* Make sure to read and follow as closely as possible the [coding guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md)  
* Run the Static analysis from time to time: On visual studio, do Analyze -> Run Code Analysis -> On Solution
* If something seems inexplicably broken, try to do a Rebuild

## Maps

* Maps are made with a [custom tool](http://www.mapeditor.org/)
* Supported version: 1.2x
* Multiple layers are supported
* Layer offset is supported
* Layer encoding is not supported
* 'Infinite' maps are not supported

### Creating a custom map
* Open Tiled
* File -> New -> New map
* Set orientation as Ortoghonal
* Set Tile layer format as CSV
* Set Tile render order as Right Down
* Map size should be fixed
* Save as .json

### Loading tilesets

* File -> New -> New Tileset
* Check 'Embed in the map'

### Using in-game
* Move the map and every used tileset file to the folder /Assets/Maps

#### Project structure
  
```bash
|-- src # Game code
|    |-- Actions       # Actions that can be done to an entity  
|    |-- BuildScripts  # Self explanatory   
|    |-- Components    # Components for entity assembly   
|    |-- Effects       # Effects that an entity can do when triggered  
|    |-- Entities      # Entity definitions   
|    |-- Game          # Game logic    
|    |-- Interface     # Common interface components   
|    |-- Map           # Game map   
|    |-- Mesurement    # Auxiliar classes for common mesurements   
|    |-- Mutations     # Mutations for entities    
|    |-- States        # Aplication states
|          |-- StateGameLoop # Game running state
|    |-- Systems       # Game systens   
|    |-- ThirdParty    # Dependencies     
|          |-- [SFML](https://www.sfml-dev.org/)  # Simple Multiplataform Multimedia Library  
|          |-- [easylogging](https://github.com/muflihun/easyloggingpp) # Logging  
|          |-- [json](https://github.com/nlohmann/json) # JSON serialization/deserialization  
|    |-- Utils         # Self explanatory       
|-- Dependencies # Executable dependencies   
```


### Credits
* Diego Dasso Migotto 
* Fernando Bock 
* Gabriel Allegretti 
* Gustavo Einsfeldt  

### Extra Assets
 
[Game-icons.net for game icons](http://game-icons.net/) 
 
[Kenney.nl for UI sounds](http://www.kenney.nl/) 
 
[GUI construction kit by Lamoot for interface](http://opengameart.org/content/rpg-gui-construction-kit-v10) 
 
 

