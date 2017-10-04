//
// Created by Ricardo Delfin on 10/14/16.
//

/*
 * Main method for the wave equation simulation.
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// OpenGL library includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <debuggl.h>
#include "../include/Field.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 780

static const char* vertex_shader =
R"zzz(#version 330 core
in vec4 vertex_position;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 light_position;
out vec4 light_direction;
void main()
{
    // Transform vertex into clipping coordinates
    gl_Position = projection * view * vertex_position;
    // Lighting in camera coordinates
    //  Compute light direction and transform to camera coordinates
    //        light_direction = view * (light_position - vertex_position);
    //  Transform normal to camera coordinates
}
)zzz";

static const char* fragment_shader =
R"zzz(#version 330 core
in vec4 light_direction;
out vec4 fragment_color;
void main()
{
    vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
//  float dot_nl = dot(normalize(light_direction), normalize(normal));
//  dot_nl = clamp(dot_nl, 0.0, 1.0);
//  fragment_color = clamp(dot_nl * color, 0.0, 1.0);
    fragment_color = color;
}
)zzz";

// VBO and VAO descriptors.
enum { kVertexBuffer, kIndexBuffer, kNumVbos };

// These are our VAOs.
enum { kGeometryVao, kNumVaos };

GLuint g_array_objects[kNumVaos];  // This will store the VAO descriptors.
GLuint g_buffer_objects[kNumVaos][kNumVbos];  // These will store VBO descriptors.


std::vector<glm::vec4> obj_vertices;
std::vector<glm::uvec2> obj_faces;

void
ErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error: " << description << "\n";
}

int main() {
    /*===================================================================================
     *============================ GLFW Initialization ==================================
     *===================================================================================*/
    /* Initialize library */
    if(!glfwInit()) exit(EXIT_FAILURE);
    glfwSetErrorCallback(ErrorCallback);

    /* Send hints to the window */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Generate window, set swap interval, and print info about screen
    GLFWwindow* window  = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window 1", NULL, NULL);
    CHECK_SUCCESS(window != nullptr);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    CHECK_SUCCESS(glewInit() == GLEW_OK);
    glGetError();  // clear GLEW's error for it
    glfwSwapInterval(1);
    const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
    const GLubyte* version = glGetString(GL_VERSION);    // version as a string
    std::cout << "Renderer: " << renderer << "\n";
    std::cout << "OpenGL version supported:" << version << "\n";


    /*===================================================================================
     *============================= Generate geometry ===================================
     *===================================================================================*/
    Field field(glm::dvec2(-2.5, -2.5), glm::dvec2(2.5, 2.5), 0.1, 0.2);
    std::vector<std::vector<double>> map(50);
    for(unsigned long i = 0; i < map.size(); i++)
        map[i] = std::vector<double>(50);
    int x = 0, y = 0;

    for(int i = 24; i <= 26; i++)
        for(int j = 24; j <= 26; j++)
            map[i][j] = 1;
    /*map[24+x][240+y] = 1;
    map[24+x][250+y] = 1;
    map[24+x][260+y] = 1;
    map[25+x][240+y] = 1;
    map[25+x][250+y] = 1;
    map[25+x][260+y] = 1;
    map[26+x][24+y] = 1;
    map[26+x][25+y] = 1;
    map[26+x][26+y] = 1;*/
    field.setField(map);

    int idx = 0;
    for(int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            obj_vertices.push_back(glm::vec4((i - 25) * 0.1, -1 + map[i][j], (j - 25) * 0.1, 1));

            if(j < 49)
                obj_faces.push_back(glm::uvec2(idx, idx+1));
            if(i < 49)
                obj_faces.push_back(glm::uvec2(idx, idx+50));

            idx++;
        }
    }
    /*===================================================================================
     *============================ GLM LOADING VBO AND VAO ==============================
     *===================================================================================*/
    CHECK_GL_ERROR(glGenVertexArrays(kNumVaos, &g_array_objects[0]));
    CHECK_GL_ERROR(glBindVertexArray(g_array_objects[kGeometryVao]));
    CHECK_GL_ERROR(glGenBuffers(kNumVbos, &g_buffer_objects[kGeometryVao][0]));
    CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kVertexBuffer]));
    CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                                sizeof(float) * obj_vertices.size() * 4, nullptr,
                                GL_STATIC_DRAW));
    CHECK_GL_ERROR(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
    CHECK_GL_ERROR(glEnableVertexAttribArray(0));
    CHECK_GL_ERROR(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0));
    CHECK_GL_ERROR(glEnableVertexAttribArray(1));
    CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kIndexBuffer]));
    CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                sizeof(uint32_t) * obj_faces.size() * 2,
                                &obj_faces[0], GL_STATIC_DRAW));


    /*===================================================================================
     *============================ GLM LOADING VBO AND VAO ==============================
     *===================================================================================*/
    // Setup vertex shader.
    GLuint vertex_shader_id = 0;
    const char* vertex_source_pointer = vertex_shader;
    CHECK_GL_ERROR(vertex_shader_id = glCreateShader(GL_VERTEX_SHADER));
    CHECK_GL_ERROR(glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, nullptr));
    glCompileShader(vertex_shader_id);
    CHECK_GL_SHADER_ERROR(vertex_shader_id);

    // Setup fragment shader.
    GLuint fragment_shader_id = 0;
    const char* fragment_source_pointer = fragment_shader;
    CHECK_GL_ERROR(fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER));
    CHECK_GL_ERROR(glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, nullptr));
    glCompileShader(fragment_shader_id);
    CHECK_GL_SHADER_ERROR(fragment_shader_id);

    // Let's create our program.
    GLuint program_id = 0;
    CHECK_GL_ERROR(program_id = glCreateProgram());
    CHECK_GL_ERROR(glAttachShader(program_id, vertex_shader_id));
    CHECK_GL_ERROR(glAttachShader(program_id, fragment_shader_id));
