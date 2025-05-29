#include"Model.h"


const unsigned int width = 800;
const unsigned int height = 800;


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Burunya", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, width, height);

	Shader shaderProgram("default.vert", "default.frag");

	glm::vec4 lightColor = glm::vec4(1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 5.0f));

	// Load models
	Model model1("models/necoarc2/scene.gltf");
	Model model2("models/necoarc/scene.gltf");
	Model model3("models/cirno/scene.gltf");


	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		shaderProgram.Activate();

		// ----- MODEL 1 -----
		glm::mat4 modelMatrix1 = glm::mat4(1.0f);
		model1.Draw(shaderProgram, camera,
			glm::vec3(-2.0f, 0.0f, 0.0f),
			glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec3(1.5f));



		// ----- MODEL 2 -----
		glm::mat4 modelMatrix2 = glm::mat4(1.0f);
		model2.Draw(shaderProgram, camera,
			glm::vec3(2.0f, 0.0f, 0.0f),
			glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec3(0.5f));


		glm::mat4 modelMatrix3 = glm::mat4(1.0f);
		model3.Draw(shaderProgram, camera,
			glm::vec3(2.0f, 10.0f, 0.0f),
			glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
			glm::vec3(0.5f));


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}