#include <stdio.h>
#include <GLFW/glfw3.h>

// Function prototype for loading a video frame
bool load_frame(const char *filename, int *width, int *height, unsigned char **data);

int main(int argc, const char **argv)
{
    if (!glfwInit())
    {
        printf("Couldn't init GLFW\n");
        return 1;
    }

    // Create a windowed mode windoww and its OpenGL context
    GLFWwindow *window;
    window = glfwCreateWindow(640, 480, "Viditor", NULL, NULL);
    if (!window)
    {
        printf("Couldn't create window\n");
        glfwTerminate();
        return 1;
    }

    // Load a video frame
    int frame_width, frame_height;
    unsigned char *frame_data;
    if (!load_frame("/home/itsuki/cpp_projects/video-app/videos/toho.mp4", &frame_width, &frame_height, &frame_data))
    {
        printf("Couldn't load video frame\n");
        glfwTerminate();
        return 1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Generate and configure a texture for displaying the video frame
    GLuint tex_handle;
    glGenTextures(1, &tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);

    // Set texture parameters
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Specify the texture image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up an orphographic projection matrix
        int window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, window_width, window_height, 0, -1, 1); // Set up a 2D orthographic projection
        glMatrixMode(GL_MODELVIEW);

        // Render the texture onto a rectangle
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex2i(200, 200); // Bottom-left corner
        glTexCoord2d(1, 0);
        glVertex2i(200 + frame_width, 200); // Bottom-right corner
        glTexCoord2d(1, 1);
        glVertex2i(200 + frame_width, 200 + frame_height); // Top-right corner
        glTexCoord2d(0, 1);
        glVertex2i(200, 200 + frame_height); // Top-left corner
        glEnd();
        glDisable(GL_TEXTURE_2D);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwWaitEvents();
    }

    // Cleanup and exit
    glfwDestroyWindow(window);
    glfwTerminate();
    delete[] frame_data; // Free the memory allocated for the frame data

    return 0;
}