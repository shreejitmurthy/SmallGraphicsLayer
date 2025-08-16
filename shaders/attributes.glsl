@vs vs
in vec3 position;
in vec4 aColor;
  
out vec4 colour;

void main() {
    gl_Position = vec4(position.xyz, 1.0);
    colour = aColor;
}
@end

@fs fs
in vec4 colour;

out vec4 FragColor;

void main() {
    FragColor = colour;
}
@end

@program main vs fs
