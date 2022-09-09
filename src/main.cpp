#include <string>

#include "imgui-1.88/imgui.h"
#include "imgui-1.88/imgui_impl_sdl.h"
#include "imgui-1.88/imgui_impl_sdlrenderer.h"
#include "sdl2_wrapper.h"
#include "log/log.h"

#include "main.h"

float TitleBarHeight = 40.0f;
float StatusBarHeight = 40.0f;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int WindowDims[2] = { 700, 500 };

bool AppCloseRequested = false;

int main(int argc, char** argv) {
	atexit(_FreeEverything);

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

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	SDL_EnableScreenSaver();
	SDL_AddEventWatch(_EventWatcher, window);

#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR /* Available since 2.0.8 */
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 12)
	// This hint tells SDL to allow the user to resize a borderless windoow, It also enables aero-snap on Windows apparently.
	SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "1");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 8)
	// This hint tells SDL to respect borderless window as a normal window, For example - the window will sit right on top of the taskbar instead of obscuring it.
	SDL_SetHint("SDL_BORDERLESS_WINDOWED_STYLE", "1");
#endif

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
		_ProcessEvents();

		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		_BuildGui();

		ImGui::Render();
		SDL_SetRenderDrawColor(renderer, (Uint8)(EditorBG.x * 255), (Uint8)(EditorBG.y * 255), (Uint8)(EditorBG.z * 255), (Uint8)(EditorBG.w * 255));
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		SDL_RenderPresent(renderer);
	}

	return 0;
}

static inline void _BuildGui(void) {
	ImGui::Begin("aru title bar###TitleBar", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos({ 0.0f, 0.0f });
	ImGui::SetWindowSize({ (float)WindowDims[0], TitleBarHeight });

	ImGui::Text("this is title bar text!");

	ImGui::End();

	ImGui::Begin("aru workspace###Workspace", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos({ 0.0f, TitleBarHeight });
	ImGui::SetWindowSize({ (float)WindowDims[0], WindowDims[1] - StatusBarHeight - TitleBarHeight });

	ImGui::Text("this is main workspace!");

	ImGui::End();

	ImGui::Begin("aru status bar###StatusBar", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos({ 0.0f, (float)WindowDims[1] - StatusBarHeight });
	ImGui::SetWindowSize({ (float)WindowDims[0], StatusBarHeight });

	ImGui::Text("this is status bar text!");

	ImGui::End();
}

static inline void _ProcessEvents(void) {
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

static inline int _EventWatcher(void* data, SDL_Event* event) {
	switch (event->type) {
		case SDL_WINDOWEVENT: {
			switch (event->window.event) {
				case SDL_WINDOWEVENT_RESIZED: {
					SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
					if (win == (SDL_Window*)data) {
						SDL_GetWindowSize(win, &WindowDims[0], &WindowDims[1]);
					}
					break;
				}
			}
			break;
		}
		case SDL_DROPFILE: {
			char* filePath = event->drop.file;
			if (filePath != NULL) {
				log_info("file dropped: %s", filePath);
				SDL_free(filePath);
			}

			break;
		}
	}
	return 0;
}

int WriteScreenDataToImg(SDL_Renderer* ren, const char* path) {
	int h = 0, w = 0;
	if (SDL_GetRendererOutputSize(ren, &w, &h) != 0) {
		log_error("cannot get renderer output size: %s", SDL_GetError());
		return -1;
	}

	SDL_Surface* sshot = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
	SDL_SaveBMP(sshot, path);
	SDL_FreeSurface(sshot);
	return 0;
}

void _FreeEverything(void) {
	WriteScreenDataToImg(renderer, "screenshot-before-close.bmp");

	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	if (renderer != NULL) { SDL_DestroyRenderer(renderer); renderer = NULL; }
	if (window != NULL) { SDL_DestroyWindow(window); window = NULL; }

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	SDL_Quit();
}
