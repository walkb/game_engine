#include "TemplateDB.h"
#include "EngineUtils.h"


bool TemplateDB::CheckTemplateExists(std::string template_name) {
	if (templates.find(template_name) == templates.end()) {
		return false;
	}
	return true;
}


// Loads all templates in the directory
void TemplateDB::LoadTemplates() {
	const std::string path = "./resources/actor_templates";
	if (EngineUtils::CheckPathExists(path)) {
		for (auto const& dir_entry : std::filesystem::directory_iterator(path)) {
			// Load template
			rapidjson::Document tmp = rapidjson::Document();
			EngineUtils::ReadJsonFile(dir_entry.path().string(), tmp);
			std::string tmpname = dir_entry.path().filename().stem().string();
			templates.emplace(tmpname, std::move(tmp));
		}
	}
}