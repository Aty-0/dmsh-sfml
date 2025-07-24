# Todo list

# to the far future 
- [ ] save patterns
- [ ] implement enemy movement
- [ ] buttons, window
- [ ] add splines for patterns
- [ ] investigate about sf::config  
 
## 23-24/07/2025
- [x] coroutine
- [x] improved editor a bit
- [x] implement pattern class 
    - [x] add vector of patterns in editor 
    - [x] we can change patterns 
- [x] add onInputUpdate for game objects and components 
    - [x] add isListenerActive(name)
    That method is more correct and convenient than we are have now 

## 21/07/2025
- [x] onMouseUnselect 
- [-] proper names for (onMouseSelect | onMouseUnselect)  
- [x] improve editor ui a bit 
    - [x] transperency, change position 
- [x] (collsion bug) if something already colliding with object, that's cause that other objects is not triggering enter callback
        - [x] fixed same bug but for mouse 
        - [x] remove collider tracker for mouse, because it's too complicated for mouse 
        - I think, to we need to store collided object in collider to fix this bug



## Game engine 
- [x] z depth
- [x] mouse picking
- [x] tags
- [x] debug draw
- [ ] rename getDrawable in Drawable it's weird
- [ ] mouse hover
- [ ] ray trace
- [ ] (game object) visible variable 
- [ ] (game object) name, uuid 
- [ ] (components) name, uuid 
- [ ] easy transform animations (like in osu!)
- [ ] camera
- [ ] improve-rewrite logger 
    - [ ] save logs 
    - [ ] use std::format    
- [ ] colliders 
    - [x] callback on collision 
    - [ ] push objects on collision if it's dynamic 

- [ ] input
    - [x] listener logic
    - [x] support for keyboard 
    - [x] support for mouse 
    - [ ] support for joystick
    - [x] add hold key
    

    - [ ] isKeyDown, isKeyReleased for input
- [ ] ui
    - [x] text 
        - [x] also will be nice add std::format for setting text 
    - [ ] button 
    - [ ] square  
- [ ] scene / game objects
    - [ ] create go, delete go
        - [x] create
        - [ ] add
        - [ ] delete 
    - [ ] add for game objects uuid and name
- [ ] basic editor 
    - [ ] scene inspector
    - [ ] select go 
    - [ ] show go stats 
    - [ ] move go (by mouse, by text box)

## Game
    - [ ] player
        - [ ] health, bombs
        - [ ] shoot
            - [ ] pool bullets
        - [x] fix moving        
    - [ ] stage system 
        - [ ] editor for stage system 
        - [ ] choice json or xml
        - etc...              

    - [ ] ui
        - [ ] pause 
        - [ ] main menu scene 
