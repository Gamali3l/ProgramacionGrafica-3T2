#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_easy_font.h>
#include <string>

void DrawTitleScreen(GLFWwindow* window)
{
    const char* title = "LOREM IPSUM ZOO";
    const char* names[] = { "Lorem 1", "Lorem 2", "Lorem 3", "Lorem 4" };
    const char* prompt = "Press ENTER to start";

    float y = 200.0f;
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.25f, 0.4f);
    char buffer[99999];
    int num_quads = stb_easy_font_print(10, y, (char*)title, NULL, buffer, sizeof(buffer));

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    y += 60;

    for (int i = 0; i < 4; ++i) {
        num_quads = stb_easy_font_print(10, y, (char*)names[i], NULL, buffer, sizeof(buffer));
        glDrawArrays(GL_QUADS, 0, num_quads * 4);
        y += 30;
    }

    num_quads = stb_easy_font_print(10, y + 40, (char*)prompt, NULL, buffer, sizeof(buffer));
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glfwSwapBuffers(window);
}
