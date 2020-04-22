#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <iostream>


unsigned long vertex_count;
unsigned vao;
unsigned int vbo;

//function definitions 
void load_geometry() {
    // Send the vertex data to the GPU
    {
        // Generate the data on the CPU
        GLfloat vertices[] = {
            0.0f, 0.5f, 0.0f, // top center
            0.5f, -0.5f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f, // bottom left
        };
        vertex_count = sizeof(vertices) / sizeof(vertices[0]);

        // Use OpenGL to store it on the GPU
        {
            // Create a Vertex Buffer Object on the GPU
            glGenBuffers(1, &vbo);
            // Tell OpenGL that we want to set this as the current buffer
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            // Copy all our data to the current buffer!
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        }
    }

    // Tell the GPU how to interpret our existing vertex data
    {
        // Create a Vertex Array Object to hold the settings
        glGenVertexArrays(1, &vao);

        // Tell OpenGL that we want to set this as the current vertex array
        glBindVertexArray(vao);

        // Tell OpenGL the settings for the current 0th vertex array!
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

        // Enable the 0th vertex attrib array!
        glEnableVertexAttribArray(0);
    }
}//end load_geometry
//
GLuint compile_shader() {
    // Define shader sourcecode
    const char* vertex_shader_src =
        "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "uniform vec2 offset;\n"
        "void main() {\n"
        "   gl_Position = vec4(pos.x + offset.x, pos.y + offset.y, pos.z, 1.0);\n"
        "}\n";
    const char* fragment_shader_src =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 color;\n"
        "uniform sampler2D tex;\n"
        "void main() {\n"
        "   vec2 uvs = vec2(gl_FragCoord) / 100.0;\n"
        "   FragColor = texture(tex, uvs);\n"
        //        "   FragColor = color;\n"
        "}\n";
    //GLint color_location = glGetUniformLocation(compile_shader(), "color");
    //glUniform4f(color_location, 1.0, 0.0, 0.0, 1.0);
    // Define some vars
    const int MAX_ERROR_LEN = 512;
    GLint success;
    GLchar infoLog[MAX_ERROR_LEN];

    // Compile the vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);

    // Check for errors
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, MAX_ERROR_LEN, NULL, infoLog);
        std::cout << "vertex shader compilation failed:\n" << infoLog << std::endl;
        abort();
    }

    // Compile the fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);

    // Check for errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, MAX_ERROR_LEN, NULL, infoLog);
        std::cout << "fragment shader compilation failed:\n" << infoLog << std::endl;
        abort();
    }

    // Link the shaders
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, MAX_ERROR_LEN, NULL, infoLog);
        std::cout << "shader linker failed:\n" << infoLog << std::endl;
        abort();
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Enable the shader here since we only have one
    glUseProgram(shader_program);

    return shader_program;
}//end compile_shader()

void tri(GLsizei vertex_count){
    //load multiple triangles here
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);

}


GLFWwindow* initialize_glfw() {
    // Initialize the context
    if (!glfwInit()) {
        std::cout << "glfwInit(...) failed\n";
        abort();
    }

    // Ask for a core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        std::cout << "glfwCreateWindow(...) failed\n";
        glfwTerminate();
        abort();
    }

    // Make the context current
    glfwMakeContextCurrent(window);

    // Load the function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    return window;
}



void uniform(GLuint shader) {
    //color change
    GLint color_location = glGetUniformLocation(shader, "color");
    glUniform4f(color_location, 1.0, 0.0, 0.0, 1.0);

    //motion moves up right 
    GLint move = glGetUniformLocation(shader, "offset");
    glUniform2f(move, .8, .5);
}

void offsetUniform(GLuint shader, int num) {
    //basic offset uniform here 
    //motion moves up right 
    GLint move = glGetUniformLocation(shader, "offset");
    glUniform2f(move, .1*num, .2*num);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    //newtonian 
    //particle.velocity += particle.acceleration * delta_time;
    //particle.position += particle.velocity * delta_time;

}


void colorUniform(GLuint shader) {
    //basic color uniform here
    //color change
    GLint color_location = glGetUniformLocation(shader, "color");
    glUniform4f(color_location, 1.0, 1.0, 0.0, 1.0);
}

void load_textures() {
    glActiveTexture(GL_TEXTURE0); 
    GLuint tex; 
    glGenTextures(1, &tex); 
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    //set pixels -- typicalls from file vs in funct 
    GLsizei width = 2;
    GLsizei height = 2;
    float pixels[] = {
    0.0f, 0.0f, 0.0f,	1.0f, 1.0f, 1.0f, // r, g, b,   r, g, b
    1.0f, 1.0f, 1.0f,	0.0f, 0.0f, 0.0f, // r, g, b,   r, g, b
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixels);
    
    //GLuint tex_location = glGetUniformLocation(compile_shader, tex);
    //glUniform1i(tex_location, 0);

    //glDeleteTextures(1, &tex);

}
void render_scene(GLFWwindow* window, GLsizei vertex_count, GLuint shader) {
    // Set the clear color
    glClearColor(0.7f, 0.0f, 0.5f, 1.0f);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the current vao/vbo, with the current shader
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    offsetUniform(shader, 1);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    offsetUniform(shader, 2);
    // Display the results on screen
    glfwSwapBuffers(window);
}//end render_scene()
//
void cleanup(GLFWwindow* window) {
    // Call glfw terminate here
    //glTerminate();
    //glDeleteTextures(1, &tex);
}

//end function definitions
int main(void) {
    GLFWwindow* window = initialize_glfw();
    GLuint shader = compile_shader();
    load_geometry();
    //uniform(shader); 
    //colorUniform(shader); 
    //offsetUniform(shader);
    //offsetUniform(shader);


    

    while (!glfwWindowShouldClose(window)) {
        render_scene(window, 3, shader);
        glfwPollEvents();
    }

    cleanup(window);
    return 0;
}
