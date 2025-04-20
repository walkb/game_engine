#pragma once
#include <string>
#include <unordered_map>
#include "Actor.h"
#include "ImageDB.h"
#include "rapidjson/document.h"
class TemplateDB
{
public:
	static bool CheckTemplateExists(std::string template_name);
	static void LoadTemplates();
	inline static std::unordered_map<std::string, rapidjson::Document> templates;
};

