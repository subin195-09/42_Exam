#include "TargetGenerator.hpp"

TargetGenerator::TargetGenerator() {}

TargetGenerator::~TargetGenerator() {
	std::map<std::string, ATarget *>::iterator it_begin = arr.begin();
	std::map<std::string, ATarget *>::iterator it_end = arr.begin();

	while (it_begin != it_end) {
		delete it_begin->second;
		it_begin++;
	}
	arr.clear();
}

void TargetGenerator::learnTargetType(ATarget *target_ptr) {
	if (target_ptr) {
		arr.insert(std::pair<std::string, ATarget *>(target_ptr->getType(), target_ptr->clone()));
	}
}

void TargetGenerator::forgetTargetType(std::string const &name) {
	std::map<std::string, ATarget *>::iterator it = arr.find(name);

	if (it != arr.end()) {
		delete it->second;
	}
	arr.erase(name);
}

ATarget *TargetGenerator::createTarget(std::string const &name) {
	std::map<std::string, ATarget *>::iterator it = arr.find(name);

	if (it != arr.end()) {
		return arr[name];
	}
	return NULL;
}
