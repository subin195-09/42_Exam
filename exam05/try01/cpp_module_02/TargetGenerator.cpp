#include "TargetGenerator.hpp"

TargetGenerator::TargetGenerator() {

}

TargetGenerator::~TargetGenerator() {
	std::map<std::string, ATarget *>::iterator it;

	for (it = this->arr_target.begin(); it != this->arr_target.end(); ++it) {
		delete it->second;
	}
	arr_target.clear();
}

void TargetGenerator::learnTargetType(ATarget *target_ptr) {
	if (target_ptr) {
		this->arr_target.insert(std::pair<std::string, ATarget *>(target_ptr->getType(), target_ptr->clone()));
	}
}

void TargetGenerator::forgetTargetType(std::string const &name) {
	std::map<std::string, ATarget *>::iterator it = arr_target.find(name);

	if (it != this->arr_target.end()) {
		delete it->second;
	}
	this->arr_target.erase(name);
}

ATarget *TargetGenerator::createTarget(std::string const &name) {
	std::map<std::string, ATarget *>::iterator it = arr_target.find(name);

	if (it != this->arr_target.end()) {
		return arr_target[name];
	}
	return NULL;
}

