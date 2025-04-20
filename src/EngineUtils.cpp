#include "EngineUtils.h"

void EngineUtils::ReadJsonFile(const std::string& path, rapidjson::Document& out_document) {
	FILE* file_pointer = nullptr;
#ifdef _WIN32
	fopen_s(&file_pointer, path.c_str(), "rb");
#else
	file_pointer = fopen(path.c_str(), "rb");
#endif
	char buffer[65536];
	rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
	out_document.ParseStream(stream);
	std::fclose(file_pointer);

	if (out_document.HasParseError()) {
		rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
		std::cout << "error parsing json at [" << path << "]" << std::endl << errorCode << std::endl;
		exit(0);
	}
}

bool EngineUtils::CheckPathExists(const std::string& path) {
	if (std::filesystem::exists(path)) {
		return true;
	}
	return false;
}

uint64_t EngineUtils::create_composite_key(const float x, const float y, const float REGION_SIZE) {
	uint32_t ux = static_cast<uint32_t>(x / REGION_SIZE);
	uint32_t uy = static_cast<uint32_t>(y / REGION_SIZE);

	uint64_t result = static_cast<uint64_t>(ux);
	result = result << 32;

	result = result | static_cast<uint64_t>(uy);

	return result;
}

std::string EngineUtils::obtain_word_after_phrase(const std::string& input, const std::string& phrase) {
	size_t pos = input.find(phrase);

	if (pos == std::string::npos) return "";

	pos += phrase.length();
	while (pos < input.size() && std::isspace(input[pos])) {
		++pos;
	}
	if (pos == input.size()) return "";

	size_t endPos = pos;
	while (endPos < input.size() && !std::isspace(input[endPos])) {
		++endPos;
	}

	return input.substr(pos, endPos - pos);
}

ActorComp EngineUtils::actorcomp;

bool ActorComp::operator()(Actor* a, Actor* b) const {
	return a->id < b->id;
}

void EngineUtils::ReportError(const std::string& actor_name, const luabridge::LuaException& e) {
	std::string error_message = e.what();

	// Normalize file paths across platforms
	std::replace(error_message.begin(), error_message.end(), '\\', '/');

	// Display (with color codes)
	std::cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << std::endl;
}