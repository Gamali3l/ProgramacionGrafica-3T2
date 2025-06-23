#include "Model.h" // Include custom model loader class
#include "Camara.h" // Include custom camera class
#include "Shaderclass.h" // Include custom shader class
#include <stb_image.h> // Include stb_image for image loading
#include <gtc/matrix_transform.hpp> // Include GLM for matrix transformations

const unsigned int width = 1800; // Set screen width
const unsigned int height = 800; // Set screen height

// Define vertices for a cube used as the skybox
float skyboxVertices[] = {
    // Positions for the 36 vertices of a cube
    -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
};

// Load cubemap textures for the skybox
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID); // Generate a texture ID
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); // Bind as cubemap

    int width, height, nrChannels;

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        // Load image using stb_image
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            std::cout << "[OK] Cargada: " << faces[i] << " (" << width << "x" << height << ", canales: " << nrChannels << ")" << std::endl;
            // Upload image to the GPU as a cubemap face
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data); // Free image memory
        }
        else
        {
            std::cout << "[ERROR] No se pudo cargar: " << faces[i] << std::endl;
            stbi_image_free(data); // Free if failed too
        }
    }

    // Set cubemap texture filtering and wrapping parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID; // Return the texture ID
}
int main()
{
    glfwInit(); // Initialize GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Set OpenGL major version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Set OpenGL minor version
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(width, height, "Cargador de Modelos 3D con Assimp", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // Terminate if window creation fails
        return -1;
    }
    glfwMakeContextCurrent(window); // Set current OpenGL context

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, width, height); // Set the viewport size
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glEnable(GL_BLEND); // Enable alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blend function

    Shader shaderProgram("default.vert", "default.frag"); // Load main shader program
    Shader skyboxShader("skybox.vert", "skybox.frag"); // Load skybox shader program

    glm::vec3 lightPos(50.0f, 100.0f, 50.0f); // Light position in world space
    glm::vec3 lightColor(1.0f, 1.0f, 0.95f); // Light color

    shaderProgram.Activate(); // Use main shader
    shaderProgram.setVec3("lightColor", lightColor); // Set light color in shader
    shaderProgram.setVec3("lightPos", lightPos); // Set light position in shader

    Camera camera(width, height, glm::vec3(0.0f, 10.0f, 10.0f)); // Create camera

    // Load all models to be used in the scene
    Model model("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/zoo/Zoo.gltf");
    Model velociraptor("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/velociraptor/scene.gltf");
    Model velociraptor2("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/velociraptor/scene.gltf");
    Model tigre("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/tiger/scene.gltf");
    Model tigre2("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/tiger/scene.gltf");
    Model mono1("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/monkey/monkey.gltf");
    Model mono2("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/monkey/monkey.gltf");
    Model mono3("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/monkey3/scene.gltf");
    Model mono4("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/monkey4/scene.gltf");
    Model mono5("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/monkey2/scene.gltf");
    Model mono6("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/monkey2/scene.gltf");
    Model mono7("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/monkey2/scene.gltf");
    Model puma("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/puma/scene.gltf");
    Model puma2("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/puma/scene.gltf");
    Model bird("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/bird/scene.gltf");
    Model bird2("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/bird/scene.gltf");
    Model bird3("C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/bird/scene.gltf");

    // Skybox VAO and VBO setup
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // List of texture paths for skybox faces
    std::vector<std::string> faces{
        "C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/Skybox/bluecloud_rt.jpg",
        "C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/Skybox/bluecloud_lf.jpg",
        "C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/Skybox/bluecloud_up.jpg",
        "C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/Skybox/bluecloud_dn.jpg",
        "C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/Skybox/bluecloud_ft.jpg",
        "C:/Users/MI PC/Desktop/1 SEMESTRE 2025/PROGRAMACION GRAFICA/lab4.1/Modelos/Skybox/bluecloud_bk.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // ----------- Main render loop -----------
    while (!glfwWindowShouldClose(window))
    {
        camera.Inputs(window); // Handle camera inputs
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f); // Set background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
        camera.updateMatrix(45.0f, 1.0f, 3000.0f); // Update camera matrix

        // --- Draw skybox ---
        glDepthFunc(GL_LEQUAL); // Change depth function to draw skybox
        skyboxShader.Activate(); // Use skybox shader
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Get view matrix without translation
        skyboxShader.setMat4("view", view); // Set view matrix
        skyboxShader.setMat4("projection", camera.GetProjectionMatrix()); // Set projection matrix
        glBindVertexArray(skyboxVAO); // Bind skybox VAO
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); // Bind cubemap texture
        glDrawArrays(GL_TRIANGLES, 0, 36); // Draw skybox cube
        glBindVertexArray(0); // Unbind VAO
        glDepthFunc(GL_LESS); // Reset depth function

        // --- Draw zoo scene ---
        shaderProgram.Activate(); // Use main shader
        shaderProgram.setVec3("viewPos", camera.Position); // Set view position
        shaderProgram.setMat4("view", camera.GetViewMatrix()); // Set view matrix
        shaderProgram.setMat4("projection", camera.GetProjectionMatrix()); // Set projection matrix

        glm::mat4 modelMat = glm::mat4(1.0f); // Create model matrix
        modelMat = glm::scale(modelMat, glm::vec3(0.01f)); // Scale zoo model
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, -2.0f, 0.0f)); // Translate model down
        modelMat = glm::rotate(modelMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate model upright
        shaderProgram.setMat4("model", modelMat); // Set model matrix in shader
        model.Draw(shaderProgram); // Draw the zoo model

        // --- Draw velociraptor 2 ---
        glm::mat4 velociraptor2ModelMat = glm::mat4(1.0f);
        velociraptor2ModelMat = glm::translate(velociraptor2ModelMat, glm::vec3(-144.0f, -0.1f, -102.099f)); // Position
        velociraptor2ModelMat = glm::rotate(velociraptor2ModelMat, glm::radians(47.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate
        velociraptor2ModelMat = glm::scale(velociraptor2ModelMat, glm::vec3(1.0f)); // Scale
        shaderProgram.setMat4("model", velociraptor2ModelMat);
        velociraptor.Draw(shaderProgram);

        // --- Draw velociraptor ---
        glm::mat4 velociraptorModelMat = glm::mat4(1.0f);
        velociraptorModelMat = glm::translate(velociraptorModelMat, glm::vec3(-155.3f, -0.1f, -116.399f));
        velociraptorModelMat = glm::rotate(velociraptorModelMat, glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        velociraptorModelMat = glm::scale(velociraptorModelMat, glm::vec3(1.0f));
        shaderProgram.setMat4("model", velociraptorModelMat);
        velociraptor.Draw(shaderProgram);

        // --- Draw tiger ---
        glm::mat4 tigreModelMat = glm::mat4(1.0f);
        tigreModelMat = glm::translate(tigreModelMat, glm::vec3(-111.199f, 2.0f, -11.0f));
        tigreModelMat = glm::rotate(tigreModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Flip upward
        tigreModelMat = glm::rotate(tigreModelMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Correct upside down
        tigreModelMat = glm::rotate(tigreModelMat, glm::radians(357.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Final rotation
        tigreModelMat = glm::scale(tigreModelMat, glm::vec3(0.002f)); // Smaller scale
        shaderProgram.setMat4("model", tigreModelMat);
        tigre.Draw(shaderProgram);

        // --- Draw tiger 2 ---
        glm::mat4 tigre2ModelMat = glm::mat4(1.0f);
        tigre2ModelMat = glm::translate(tigre2ModelMat, glm::vec3(-124.999f, 2.0f, -25.1001f));
        tigre2ModelMat = glm::rotate(tigre2ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        tigre2ModelMat = glm::rotate(tigre2ModelMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        tigre2ModelMat = glm::rotate(tigre2ModelMat, glm::radians(357.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        tigre2ModelMat = glm::scale(tigre2ModelMat, glm::vec3(0.002f));
        shaderProgram.setMat4("model", tigre2ModelMat);
        tigre2.Draw(shaderProgram);

        // --- Draw monkey 1 ---
        glm::mat4 mono1ModelMat = glm::mat4(1.0f);
        mono1ModelMat = glm::translate(mono1ModelMat, glm::vec3(-141.399f, 0.400003f, -34.5f));
        mono1ModelMat = glm::rotate(mono1ModelMat, glm::radians(255.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mono1ModelMat = glm::rotate(mono1ModelMat, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mono1ModelMat = glm::rotate(mono1ModelMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mono1ModelMat = glm::scale(mono1ModelMat, glm::vec3(1.0f));
        shaderProgram.setMat4("model", mono1ModelMat);
        mono1.Draw(shaderProgram);

        // --- Draw monkey 2 ---
        glm::mat4 mono2ModelMat = glm::mat4(1.0f);
        mono2ModelMat = glm::translate(mono2ModelMat, glm::vec3(-153.9f, 0.100003f, -64.3996f));
        mono2ModelMat = glm::rotate(mono2ModelMat, glm::radians(103.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mono2ModelMat = glm::rotate(mono2ModelMat, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mono2ModelMat = glm::rotate(mono2ModelMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mono2ModelMat = glm::scale(mono2ModelMat, glm::vec3(1.0f));
        shaderProgram.setMat4("model", mono2ModelMat);
        mono2.Draw(shaderProgram);

        // --- Draw monkey 3 ---
        glm::mat4 mono3ModelMat = glm::mat4(1.0f);
        mono3ModelMat = glm::translate(mono3ModelMat, glm::vec3(-163.601f, 5.7f, -46.9999f));
        mono3ModelMat = glm::rotate(mono3ModelMat, glm::radians(279.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mono3ModelMat = glm::rotate(mono3ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mono3ModelMat = glm::rotate(mono3ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mono3ModelMat = glm::scale(mono3ModelMat, glm::vec3(0.1f));
        shaderProgram.setMat4("model", mono3ModelMat);
        mono3.Draw(shaderProgram);

        // --- Draw monkey 4 ---
        glm::mat4 mono4ModelMat = glm::mat4(1.0f);
        mono4ModelMat = glm::translate(mono4ModelMat, glm::vec3(-153.5f, 1.1f, -35.1f));
        mono4ModelMat = glm::rotate(mono4ModelMat, glm::radians(53.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mono4ModelMat = glm::rotate(mono4ModelMat, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mono4ModelMat = glm::rotate(mono4ModelMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mono4ModelMat = glm::scale(mono4ModelMat, glm::vec3(1.0f));
        shaderProgram.setMat4("model", mono4ModelMat);
        mono4.Draw(shaderProgram);

        // --- Draw monkey 5 ---
        glm::mat4 mono5ModelMat = glm::mat4(1.0f);
        mono5ModelMat = glm::translate(mono5ModelMat, glm::vec3(-157.5f, 0.3f, -64.1996f));
        mono5ModelMat = glm::rotate(mono5ModelMat, glm::radians(25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mono5ModelMat = glm::rotate(mono5ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mono5ModelMat = glm::rotate(mono5ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mono5ModelMat = glm::scale(mono5ModelMat, glm::vec3(2.0f));
        shaderProgram.setMat4("model", mono5ModelMat);
        mono5.Draw(shaderProgram);

        // --- Draw monkey 6 ---
        glm::mat4 mono6ModelMat = glm::mat4(1.0f);
        mono6ModelMat = glm::translate(mono6ModelMat, glm::vec3(-126.198f, 0.3f, -68.4995f));
        mono6ModelMat = glm::rotate(mono6ModelMat, glm::radians(305.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mono6ModelMat = glm::rotate(mono6ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mono6ModelMat = glm::rotate(mono6ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mono6ModelMat = glm::scale(mono6ModelMat, glm::vec3(2.0f));
        shaderProgram.setMat4("model", mono6ModelMat);
        mono6.Draw(shaderProgram);

        // --- Draw monkey 7 ---
        glm::mat4 mono7ModelMat = glm::mat4(1.0f);
        mono7ModelMat = glm::translate(mono7ModelMat, glm::vec3(-158.6f, 0.3f, -71.1995f));
        mono7ModelMat = glm::rotate(mono7ModelMat, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mono7ModelMat = glm::rotate(mono7ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mono7ModelMat = glm::rotate(mono7ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mono7ModelMat = glm::scale(mono7ModelMat, glm::vec3(2.0f));
        shaderProgram.setMat4("model", mono7ModelMat);
        mono7.Draw(shaderProgram);

        // --- Draw puma ---
        glm::mat4 pumaModelMat = glm::mat4(1.0f);
        pumaModelMat = glm::translate(pumaModelMat, glm::vec3(-95.2991f, 0.200003f, -11.3f)); // Position
        pumaModelMat = glm::rotate(pumaModelMat, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation Y
        pumaModelMat = glm::rotate(pumaModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Flip upside down
        pumaModelMat = glm::rotate(pumaModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Flip around Z
        pumaModelMat = glm::scale(pumaModelMat, glm::vec3(0.04f)); // Scale down
        shaderProgram.setMat4("model", pumaModelMat);
        puma.Draw(shaderProgram);

        // --- Draw puma 2 ---
        glm::mat4 puma2ModelMat = glm::mat4(1.0f);
        puma2ModelMat = glm::translate(puma2ModelMat, glm::vec3(-129.399f, 0.200003f, -8.2f));
        puma2ModelMat = glm::rotate(puma2ModelMat, glm::radians(302.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        puma2ModelMat = glm::rotate(puma2ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        puma2ModelMat = glm::rotate(puma2ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        puma2ModelMat = glm::scale(puma2ModelMat, glm::vec3(0.04f));
        shaderProgram.setMat4("model", puma2ModelMat);
        puma2.Draw(shaderProgram);

        // --- Draw bird ---
        glm::mat4 birdModelMat = glm::mat4(1.0f);
        birdModelMat = glm::translate(birdModelMat, glm::vec3(-48.2998f, 11.9f, -134.499f));
        birdModelMat = glm::rotate(birdModelMat, glm::radians(162.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        birdModelMat = glm::rotate(birdModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        birdModelMat = glm::rotate(birdModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        birdModelMat = glm::scale(birdModelMat, glm::vec3(0.8f));
        shaderProgram.setMat4("model", birdModelMat);
        bird.Draw(shaderProgram);

        // --- Draw bird 2 ---
        glm::mat4 bird2ModelMat = glm::mat4(1.0f);
        bird2ModelMat = glm::translate(bird2ModelMat, glm::vec3(-83.5993f, 3.3f, -43.5999f));
        bird2ModelMat = glm::rotate(bird2ModelMat, glm::radians(158.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        bird2ModelMat = glm::rotate(bird2ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        bird2ModelMat = glm::rotate(bird2ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        bird2ModelMat = glm::scale(bird2ModelMat, glm::vec3(0.5f));
        shaderProgram.setMat4("model", bird2ModelMat);
        bird2.Draw(shaderProgram);

        // --- Draw bird 3 ---
        glm::mat4 bird3ModelMat = glm::mat4(1.0f);
        bird3ModelMat = glm::translate(bird3ModelMat, glm::vec3(0.0999996f, 1.4f, -47.5998f));
        bird3ModelMat = glm::rotate(bird3ModelMat, glm::radians(126.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        bird3ModelMat = glm::rotate(bird3ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        bird3ModelMat = glm::rotate(bird3ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        bird3ModelMat = glm::scale(bird3ModelMat, glm::vec3(0.5f));
        shaderProgram.setMat4("model", bird3ModelMat);
        bird3.Draw(shaderProgram);

        glfwSwapBuffers(window); // Swap front and back buffers
        glfwPollEvents(); // Poll for and process events
    }

    // Cleanup resources
    shaderProgram.Delete(); // Delete main shader program
    skyboxShader.Delete(); // Delete skybox shader program
    glfwDestroyWindow(window); // Destroy window
    glfwTerminate(); // Terminate GLFW
    return 0; // Exit application
}