#ifndef MAIN_H
#define MAIN_H

#ifndef ARU_VERSION_MAJOR
	#define ARU_VERSION_MAJOR 0
#endif

#ifndef ARU_VERSION_MINOR
	#define ARU_VERSION_MINOR 0
#endif

#ifndef ARU_VERSION_PATCH
	#define ARU_VERSION_PATCH 0
#endif

#ifndef ARU_BUILD_STABLE
	#define ARU_BUILD_STABLE 0
#endif

#if ARU_BUILD_STABLE == 0
	#define ARU_BUILD_TYPE "dev"
#else
	#define ARU_BUILD_TYPE "stable"
#endif

#define VERSION_STR "v" + std::to_string(ARU_VERSION_MAJOR) + \
						"." + std::to_string(ARU_VERSION_MINOR) + \
						"." + std::to_string(ARU_VERSION_PATCH) + \
						"-" + ARU_BUILD_TYPE


#endif // MAIN_H