configure_file(
	PathConfig.h.in 
	PathConfig.h
)

add_executable(ClothSimulation
	Camera.cpp 		Camera.h
	Cloth.cpp 		Cloth.h
	Cube.cpp 		Cube.h
	Entity.cpp 		Entity.h
	Shader.cpp 		Shader.h
	Sphere.cpp 		Sphere.h
	Texture.cpp 	Texture.h
	Window.cpp 		Window.h
	main.cpp
)

if (MSVC)
	target_compile_options(ClothSimulation PRIVATE /W4)
else()
	target_compile_options(ClothSimulation PRIVATE -Wall -Wextra -pedantic)
endif()


target_include_directories(ClothSimulation
	PRIVATE
		${CMAKE_SOURCE_DIR}/Dependencies/ImageLoader/
		${CMAKE_SOURCE_DIR}/Dependencies/glm/
		${CMAKE_BINARY_DIR}/src/
)

target_link_libraries(ClothSimulation
	PRIVATE
		Glad
		OpenGL::GL
		glfw
)
