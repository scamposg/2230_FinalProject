# Inception

Our program is designed to recreate the city bending scene from the movie Inception. We use L-systems to procedurally generate the city geometry, texture and normal maps to paint the buildings, and Bezier curves to translate and rotate the buildings to mimic the bending effect.

To use the program, click on the ‘Generate City’ button to generate the buildings, roads, and grass geometry. You can view the scene by clicking and dragging on the viewport as well as using the WASD keys to move the camera. To initiate the bending effect, click on the ‘Play/Pause Scene’ button which will gradually bend the scene in real time. The ‘Reset Scene’ button will reset the entire scene to its original flat appearance. Finally, the ‘Camera Jitter’ checkbox will add a small jitter to the camera for dramatic effect while the bending scene is playing. The program window can be resized at any time to change the resolution of the scene.

We implemented the following new features in our program:

- **Procedural City Generation:** Within city.cpp, we procedurally generate the buildings, roads, and grass that populate our scene by adding the corresponding CTMs to a vector container. Our scene is a square with side length m_radius which is set to 50.
-  **L-Systems:** We use L-systems to determine the location of our buildings and roads by generating a string that encodes a traversal along our scene. The traversal includes instructions such as going in the direction of travel, inverting the direction, switching the direction, and placing a building at the current location. The recursion depth, which determines how dense the city looks can be changed within city.cpp and is set to a value of 7.
-  **Bezier Curves:** We make use of Bezier curves within bezier.cpp to translate and rotate our objects to create a bending effect. We use a cubic polynomial to interpolate the positions and orientations of the objects in our scene.
-  **Texture Mapping:** The loadTextures() function within glrenderer.cpp loads a set of 11 building textures which are randomly assigned to each of the buildings in the scene. A sampler2D containing this texture is passed on to our fragment shader which replaces the diffuse component of our fragment with an interpolated pixel from the texture.
-  **Normal Mapping:** Normal mapping replaces per-fragment normals in each of the object's tangent space to produce the illusion of depth on a flat polygon. We added a tangent vector component to our object VBOs which is used to calculate the tangent-bitangent-normal matrix used to compute the bumped normal in the fragment shader.

While we attempted to incorporate shadow maps into our program, we weren't successful but you can still find our code throughout the program. Namely, we created a shadow.frag and shadow.vert shaders as well as a shadow_map() function within glrenderer.cpp to create a depth map and store it in an FBO.
