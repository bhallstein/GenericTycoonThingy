#ifndef MrPaths_h
#define MrPaths_h

#define MRPATHS_MAX_PATH 300
#define TYCOON_NAME "Generic Tycoon Thingy"

#include <string>

class MrPaths {
public:
  static std::string desktopPath;
  static std::string settingsPath;
  static std::string resourcesPath;

  struct init;
  static struct init *initializer;
};

#endif
