#version 460 core
out vec4 FragColor;

in float FaceIndex;

uniform int colorMask;

void main()
{
    // Colors for the faces
    vec3 color;
    int index = int(round(FaceIndex));

    if ((colorMask & (1 << index)) == 0) {
        color = vec3(0.0, 0.0, 0.0); // black internals
    } else {
        if (index == 0) color = vec3(0.0, 0.0, 1.0); // Blue (Back)
        else if (index == 1) color = vec3(0.0, 1.0, 0.0); // Green (Front)
        else if (index == 2) color = vec3(1.0, 0.5, 0.0); // Orange (Left)
        else if (index == 3) color = vec3(1.0, 0.0, 0.0); // Red (Right)
        else if (index == 4) color = vec3(1.0, 1.0, 0.0); // Yellow (Bottom)
        else if (index == 5) color = vec3(1.0, 1.0, 1.0); // White (Top)
        else color = vec3(0.0, 0.0, 0.0); // Black (Internal/Error)    
    }


    FragColor = vec4(color, 1.0);
}