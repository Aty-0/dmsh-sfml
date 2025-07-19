# Todo list

## Game engine 
- [x] z depth
- [x] mouse picking
- [x] tags
- [ ] debug draw
- [ ] rename getDrawable in Drawable it's weird
- [ ] mouse hover
- [ ] ray trace
- [ ] (game object) visible variable 
- [ ] (game object) name, uuid 
- [ ] (components) name, uuid 
- [ ] easy transform animations (like in osu!)
- [ ] camera 
- [ ] colliders 
    - [x] callback on collision 
    - [ ] push objects on collision if it's dynamic 
    - [ ] (collsion bug) if something already colliding with object, that's cause that other objects is not triggering enter callback
- [ ] input
    - [x] listener logic
    - [x] support for keyboard 
    - [x] support for mouse 
    - [ ] support for joystick
    - [x] add hold key 
    - [ ] isKeyDown, isKeyReleased for input
- [ ] ui
    - [ ] text 
        - [ ] also will be nice add std::format for setting text 
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
