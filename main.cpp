//OpenGL Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Header definitions
#define STB_IMAGE_IMPLEMENTATION


#include <imgui_tools.h>
#include <stb_image_write.h>

//GUI Headers
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>


//Standard Headers
#include <iostream>
#include <ctime>
#include <Windows.h>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadImageGUI(string const &path);


// settings
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;


using namespace std;

/*int CALLBACK WinMain(
	__in  HINSTANCE hInstance,
	__in  HINSTANCE hPrevInstance,
	__in  LPSTR lpCmdLine,
	__in  int nCmdShow
)*/
int main(){
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, true);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "VText",NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, NULL);
	glfwSwapInterval(1);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}



	//GUI

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsClassic();

	//Load Save State
	itStateModifier::loadState();
	
	//Background Image
	unsigned int backgroundImage = loadImageGUI("resources\\Background Images\\wave.jpg");

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	// -----------
	float start = glfwGetTime();
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while (!glfwWindowShouldClose(window))
	{
		

		//MAIN BUFFER
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		


		//GUI 
		ImVec2 guiSize = ImGui::GetIO().DisplaySize;

		//GUI MENU BAR
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		static int c = 0;
		static bool filedialog = false;
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New","Ctrl-N")) {
					itFileDialog::enableFD = true;
				}
				if (ImGui::MenuItem("Save","Ctrl-S")) {
					itTextView::SaveTextToFile(itFileDialogView::file);
				}
				if (ImGui::MenuItem("Save as")) {

				}
				if (ImGui::MenuItem("Delete")) {
					itSystemFileModifer::deleteFile(itFileDialogView::file);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {
					glfwSetWindowShouldClose(window, true);
				}
				
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				ImGui::MenuItem("Console", NULL, &itConsole::enableConsole);
				ImGui::MenuItem("File Chooser", NULL, &itFileDialogView::enableFDV);
				ImGui::Separator();
				if(ImGui::MenuItem("Code Editor", NULL)){
					if (itTextView::textViewFormat != 1) {
						itTextView::switchTextView(1);
					}
				}
				if (ImGui::MenuItem("Note Editor", NULL)) {
					if (itTextView::textViewFormat != 0) {
						itTextView::switchTextView(0);
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools")) {
				if (ImGui::MenuItem("Run/Compile File")) {
					itConsole::compile(itFileDialogView::file);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		//Background Image
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(guiSize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("", NULL, ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::Image((ImTextureID)(backgroundImage), guiSize);
		ImGui::PopStyleVar();
		ImGui::End();

		//OPTIONS
		ImGui::SetNextWindowBgAlpha(0.3f);
		ImGui::SetNextWindowSize(ImVec2(guiSize.x, 50));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0.0f);
		ImGui::Begin("Frame",NULL,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar );
		ImGui::SetWindowPos(ImVec2(0, 18));
		static unsigned int currentView = itTextView::textViewFormat;
		if (ImGui::RadioButton("Text Editor", &itTextView::textViewFormat, 0)) {
			if (currentView == 1) {
				itTextView::switchTextView(0);
				currentView = 0;
			}
		}; ImGui::SameLine();
		if (ImGui::RadioButton("Code Editor", &itTextView::textViewFormat, 1)) {
			if (currentView == 0) {
				itTextView::switchTextView(1);
				currentView = 1;
			}
		}ImGui::SameLine();
		ImGui::PopStyleVar();
		ImGui::End();

		//UI SCALING
		ImVec2 textViewSize(guiSize.x, guiSize.y - 70);
		ImVec2 textViewPos(0, 70);
		ImVec2 consoleViewSize(guiSize.x, guiSize.y / 4.25);
		ImVec2 consoleViewPos(0, guiSize.y - guiSize.y / 4.25);
		if (itConsole::enableConsole) {
			textViewSize.y -= guiSize.y / 4.25;
		}
		if (itFileDialogView::enableFDV) {
			textViewSize.x -= guiSize.x / 5;
			textViewPos.x = guiSize.x / 5;
			consoleViewSize.x -= guiSize.x / 5;
			consoleViewPos.x = guiSize.x / 5;
		}

		//UI 
		itFileDialogView::FileDialogView(ImVec2(guiSize.x / 5, guiSize.y - 70), ImVec2(0, 70));
		itTextView::NoteTextView(textViewSize,textViewPos);
		itTextView::CodeEditorTextView(ImVec2(textViewSize.x,textViewSize.y), textViewPos);
		itTextView::LoadTextFromFile(itFileDialogView::file);
		itConsole::consoleView(consoleViewSize,consoleViewPos);
		itFileDialog::FileDialog();

		//User Inputs
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			itTextView::SaveTextToFile(itFileDialogView::file);
		}

		
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//Save State
	itStateModifier::saveState();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------


unsigned int loadImageGUI(string const &path) {
	int width;
	int height;
	int nrChannels;
	unsigned int textureID;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return textureID;
}