#include <string>

#include "imgui-1.88/imgui.h"
#include "imgui-1.88/imgui_impl_sdl.h"
#include "imgui-1.88/imgui_impl_sdlrenderer.h"
#include "sdl2_wrapper.h"
#include "log/log.h"

#include "main.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int WindowDims[2] = { 700, 500 };

bool AppCloseRequested = false;

int main(int argc, char** argv) {
	atexit(FreeEverything);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		log_error("failed to initialize SDL2: %s", SDL_GetError());
		return -1;
	}

	{
		SDL_version compiled;
		SDL_version linked;

		SDL_VERSION(&compiled);
		SDL_GetVersion(&linked);
		log_info("Compiled With SDL version %u.%u.%u", compiled.major, compiled.minor, compiled.patch);
		log_info("Linked With SDL version %u.%u.%u", linked.major, linked.minor, linked.patch);

		SDL_DisplayMode dm;
		SDL_GetCurrentDisplayMode(0, &dm);
		WindowDims[0] = dm.w * 0.7;
		WindowDims[1] = dm.h * 0.8;
	}

	window = SDL_CreateWindow(
		WINDOW_TITLE_CSTR,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WindowDims[0], WindowDims[1],
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN
	);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		SDL_Log("Error creating SDL_Renderer: %s", SDL_GetError());
		return -1;
	}

	{
		SDL_RendererInfo rendererInfo;
		SDL_GetRendererInfo(renderer, &rendererInfo);
		log_info("initialized app with %s renderer!", rendererInfo.name);
	}

	log_info("aru version: %s\n", (VERSION_STR).c_str());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// ImGuiStyle& style = ImGui::GetStyle();
	io.IniFilename = NULL;

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	ImVec4 EditorBG = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

	SDL_ShowWindow(window);

	while (!AppCloseRequested) {
		ProcessEvents();

		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Render();
		SDL_SetRenderDrawColor(renderer, (Uint8)(EditorBG.x * 255), (Uint8)(EditorBG.y * 255), (Uint8)(EditorBG.z * 255), (Uint8)(EditorBG.w * 255));
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		SDL_RenderPresent(renderer);
	}

	return 0;
}

static inline void ProcessEvents(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			AppCloseRequested = true;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
				AppCloseRequested = true;
			}
			break;
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		case SDL_MOUSEWHEEL:
			break;
		case SDL_MOUSEBUTTONUP:
			break;
		case SDL_MOUSEBUTTONDOWN:
			break;
		case SDL_MOUSEMOTION:
			break;
		}
	}
}

void FreeEverything(void) {
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	if (renderer != NULL) { SDL_DestroyRenderer(renderer); renderer = NULL; }
	if (window != NULL) { SDL_DestroyWindow(window); window = NULL; }

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	SDL_Quit();
}
