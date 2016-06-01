#ifndef _functions_glfw_hpp_
#define _functions_glfw_hpp_

#ifdef EZ_WITH_GLFW

#include <string>
#include <GLFW/glfw3.h>
#include "optional.hpp"

namespace ez {

typedef GLFWwindow* Window;

void _error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW error %d: %s", error, desc);
}

void _key_callback(GLFWwindow* window, int key, int scancode,
                   int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

ez::optional<std::string> open_window(Window& window,
                                      const std::string& title,
                                      unsigned int width, unsigned int height)
{
    if (!glfwInit()) {
        return ez::optional<std::string>("couldn't initialize GLFW");
    }
    glfwSetErrorCallback(&_error_callback);
    window = glfwCreateWindow(width, height, title.c_str(),
                            NULL, NULL);
    if (!window) {
        return ez::optional<std::string>("couldn't open window");
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, &_key_callback);

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, fb_width, fb_height, 0.0, 1.0, -1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return ez::optional<std::string>();
}

void close_window(Window& window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void update_window(Window& window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool should_close_window(Window& window) {
    return glfwWindowShouldClose(window);
}

void clear_window(Window& window) {
    glClear(GL_COLOR_BUFFER_BIT);
}

struct Color {
    unsigned int red;
    unsigned int green;
    unsigned int blue;

    Color(unsigned int red, unsigned int green, unsigned int blue)
        : red(red)
        , green(green)
        , blue(blue)
    {

    }
};

Color color(unsigned int red, unsigned int green, unsigned int blue) {
    return Color(red, green, blue);
}

void put_pixel(Window& window,
               unsigned int x, unsigned int y, const Color& color)
{
    glBegin(GL_POINTS);
        glColor3ub(color.red, color.green, color.blue);
        glVertex2i(x, y);
    glEnd();
}

}

#endif

#endif

