#ifndef MAIN_H
#define MAIN_H

#include <string>

void _FreeEverything(void);
static inline void _ProcessEvents(void);
static inline void _BuildGui(void);
static inline int _EventWatcher(void* data, SDL_Event* event);

int WriteScreenDataToImg(SDL_Renderer* ren, const char* path);

#ifndef ARU_VMAJOR
	#define ARU_VMAJOR 0
#endif

#ifndef ARU_VMINOR
	#define ARU_VMINOR 0
#endif

#ifndef ARU_VPATCH
	#define ARU_VPATCH 0
#endif

#ifndef ARU_BUILD_STABLE
	#define ARU_BUILD_STABLE 0
#endif

#if ARU_BUILD_STABLE == 0
	#define ARU_BUILD_TYPE "dev"
#else
	#define ARU_BUILD_TYPE "stable"
#endif

#define VERSION_STR "v" + std::to_string(ARU_VMAJOR) + \
						"." + std::to_string(ARU_VMINOR) + \
						"." + std::to_string(ARU_VPATCH) + \
						"-" + ARU_BUILD_TYPE


#define WINDOW_TITLE_CSTR (               \
		std::string("aru ") + VERSION_STR \
	).c_str()

#endif // MAIN_H