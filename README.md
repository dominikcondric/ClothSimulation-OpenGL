# ClothSimulation-OpenGL
## Cloning the project
This project uses git submodules, so clone with:  
```
git clone --recurse-submodules https://github.com/dominikcondric/ClothSimulation-OpenGL.git
```

## About the project
Project contains a cloth simulation written in C++/GLSL with minimal dependencies with a goal of understanding computer simulation and related subjects such as 
mathematical models behind simulations as well as numerical integration methods. Mathematical model behind this cloth simulation is a mass-spring model
based on Newtonian mechanics with addition of extrenal forces of gravity and wind acting on the cloth. Scene also contains a sphere that collides with the cloth
as well as point light source and skybox. All physics computation is done on the CPU, but that could be transferred to GPU in the future.

## Controls
**Right arrow** - moves sphere in positive x direction of a scene camera  
**Left arrow** - moves sphere in negative x direction of a scene camera  
**Up arrow** - moves sphere in positive z direction of a scene camera  
**Down arrow** - moves sphere inn negative z direction of a scene camera  
**Right shift** - moves sphere in positive y direction of a scene camera  
**Right control** - moves sphere in positive y direction of a scene camera  
**W** - moves camera in positive z direction of a scene camera  
**A** - moves camera in positive x direction of a scene camera   
**S** - moves camera in positive z direction of a scene camera   
**D** - moves camera in positive x direction of a scene camera   
**Spacebar** - moves camera in positive y direction of a scene camera   
**Left shift** - moves camera in positive y direction of a scene camera  