//	CHECK_GL_ERROR(glAttachShader(program_id, geometry_shader_id));

    CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, g_buffer_objects[kGeometryVao][kVertexBuffer]));
    // NOTE: We do not send anything right now, we just describe it to OpenGL.
    CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                                sizeof(float) * obj_vertices.size() * 4, nullptr,
                                GL_STATIC_DRAW));
    // Bind attributes.
    CHECK_GL_ERROR(glBindAttribLocation(program_id, 0, "vertex_position"));

    CHECK_GL_ERROR(glBindAttribLocation(program_id, 1, "vertex_normal"));
    CHECK_GL_ERROR(glBindFragDataLocation(program_id, 0, "fragment_color"));
    glLinkProgram(program_id);
    CHECK_GL_PROGRAM_ERROR(program_id);

    // Get the uniform locations.
    GLint projection_matrix_location = 0;
    CHECK_GL_ERROR(projection_matrix_location =
                           glGetUniformLocation(program_id, "projection"));
    GLint view_matrix_location = 0;
    CHECK_GL_ERROR(view_matrix_location =
                           glGetUniformLocation(program_id, "view"));
    GLint light_position_location = 0;
    CHECK_GL_ERROR(light_position_location =
                           glGetUniformLocation(program_id, "light_position"));

    glm::vec4 light_position = glm::vec4(2.0f, 2.0f, 2.0f, 1.0f);
    float aspect = 0.0f;
    float theta = 0.0f;

    glfwSetTime(0);

    while(!glfwWindowShouldClose(window)) {

        double frameTime = glfwGetTime();
        glfwSetTime(0.0);

        field.update(frameTime);

        obj_vertices.clear();
        for(int i = 0; i < 50; i++) {
            for (int j = 0; j < 50; j++) {
                double fxt = field.getValAt(glm::dvec2(-2.5 + (i-1)*0.1, -2.5 + (j-1)*0.1));
                obj_vertices.push_back(glm::vec4((i - 25) * 0.1, -1 + fxt, (j - 25) * 0.1, 1));
            }
        }

        // Setup some basic window stuff.
        int screenW, screenH;
        glfwGetFramebufferSize(window, &screenW, &screenH);
        glViewport(0, 0, screenW, screenH);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LESS);

        // Switch to the Geometry VAO.
        CHECK_GL_ERROR(glBindVertexArray(g_array_objects[kGeometryVao]));



        // Compute the projection matrix.
        aspect = static_cast<float>(screenW) / screenH;
        glm::mat4 projection_matrix =
                glm::perspective(glm::radians(45.0f), aspect, 0.0001f, 1000.0f);


        glm::mat4 view_matrix = glm::lookAt(glm::vec3(0, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        // Send vertices to the GPU.
        CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER,
                                    g_buffer_objects[kGeometryVao][kVertexBuffer]));
        CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                                    sizeof(float) * obj_vertices.size() * 4,
                                    &obj_vertices[0], GL_STATIC_DRAW));
        // Use our program.
        CHECK_GL_ERROR(glUseProgram(program_id));

        // Pass uniforms in.
        CHECK_GL_ERROR(glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE,
                                          &projection_matrix[0][0]));
        CHECK_GL_ERROR(glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE,
                                          &view_matrix[0][0]));
        CHECK_GL_ERROR(glUniform4fv(light_position_location, 1, &light_position[0]));

        CHECK_GL_ERROR(glDrawElements(GL_LINES, obj_faces.size() * 2, GL_UNSIGNED_INT, 0));

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}

