# **Void Promise**

## Naming Conventions

This document specifies naming conventions for any code written for the engine, games or related projects.
This is an initial draft and the rules to be here defined are subject to change. Please, log any modification at the bottom of this file.

### Files

* File name shall follow the class name;

### Classes

* Class names should be nouns in UpperCamelCase;
* StateIntro()
* EntityManager()

### Members

* Class members should be written in lowercase_separated_by_underscores;
* is_initialized;
* current_health;

### Methods

* Methods should be verbs in lowerCamelCase;
* drawSprite();
* getWidth();

### Others
* ?

## File Structure
  
```bash
|-- Caravans # Game code
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


### Modifications
* Diego Migotto - 01/09/18 - Created naming conventions
 
 

