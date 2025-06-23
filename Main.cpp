#include "Model.h"
#include "Camera.h"
#include "ShaderClass.h"
#include <stb/stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <GLFW/glfw3.h>
#include <AL/al.h>
#include <AL/alc.h>
#include "ProximitySoundSystem.h"
#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <AntTweakBar.h>


const unsigned int width = 1800;
const unsigned int height = 800;


void createSphere(float radius, unsigned int sectorCount, unsigned int stackCount,
    std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
    const float PI = 3.14159265359f;
    // Media texel
    float pad = 0.5f / (float)sectorCount;

    for (unsigned int i = 0; i <= stackCount; ++i)
    {
        float stackAngle = PI * i / stackCount;
        float y = cosf(stackAngle);
        float sinStack = sinf(stackAngle);

        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = 2 * PI * j / sectorCount;
            float x = cosf(sectorAngle) * sinStack;
            float z = sinf(sectorAngle) * sinStack;

            // Calculate u with padding
            float u0 = (float)j / sectorCount;
            float u = 1.0f - (pad + u0 * (1.0f - 2.0f * pad));
            float v = glm::clamp((float)i / stackCount, 0.001f, 0.999f);

            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (unsigned int i = 0; i < stackCount; ++i)
    {
        for (unsigned int j = 0; j < sectorCount; ++j)
        {
            unsigned int first = i * (sectorCount + 1) + j;
            unsigned int second = first + sectorCount + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}
//int sceneMain;

//-------------------------------------main------------------------------------------
int main()
{


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Zoo + Skydome", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ImGui setup-----------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 30.0f);
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    // -------------------- Camera --------------------
    Camera camera(width, height, glm::vec3(0.0f, 10.0f, 10.0f));
    //setting up proximitySoundSystem
    ProximitySoundSystem proximitySoundSystem;

    // -------------------- AntTweakBar --------------------
    TwInit(TW_OPENGL_CORE, glfwGetProcAddress);
    TwWindowSize(width, height);

    // GUI
    bool  showWireframe = false;
    float exposure = 1.0f;

    // Camera Bar in rea time
    TwBar* barCamera = TwNewBar("Camera");
    TwDefine("Camera size='200 150' position='10 70' color='255 255 255'");
    TwAddVarRO(barCamera, "PosX", TW_TYPE_FLOAT, &camera.Position.x,
        "label='X' precision=2");
    TwAddVarRO(barCamera, "PosY", TW_TYPE_FLOAT, &camera.Position.y,
        "label='Y' precision=2");
    TwAddVarRO(barCamera, "PosZ", TW_TYPE_FLOAT, &camera.Position.z,
        "label='Z' precision=2");

    // info Bar
    int winW, winH;
    glfwGetWindowSize(window, &winW, &winH);
    int barW = 200;
    int barH = 260;
    int margin = 10;
    int posX = winW - barW - margin;
    int posY = winH - barH - margin;



    // -------------------- AntTweakBar:  info --------------------
    TwBar* barInfo = TwNewBar("Info");



    char buf[256];
    sprintf_s(buf, sizeof(buf),
        "Info size='%d %d' position='%d %d' color='50 50 50' text=light fontsize=3 valuesalign=center",
        barW, barH, posX, posY);
    TwDefine(buf);


    static std::string line0 = "Controls";
    static std::string line1 = "W:Z I";
    static std::string line2 = "S:Z O";
    static std::string line3 = "A:L";
    static std::string line4 = "D:R";
    static std::string line7 = "Mouse:M";

    TwAddVarRO(barInfo, "l0", TW_TYPE_STDSTRING, &line0, "label='' keyincr=-1 valueswidth=200");
    TwAddVarRO(barInfo, "l1", TW_TYPE_STDSTRING, &line1, "label='' keyincr=-1 valueswidth=200");
    TwAddVarRO(barInfo, "l2", TW_TYPE_STDSTRING, &line2, "label='' keyincr=-1 valueswidth=200");
    TwAddVarRO(barInfo, "l3", TW_TYPE_STDSTRING, &line3, "label='' keyincr=-1 valueswidth=200");
    TwAddVarRO(barInfo, "l4", TW_TYPE_STDSTRING, &line4, "label='' keyincr=-1 valueswidth=200");
    TwAddVarRO(barInfo, "l7", TW_TYPE_STDSTRING, &line7, "label='' keyincr=-1 valueswidth=200");

    // Callbacks GLFW → AntTweakBar
    glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW);
    glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);
    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW);
    glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW);
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int w_, int h_) {
        glViewport(0, 0, w_, h_);
        TwWindowSize(w_, h_);
        });

    // -------------------- OpenGL State --------------------
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    Shader shaderProgram("default.vert", "default.frag");
    Shader skyShader("sky_sphere.vs", "sky_sphere.fs");

    glm::vec3 lightPos(50.0f, 100.0f, 50.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 0.95f);

    shaderProgram.Activate();
    shaderProgram.setVec3("lightColor", lightColor);
    shaderProgram.setVec3("lightPos", lightPos);


    Model model("modelo//Zoo.gltf");
    //*************************************************
    Model coco("animals//coco//coco.gltf");
    Model coco2("animals//coco//coco.gltf");
    Model coco3("animals//coco//coco.gltf");
    Model coco4("animals//coco//coco.gltf");
    Model coco5("animals//coco//coco.gltf");
    Model lion("animals//leon//leon.gltf");
    Model peacock("animals//peacock//peacock//Untitled.gltf");
    Model wolf("animals//wolfv2//WOLFV2//Untitled.gltf");
    Model femaleWolf("animals//wolfv2//WOLFV2//Untitled.gltf");
    Model BroWolf("animals//wolfv2//WOLFV2//Untitled.gltf");
    Model SisWolf("animals//wolfv2//WOLFV2//Untitled.gltf");
    Model capy("animals//capy//scene.gltf");
    Model capy2("animals//capy//scene.gltf");
    Model capy3("animals//capy//scene.gltf");
    Model elephant("animals//elephant__gajah//scene.gltf");
    Model biston("animals//biston//scene.gltf");
    Model biston2("animals//biston//scene.gltf");
    Model giraffe("animals//giraffe//scene.gltf");

    //*Gamaliel models

    Model velociraptor("animals//velociraptor//scene.gltf");
    Model velociraptor2("animals//velociraptor//scene.gltf");
    Model tigre("animals//tiger//scene.gltf");
    Model tigre2("animals//tiger//scene.gltf");
    Model mono1("animals//monkey//monkey.gltf");
    Model mono2("animals//monkey//monkey.gltf");
    Model mono3("animals//monkey3//scene.gltf");
    Model mono4("animals//monkey4//scene.gltf");
    Model mono5("animals//monkey2//scene.gltf");
    Model mono6("animals//monkey2//scene.gltf");
    Model mono7("animals//monkey2//scene.gltf");
    Model puma("animals//puma//scene.gltf");
    Model puma2("animals//puma//scene.gltf");
    Model bird("animals//bird//scene.gltf");
    Model bird2("animals//bird//scene.gltf");
    Model bird3("animals//bird//scene.gltf");

    //Setting up proximity sound triggers
    proximitySoundSystem.addTrigger(glm::vec3(20.6f, 0.5f, -82.2993f), 10.0f, "sounds/Coco1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(15.8f, 0.2f, -105.499f), 10.0f, "sounds/Coco2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(36.1f, 0.2f, -82.2993f), 10.0f, "sounds/Coco2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(24.8f, -0.1f, -91.6992f), 10.0f, "sounds/Coco4.wav");
    proximitySoundSystem.addTrigger(glm::vec3(8.69995f, 0.4f, -91.7992f), 10.0f, "sounds/Coco3.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-13.5001f, 0.5f, -36.6f), 25.0f, "sounds/Lion.wav");
    proximitySoundSystem.addTrigger(glm::vec3(26.7f, 2.0f, -15.5f), 20.0f, "sounds/Peac.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-31.7001f, -4.5f, -81.9993f), 20.0f, "sounds/Wolf1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-53.2998f, -4.5f, -81.9993f), 20.0f, "sounds/Wolf2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-19.3001f, -4.5f, -81.9993f), 20.0f, "sounds/Wolf3.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-55.0998f, -4.5f, -85.3993f), 20.0f, "sounds/Wolf4.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-49.5999f, 7.45058e-08f, -41.2999f), 20.0f, "sounds/Capy1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-49.2999f, 0.5f, -32.2001f), 20.0f, "sounds/Capy2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-43.7f, 7.45058e-08f, -52.9997f), 20.0f, "sounds/Capy3.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-107.399f, 0.5, -48.5998f), 15.0f, "sounds/Ele.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-71.3995f, 14.6f, -137.899f), 15.0f, "sounds/Bison1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-32.2001, 2.5, -129.099), 15.0f, "sounds/Bison2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-20.5001f, 9.5f, -107.299f), 15.0f, "sounds/Gira.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-144.0f, -0.1f, -102.099f), 30.0f, "sounds/Velo1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-155.3f, -0.1f, -116.399f), 30.0f, "sounds/Velo2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-111.199f, 2.0f, -11.0f), 20.0f, "sounds/Tigre1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-124.999f, 2.0f, -25.1001f), 20.0f, "sounds/Tigre2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-141.399f, 0.400003f, -34.5f), 20.0f, "sounds/Monk1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-153.9f, 0.100003f, -64.3996f), 20.0f, "sounds/Monk2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-163.601f, 5.7f, -46.9999f), 20.0f, "sounds/Monk3.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-153.5f, 1.1f, -35.1f), 20.0f, "sounds/Monk4.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-157.5f, 0.3f, -64.1996f), 20.0f, "sounds/Monk1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-126.198f, 0.3f, -68.4995f), 20.0f, "sounds/Monk2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-158.6f, 0.3f, -71.1995f), 20.0f, "sounds/Monk3.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-95.2991f, 0.200003f, -11.3f), 20.0f, "sounds/Puma1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-129.399f, 0.200003f, -8.2f), 20.0f, "sounds/Puma2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-48.2998f, 11.9f, -134.499f), 20.0f, "sounds/Bird1.wav");
    proximitySoundSystem.addTrigger(glm::vec3(-83.5993f, 3.3f, -43.5999f), 15.0f, "sounds/Bird2.wav");
    proximitySoundSystem.addTrigger(glm::vec3(0.0999996f, 1.4f, -47.5998f), 20.0f, "sounds/Bird3.wav");

    // --------------------------------------models positions

    //---------------coco position------------------------------

    glm::vec3 cocoPosition = glm::vec3(20.6f, 0.5f, -82.2993f);
    float cocoMoveSpeed = 0.1f;
    static glm::vec3 lastCocoPosition = cocoPosition;
    float cocoYaw = 90.0f;
    static float lastCocoYaw = cocoYaw;

    //---------------------coco2-------------------
    glm::vec3 coco2Position = glm::vec3(15.8f, 0.2f, -105.499f);
    float coco2Yaw = 279.0f;
    //---------------------coco3---------------------------
    glm::vec3 coco3Position = glm::vec3(36.1f, 0.2f, -82.2993f);
    float coco3Yaw = 269.0f;

    //-------------------coco4----------------------------------
    glm::vec3 coco4Position = glm::vec3(24.8f, -0.1f, -91.6992f);
    float coco4Yaw = 269.0f;

    //------------------coco5----------------
    glm::vec3 coco5Position = glm::vec3(8.69995f, 0.4f, -91.7992f);
    float coco5Yaw = 323.0f;


    //--------lion on the rock
    glm::vec3 lionPosition = glm::vec3(-13.5001f, 0.5f, -36.6f); // ajustado a mano
    float lionYaw = 323.0f;

    //-----------peacock
    glm::vec3 peacockPosition = glm::vec3(26.7f, 2.0f, -15.5f);
    float peacockYaw = 258.0f;
    //-------------------wolf
    glm::vec3 wolfPosition = glm::vec3(-31.7001f, -4.5f, -81.9993f);
    float wolfYaw = 190.0f;
    //-----------------femalewolf
    glm::vec3 femaleWolfPosition = glm::vec3(-53.2998f, -4.5f, -81.9993f);
    float femaleWolfYaw = 343.0f;
    //---------------BroWolf
    glm::vec3 BroWolfPosition = glm::vec3(-19.3001f, -4.5f, -81.9993f);
    float BroWolfYaw = 343.0f;
    //------SisWolf
    glm::vec3 SisWolfPosition = glm::vec3(-55.0998f, -4.5f, -85.3993f);
    float SisWolfYaw = 250.0f;

    //capybara
    glm::vec3 capyPosition = glm::vec3(-49.5999f, 7.45058e-08f, -41.2999f);
    float     capyYaw = 119.0f;
    //---------capy2
    glm::vec3 capy2Position = glm::vec3(-49.2999f, 0.5f, -32.2001f);
    float capy2Yaw = 119.0f;

    //--------------------capy3
    glm::vec3 capy3Position = glm::vec3(-43.7f, 7.45058e-08f, -52.9997f);
    float capy3Yaw = 82.0f;
    //-----------------------elephant
    glm::vec3 elephantPosition = glm::vec3(-107.399f, 0.5, -48.5998f);
    float elephantYaw = 119.0f;
    //------------------------biston
    glm::vec3 bistonPosition = glm::vec3(-71.3995f, 14.6f, -137.899f);
    float bistonYaw = 330.0f;
    //-----------------------biston2
    glm::vec3 biston2Position = glm::vec3(-32.2001, 2.5, -129.099);
    float biston2Yaw = 111.0f;
    //-------------------giraffe
    glm::vec3 giraffePosition = glm::vec3(-20.5001f, 9.5f, -107.299f);
    float giraffeYaw = 348.0f;

    // --------------------------------------Generate  Skydome---------------------------------

    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;
    createSphere(5.0f, 64, 64, sphereVertices, sphereIndices);

    unsigned int skyVAO, skyVBO, skyEBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glGenBuffers(1, &skyEBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // texture sky----------------------------------------

    stbi_set_flip_vertically_on_load(true);
    unsigned int skyTexture;
    glGenTextures(1, &skyTexture);
    glBindTexture(GL_TEXTURE_2D, skyTexture);

    // Wrapping: clamp to avoid seams on the edges
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Filtering without mipmaps to avoid sampling at lower levels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texWidth, texHeight, texChannels;
    unsigned char* data = stbi_load(
        "Skybox//HDRI.jpg",
        &texWidth, &texHeight, &texChannels, 0);

    if (data)
    {
        std::cout << "[OK] loaded texture: " << texWidth
            << "x" << texHeight << ", canals: " << texChannels
            << std::endl;

        GLenum format = (texChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(
            GL_TEXTURE_2D, 0, format,
            texWidth, texHeight, 0,
            format, GL_UNSIGNED_BYTE, data
        );
    }
    else
    {
        std::cerr << "[ERROR] cannot load HDRI.jpg" << std::endl;
    }
    stbi_image_free(data);

    bool showTitleScreen = true;


    //main loop------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        camera.Inputs(window);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.updateMatrix(45.0f, 1.0f, 3000.0f);
        proximitySoundSystem.update(camera.Position);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (showTitleScreen)
        {
            // Fullscreen ImGui window (no borders or background)
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(1800, 800), ImGuiCond_Always);

            ImGui::Begin("Title Screen", nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoCollapse);

            // Centered title
            ImGui::SetCursorPos(ImVec2((1800 - ImGui::CalcTextSize("SIMULACION DE ZOOLOGICO").x) / 2, 100));
            ImGui::Text("SIMULACION DE ZOOLOGICO");

            // Centered names
            ImGui::SetCursorPos(ImVec2((1800 - ImGui::CalcTextSize("Gamaliel Joel Gutierrez Perez----------------2023-0731U").x) / 2, 180));
            ImGui::Text("Gutierrez Perez Gamaliel Joel----------------2023-0731U");
            ImGui::SetCursorPos(ImVec2((1800 - ImGui::CalcTextSize("Daney Nohemi Gonzalez Marin------------------2023-0674U").x) / 2, 220));
            ImGui::Text("Gonzalez Marin Daney Nohemi------------------2023-0674U");
            ImGui::SetCursorPos(ImVec2((1800 - ImGui::CalcTextSize("Marco Aurelio Antonio Lopez Gonzalez---------2023-0712U").x) / 2, 260));
            ImGui::Text("Lopez Gonzalez Marco Aurelio Antonio---------2023-0712U");
            ImGui::SetCursorPos(ImVec2((1800 - ImGui::CalcTextSize("Joshua Moises Salgado Pastran----------------2023-0605U").x) / 2, 300));
            ImGui::Text("Salgado Pastran Joshua Moises----------------2023-0605U");

            // Centered instruction
            ImGui::SetCursorPos(ImVec2((1800 - ImGui::CalcTextSize("Press ENTER to start...").x) / 2, 500));
            ImGui::Text("Press ENTER to start...");

            ImGui::End();

            // Input check
            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
                showTitleScreen = false;

        }
            else
            {

                camera.Inputs(window);
                camera.updateMatrix(45.0f, 1.0f, 3000.0f);

                // Allow camera input only if ImGui is not capturing input
                if (!ImGui::GetIO().WantCaptureKeyboard) {
                    // Process keyboard input for camera
                    // Example: camera.ProcessKeyboard(...);
                }

                if (!ImGui::GetIO().WantCaptureMouse) {
                    // Process mouse movement for camera
                    // Example: camera.ProcessMouseMovement(...);
                }




                // ---------------------------------------------Movement coco---------------------------------------------
                if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) cocoPosition.z -= cocoMoveSpeed;
                if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) cocoPosition.z += cocoMoveSpeed;
                if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) cocoPosition.x -= cocoMoveSpeed;
                if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) cocoPosition.x += cocoMoveSpeed;
                if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) cocoPosition.y += cocoMoveSpeed;
                if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) cocoPosition.y -= cocoMoveSpeed;

                // Rotation P-----------------------------------------------------------------------------------
                if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
                {
                    cocoYaw += 1.0f;
                    if (cocoYaw >= 360.0f)
                        cocoYaw -= 360.0f;

                    if (cocoYaw != lastCocoYaw)
                    {
                        std::cout << "Coco ángulo actual (YAW): " << cocoYaw << " grados\n";
                        lastCocoYaw = cocoYaw;
                    }
                }

                // New coco position on console-----------------------------------------------------------------------
                if (cocoPosition != lastCocoPosition)
                {
                    std::cout << "Coco posición actual: ("
                        << cocoPosition.x << ", "
                        << cocoPosition.y << ", "
                        << cocoPosition.z << ")\n";
                    lastCocoPosition = cocoPosition;
                }


                if (cocoPosition != lastCocoPosition)
                {
                    std::cout << "glm::vec3(" << cocoPosition.x << "f, " << cocoPosition.y << "f, " << cocoPosition.z << "f);\n";
                    std::cout << "float yaw = " << cocoYaw << "f;\n";

                    lastCocoPosition = cocoPosition;
                }

                //draw zoo----------------------------------------------------------------------------------------------------
                shaderProgram.Activate();
                shaderProgram.setVec3("viewPos", camera.Position);
                shaderProgram.setMat4("view", camera.GetViewMatrix());
                shaderProgram.setMat4("projection", camera.GetProjectionMatrix());

                glm::mat4 modelMat = glm::mat4(1.0f);
                modelMat = glm::scale(modelMat, glm::vec3(0.01f));
                modelMat = glm::translate(modelMat, glm::vec3(0.0f, -2.0f, 0.0f));
                modelMat = glm::rotate(modelMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                shaderProgram.setMat4("model", modelMat);
                model.Draw(shaderProgram);

                // draw coco-------------------------------------------------------------------------------------------------------------


                glm::mat4 cocoModelMat = glm::mat4(1.0f);
                cocoModelMat = glm::translate(cocoModelMat, cocoPosition);
                cocoModelMat = glm::rotate(cocoModelMat, glm::radians(cocoYaw), glm::vec3(0.0f, 1.0f, 0.0f));
                cocoModelMat = glm::scale(cocoModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", cocoModelMat);
                coco.Draw(shaderProgram);

                //draw coco2-----------------
                glm::mat4 coco2ModelMat = glm::mat4(1.0f);
                coco2ModelMat = glm::translate(coco2ModelMat, coco2Position);
                coco2ModelMat = glm::rotate(coco2ModelMat, glm::radians(coco2Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
                coco2ModelMat = glm::scale(coco2ModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", coco2ModelMat);
                coco2.Draw(shaderProgram);

                //------draw coco3------------
                glm::mat4 coco3ModelMat = glm::mat4(1.0f);
                coco3ModelMat = glm::translate(coco3ModelMat, coco3Position);
                coco3ModelMat = glm::rotate(coco3ModelMat, glm::radians(coco3Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
                coco3ModelMat = glm::scale(coco3ModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", coco3ModelMat);
                coco3.Draw(shaderProgram);

                //-----------draw coco4--------------------
                glm::mat4 coco4ModelMat = glm::mat4(1.0f);
                coco4ModelMat = glm::translate(coco4ModelMat, coco4Position);
                coco4ModelMat = glm::rotate(coco4ModelMat, glm::radians(coco4Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
                coco4ModelMat = glm::scale(coco4ModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", coco4ModelMat);
                coco4.Draw(shaderProgram);

                //draw coco 5--------------

                glm::mat4 coco5ModelMat = glm::mat4(1.0f);
                coco5ModelMat = glm::translate(coco5ModelMat, coco5Position);
                coco5ModelMat = glm::rotate(coco5ModelMat, glm::radians(coco5Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
                coco5ModelMat = glm::scale(coco5ModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", coco5ModelMat);
                coco5.Draw(shaderProgram);

                //--------------lion
             // Base position on the rock and yaw
                glm::vec3 lionPosition = glm::vec3(-13.5001f, 0.5f, -36.6f);
                float lionYaw = 323.0f;

                // Fine adjustment so the paws are right on the blue line
                glm::vec3 lionOffset = glm::vec3(0.0f, 0.20f, 0.40f);

                // 1) World translation + local offset
                glm::mat4 lionModelMat = glm::translate(glm::mat4(1.0f), lionPosition + lionOffset);

                // 2) Rotate from lying down to upright: –90° on X axis
                lionModelMat = glm::rotate(lionModelMat,
                    glm::radians(-120.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f));

                // 3) Rotate “sideways” to face forward: 90° on Z axis
                lionModelMat = glm::rotate(lionModelMat,
                    glm::radians(90.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));

                // 4) Yaw to make it look at the rock
                lionModelMat = glm::rotate(lionModelMat,
                    glm::radians(lionYaw),
                    glm::vec3(0.0f, 1.0f, 0.0f));

                // 5) Final scale
                lionModelMat = glm::scale(lionModelMat, glm::vec3(5.0f));

                // Send to shader and draw
                shaderProgram.setMat4("model", lionModelMat);
                lion.Draw(shaderProgram);


                //------------ - draw peacock
                glm::mat4 peacockModelMat = glm::mat4(1.0f);
                glm::vec3 peacockOffset = glm::vec3(0.0f, 0.0f, 0.0f);

                // 1) Traslación al mundo + offset local
                peacockModelMat = glm::translate(
                    peacockModelMat,
                    peacockPosition + peacockOffset
                );

                // 2) Rotación independiente en X
                peacockModelMat = glm::rotate(
                    peacockModelMat,
                    glm::radians(-90.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );

                // 3) Luego rotación en Y
                peacockModelMat = glm::rotate(
                    peacockModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );

                // 4) Y finalmente rotación en Z
                peacockModelMat = glm::rotate(
                    peacockModelMat,
                    glm::radians(peacockYaw),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );

                // 5) Escala final
                peacockModelMat = glm::scale(
                    peacockModelMat,
                    glm::vec3(2.0f)
                );

                shaderProgram.setMat4("model", peacockModelMat);
                peacock.Draw(shaderProgram);

                //------wolf
                glm::mat4 wolfModelMat = glm::mat4(1.0f);
                glm::vec3 wolfOffset = glm::vec3(0.0f, 0.0f, 0.0f);


                wolfModelMat = glm::translate(
                    wolfModelMat,
                    wolfPosition + wolfOffset
                );


                wolfModelMat = glm::rotate(
                    wolfModelMat,
                    glm::radians(90.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                wolfModelMat = glm::rotate(
                    wolfModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                wolfModelMat = glm::rotate(
                    wolfModelMat,
                    glm::radians(wolfYaw),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                wolfModelMat = glm::scale(
                    wolfModelMat,
                    glm::vec3(3.0f)
                );

                shaderProgram.setMat4("model", wolfModelMat);
                wolf.Draw(shaderProgram);

                //----------------femalewolf
                glm::mat4 femaleWolfModelMat = glm::mat4(1.0f);
                glm::vec3 femaleWolfOffset = glm::vec3(0.0f, 0.0f, 0.0f);


                femaleWolfModelMat = glm::translate(
                    femaleWolfModelMat,
                    femaleWolfPosition + femaleWolfOffset
                );


                femaleWolfModelMat = glm::rotate(
                    femaleWolfModelMat,
                    glm::radians(90.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                femaleWolfModelMat = glm::rotate(
                    femaleWolfModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                femaleWolfModelMat = glm::rotate(
                    femaleWolfModelMat,
                    glm::radians(femaleWolfYaw),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                femaleWolfModelMat = glm::scale(
                    femaleWolfModelMat,
                    glm::vec3(3.0f)
                );

                shaderProgram.setMat4("model", femaleWolfModelMat);
                femaleWolf.Draw(shaderProgram);

                //-----------------draw BroWolf
                glm::mat4 BroWolfModelMat = glm::mat4(1.0f);
                glm::vec3 BroWolfOffset = glm::vec3(0.0f, 0.0f, 0.0f);


                BroWolfModelMat = glm::translate(
                    BroWolfModelMat,
                    BroWolfPosition + BroWolfOffset
                );


                BroWolfModelMat = glm::rotate(
                    BroWolfModelMat,
                    glm::radians(90.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                BroWolfModelMat = glm::rotate(
                    BroWolfModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                BroWolfModelMat = glm::rotate(
                    BroWolfModelMat,
                    glm::radians(BroWolfYaw),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                BroWolfModelMat = glm::scale(
                    BroWolfModelMat,
                    glm::vec3(3.0f)
                );

                shaderProgram.setMat4("model", BroWolfModelMat);
                BroWolf.Draw(shaderProgram);

                //----------------------draw SisWolf
                glm::mat4 SisWolfModelMat = glm::mat4(1.0f);
                glm::vec3 SisWolfOffset = glm::vec3(0.0f, 0.0f, 0.0f);


                SisWolfModelMat = glm::translate(
                    SisWolfModelMat,
                    SisWolfPosition + SisWolfOffset
                );


                SisWolfModelMat = glm::rotate(
                    SisWolfModelMat,
                    glm::radians(90.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                SisWolfModelMat = glm::rotate(
                    SisWolfModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                SisWolfModelMat = glm::rotate(
                    SisWolfModelMat,
                    glm::radians(SisWolfYaw),
                    glm::vec3(0.0f, 0.0f, 10.0f)
                );


                SisWolfModelMat = glm::scale(
                    SisWolfModelMat,
                    glm::vec3(3.0f)
                );

                shaderProgram.setMat4("model", SisWolfModelMat);
                SisWolf.Draw(shaderProgram);


                //------------ - capy
                glm::mat4 capyModelMat = glm::mat4(1.0f);
                glm::vec3 capyOffset = glm::vec3(0.0f);




                capyModelMat = glm::translate(
                    capyModelMat,
                    capyPosition + capyOffset
                );


                capyModelMat = glm::rotate(
                    capyModelMat,
                    glm::radians(0.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                capyModelMat = glm::rotate(
                    capyModelMat,
                    glm::radians(capyYaw),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                capyModelMat = glm::rotate(
                    capyModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                capyModelMat = glm::scale(
                    capyModelMat,
                    glm::vec3(2.0f)
                );

                shaderProgram.setMat4("model", capyModelMat);
                capy.Draw(shaderProgram);

                shaderProgram.setMat4("model", capyModelMat);
                capy.Draw(shaderProgram);


                //---------------capy2
                glm::mat4 capy2ModelMat = glm::mat4(1.0f);
                glm::vec3 capy2Offset = glm::vec3(0.0f);




                capy2ModelMat = glm::translate(
                    capy2ModelMat,
                    capy2Position + capy2Offset
                );


                capy2ModelMat = glm::rotate(
                    capy2ModelMat,
                    glm::radians(0.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                capy2ModelMat = glm::rotate(
                    capy2ModelMat,
                    glm::radians(capyYaw),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                capy2ModelMat = glm::rotate(
                    capy2ModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                capy2ModelMat = glm::scale(
                    capy2ModelMat,
                    glm::vec3(2.0f)
                );

                shaderProgram.setMat4("model", capy2ModelMat);
                capy2.Draw(shaderProgram);

                //-----------------------------capy3
                glm::mat4 capy3ModelMat = glm::mat4(1.0f);
                glm::vec3 capy3Offset = glm::vec3(0.0f);




                capy3ModelMat = glm::translate(
                    capy3ModelMat,
                    capy3Position + capy3Offset
                );


                capy3ModelMat = glm::rotate(
                    capy3ModelMat,
                    glm::radians(0.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                capy3ModelMat = glm::rotate(
                    capy3ModelMat,
                    glm::radians(capy3Yaw),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                capy3ModelMat = glm::rotate(
                    capy3ModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                capy3ModelMat = glm::scale(
                    capy3ModelMat,
                    glm::vec3(2.0f)
                );

                shaderProgram.setMat4("model", capy3ModelMat);
                capy3.Draw(shaderProgram);



                //---------------elephant
                glm::mat4 elephantModelMat = glm::mat4(1.0f);
                glm::vec3 elephantffset = glm::vec3(0.0f, 0.0f, 0.0f);


                elephantModelMat = glm::translate(
                    elephantModelMat,
                    elephantPosition + peacockOffset
                );


                elephantModelMat = glm::rotate(
                    elephantModelMat,
                    glm::radians(-90.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                elephantModelMat = glm::rotate(
                    elephantModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                elephantModelMat = glm::rotate(
                    elephantModelMat,
                    glm::radians(elephantYaw),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                elephantModelMat = glm::scale(
                    elephantModelMat,
                    glm::vec3(0.02f)
                );

                shaderProgram.setMat4("model", elephantModelMat);
                elephant.Draw(shaderProgram);

                //------------------biston

                glm::mat4 bistonModelMat = glm::mat4(1.0f);
                glm::vec3 bistonOffset = glm::vec3(0.0f, 0.0f, 0.0f);




                bistonModelMat = glm::translate(
                    bistonModelMat,
                    bistonPosition + bistonOffset
                );


                bistonModelMat = glm::rotate(
                    bistonModelMat,
                    glm::radians(0.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                bistonModelMat = glm::rotate(
                    bistonModelMat,
                    glm::radians(bistonYaw),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );

                bistonModelMat = glm::rotate(
                    bistonModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                bistonModelMat = glm::scale(
                    bistonModelMat,
                    glm::vec3(0.40f)
                );

                shaderProgram.setMat4("model", bistonModelMat);
                biston.Draw(shaderProgram);

                //----------------------------------biston2
                glm::mat4 biston2ModelMat = glm::mat4(1.0f);
                glm::vec3 biston2Offset = glm::vec3(0.0f, 0.0f, 0.0f);




                biston2ModelMat = glm::translate(
                    biston2ModelMat,
                    biston2Position + biston2Offset
                );


                biston2ModelMat = glm::rotate(
                    biston2ModelMat,
                    glm::radians(0.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                biston2ModelMat = glm::rotate(
                    biston2ModelMat,
                    glm::radians(biston2Yaw),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                biston2ModelMat = glm::rotate(
                    biston2ModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                biston2ModelMat = glm::scale(
                    biston2ModelMat,
                    glm::vec3(0.40f)
                );

                shaderProgram.setMat4("model", biston2ModelMat);
                biston2.Draw(shaderProgram);

                //----------------------giraffe
                glm::mat4 giraffeModelMat = glm::mat4(1.0f);
                glm::vec3 giraffeOffset = glm::vec3(0.0f, 0.0f, 0.0f);




                giraffeModelMat = glm::translate(
                    giraffeModelMat,
                    giraffePosition + giraffeOffset
                );


                giraffeModelMat = glm::rotate(
                    giraffeModelMat,
                    glm::radians(-90.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );


                giraffeModelMat = glm::rotate(
                    giraffeModelMat,
                    glm::radians(0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );


                giraffeModelMat = glm::rotate(
                    giraffeModelMat,
                    glm::radians(giraffeYaw),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );


                giraffeModelMat = glm::scale(
                    giraffeModelMat,
                    glm::vec3(0.10f)
                );

                shaderProgram.setMat4("model", giraffeModelMat);
                giraffe.Draw(shaderProgram);

                //**********************
                 // --- Draw raptor---
                glm::mat4 velociraptor2ModelMat = glm::mat4(1.0f);
                velociraptor2ModelMat = glm::translate(velociraptor2ModelMat, glm::vec3(-144.0f, -0.1f, -102.099f));
                velociraptor2ModelMat = glm::rotate(velociraptor2ModelMat, glm::radians(47.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                velociraptor2ModelMat = glm::scale(velociraptor2ModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", velociraptor2ModelMat);
                velociraptor.Draw(shaderProgram);

                // --- Draw raptor---
                glm::mat4 velociraptorModelMat = glm::mat4(1.0f);
                velociraptorModelMat = glm::translate(velociraptorModelMat, glm::vec3(-155.3f, -0.1f, -116.399f));
                velociraptorModelMat = glm::rotate(velociraptorModelMat, glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                velociraptorModelMat = glm::scale(velociraptorModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", velociraptorModelMat);
                velociraptor.Draw(shaderProgram);

                //tiger
                glm::mat4 tigreModelMat = glm::mat4(1.0f);
                tigreModelMat = glm::translate(tigreModelMat, glm::vec3(-111.199f, 2.0f, -11.0f));
                tigreModelMat = glm::rotate(tigreModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                tigreModelMat = glm::rotate(tigreModelMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                tigreModelMat = glm::rotate(tigreModelMat, glm::radians(357.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                tigreModelMat = glm::scale(tigreModelMat, glm::vec3(0.002f));
                shaderProgram.setMat4("model", tigreModelMat);
                tigre.Draw(shaderProgram);

                //tiger2
                glm::mat4 tigre2ModelMat = glm::mat4(1.0f);
                tigre2ModelMat = glm::translate(tigre2ModelMat, glm::vec3(-124.999f, 2.0f, -25.1001f));
                tigre2ModelMat = glm::rotate(tigre2ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                tigre2ModelMat = glm::rotate(tigre2ModelMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                tigre2ModelMat = glm::rotate(tigre2ModelMat, glm::radians(357.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                tigre2ModelMat = glm::scale(tigre2ModelMat, glm::vec3(0.002f));
                shaderProgram.setMat4("model", tigre2ModelMat);
                tigre2.Draw(shaderProgram);

                //monkey1
                glm::mat4 mono1ModelMat = glm::mat4(1.0f);
                mono1ModelMat = glm::translate(mono1ModelMat, glm::vec3(-141.399f, 0.400003f, -34.5f));
                mono1ModelMat = glm::rotate(mono1ModelMat, glm::radians(255.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mono1ModelMat = glm::rotate(mono1ModelMat, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mono1ModelMat = glm::rotate(mono1ModelMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mono1ModelMat = glm::scale(mono1ModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", mono1ModelMat);
                mono1.Draw(shaderProgram);

                //monkey2
                glm::mat4 mono2ModelMat = glm::mat4(1.0f);
                mono2ModelMat = glm::translate(mono2ModelMat, glm::vec3(-153.9f, 0.100003f, -64.3996f));
                mono2ModelMat = glm::rotate(mono2ModelMat, glm::radians(103.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mono2ModelMat = glm::rotate(mono2ModelMat, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mono2ModelMat = glm::rotate(mono2ModelMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mono2ModelMat = glm::scale(mono2ModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", mono2ModelMat);
                mono2.Draw(shaderProgram);

                //monkey3
                glm::mat4 mono3ModelMat = glm::mat4(1.0f);
                mono3ModelMat = glm::translate(mono3ModelMat, glm::vec3(-163.601f, 5.7f, -46.9999f));
                mono3ModelMat = glm::rotate(mono3ModelMat, glm::radians(279.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mono3ModelMat = glm::rotate(mono3ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mono3ModelMat = glm::rotate(mono3ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mono3ModelMat = glm::scale(mono3ModelMat, glm::vec3(0.1f));
                shaderProgram.setMat4("model", mono3ModelMat);
                mono3.Draw(shaderProgram);

                //monkey4
                glm::mat4 mono4ModelMat = glm::mat4(1.0f);
                mono4ModelMat = glm::translate(mono4ModelMat, glm::vec3(-153.5f, 1.1f, -35.1f));
                mono4ModelMat = glm::rotate(mono4ModelMat, glm::radians(53.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mono4ModelMat = glm::rotate(mono4ModelMat, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mono4ModelMat = glm::rotate(mono4ModelMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mono4ModelMat = glm::scale(mono4ModelMat, glm::vec3(1.0f));
                shaderProgram.setMat4("model", mono4ModelMat);
                mono4.Draw(shaderProgram);

                //monkey5
                glm::mat4 mono5ModelMat = glm::mat4(1.0f);
                mono5ModelMat = glm::translate(mono5ModelMat, glm::vec3(-157.5f, 0.3f, -64.1996f));
                mono5ModelMat = glm::rotate(mono5ModelMat, glm::radians(25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mono5ModelMat = glm::rotate(mono5ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mono5ModelMat = glm::rotate(mono5ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mono5ModelMat = glm::scale(mono5ModelMat, glm::vec3(2.0f));
                shaderProgram.setMat4("model", mono5ModelMat);
                mono5.Draw(shaderProgram);

                //mono6
                glm::mat4 mono6ModelMat = glm::mat4(1.0f);
                mono6ModelMat = glm::translate(mono6ModelMat, glm::vec3(-126.198f, 0.3f, -68.4995f));
                mono6ModelMat = glm::rotate(mono6ModelMat, glm::radians(305.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mono6ModelMat = glm::rotate(mono6ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mono6ModelMat = glm::rotate(mono6ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mono6ModelMat = glm::scale(mono6ModelMat, glm::vec3(2.0f));
                shaderProgram.setMat4("model", mono6ModelMat);
                mono6.Draw(shaderProgram);

                //monkey7
                glm::mat4 mono7ModelMat = glm::mat4(1.0f);
                mono7ModelMat = glm::translate(mono7ModelMat, glm::vec3(-158.6f, 0.3f, -71.1995f));
                mono7ModelMat = glm::rotate(mono7ModelMat, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mono7ModelMat = glm::rotate(mono7ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                mono7ModelMat = glm::rotate(mono7ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                mono7ModelMat = glm::scale(mono7ModelMat, glm::vec3(2.0f));
                shaderProgram.setMat4("model", mono7ModelMat);
                mono7.Draw(shaderProgram);

                //puma
                glm::mat4 pumaModelMat = glm::mat4(1.0f);
                pumaModelMat = glm::translate(pumaModelMat, glm::vec3(-95.2991f, 0.200003f, -11.3f));
                pumaModelMat = glm::rotate(pumaModelMat, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                pumaModelMat = glm::rotate(pumaModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                pumaModelMat = glm::rotate(pumaModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                pumaModelMat = glm::scale(pumaModelMat, glm::vec3(0.04f));
                shaderProgram.setMat4("model", pumaModelMat);
                puma.Draw(shaderProgram);

                //puma2
                glm::mat4 puma2ModelMat = glm::mat4(1.0f);
                puma2ModelMat = glm::translate(puma2ModelMat, glm::vec3(-129.399f, 0.200003f, -8.2f));
                puma2ModelMat = glm::rotate(puma2ModelMat, glm::radians(302.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                puma2ModelMat = glm::rotate(puma2ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                puma2ModelMat = glm::rotate(puma2ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                puma2ModelMat = glm::scale(puma2ModelMat, glm::vec3(0.04f));
                shaderProgram.setMat4("model", puma2ModelMat);
                puma2.Draw(shaderProgram);

                //bird
                glm::mat4 birdModelMat = glm::mat4(1.0f);
                birdModelMat = glm::translate(birdModelMat, glm::vec3(-48.2998f, 11.9f, -134.499f));
                birdModelMat = glm::rotate(birdModelMat, glm::radians(162.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                birdModelMat = glm::rotate(birdModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                birdModelMat = glm::rotate(birdModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                birdModelMat = glm::scale(birdModelMat, glm::vec3(0.8f));
                shaderProgram.setMat4("model", birdModelMat);
                bird.Draw(shaderProgram);

                //bird2
                glm::mat4 bird2ModelMat = glm::mat4(1.0f);
                bird2ModelMat = glm::translate(bird2ModelMat, glm::vec3(-83.5993f, 3.3f, -43.5999f));
                bird2ModelMat = glm::rotate(bird2ModelMat, glm::radians(158.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                bird2ModelMat = glm::rotate(bird2ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                bird2ModelMat = glm::rotate(bird2ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                bird2ModelMat = glm::scale(bird2ModelMat, glm::vec3(0.5f));
                shaderProgram.setMat4("model", bird2ModelMat);
                bird2.Draw(shaderProgram);

                //bird3
                glm::mat4 bird3ModelMat = glm::mat4(1.0f);
                bird3ModelMat = glm::translate(bird3ModelMat, glm::vec3(0.0999996f, 1.4f, -47.5998f));
                bird3ModelMat = glm::rotate(bird3ModelMat, glm::radians(126.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                bird3ModelMat = glm::rotate(bird3ModelMat, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                bird3ModelMat = glm::rotate(bird3ModelMat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                bird3ModelMat = glm::scale(bird3ModelMat, glm::vec3(0.5f));
                shaderProgram.setMat4("model", bird3ModelMat);
                bird3.Draw(shaderProgram);
                // Draw SkyDome-----------------------------------------------------------------------------------------
                glDepthFunc(GL_LEQUAL);
                skyShader.Activate();

                glm::mat4 domeModel = glm::translate(glm::mat4(1.0f), camera.Position);
                domeModel = glm::scale(domeModel, glm::vec3(-500.0f));  // radio
                skyShader.setMat4("model", domeModel);
                skyShader.setMat4("view", camera.GetViewMatrix());
                skyShader.setMat4("projection", camera.GetProjectionMatrix());

                glActiveTexture(GL_TEXTURE0);                      // <<< 
                skyShader.setInt("skyTexture", 0);                 // <<< 

                glBindTexture(GL_TEXTURE_2D, skyTexture);

                glBindVertexArray(skyVAO);
                glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(sphereIndices.size()), GL_UNSIGNED_INT, 0);
                glDepthFunc(GL_LESS);
                TwDraw();

            }

            
    

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        
        


        glfwSwapBuffers(window);
        glfwPollEvents();

    }


    TwDeleteBar(barCamera);
    TwDeleteBar(barInfo);
    TwTerminate();


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    shaderProgram.Delete();
    skyShader.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

