//#include "AudioHelper.h"
#include "Engine.h"
#include <iostream>
//#include "SDL2/SDL.h"


void CppDebugLog(std::string message) {
	std::cout << message << std::endl;
}

int main(int argc, char* argv[]) {
	//_putenv_s("RENDERLOGGER", "1");  // Windows-specific
	Engine e;
	e.Run();


	return 0;
}
