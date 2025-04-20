#ifndef ENGINEUTILS_H
#define ENGINEUTILS_H
#endif

#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <chrono>

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "Actor.h"

struct ActorComp {
	bool operator()(Actor* a, Actor* b) const;
};

class EngineUtils {
public:
	static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document);
	
	static bool CheckPathExists(const std::string& path);

	static uint64_t create_composite_key(const float x, const float y, const float REGION_SIZE);

	static ActorComp actorcomp;

	static std::string obtain_word_after_phrase(const std::string& input, const std::string& phrase);

	static void ReportError(const std::string& actor_name, const luabridge::LuaException& e);
};

