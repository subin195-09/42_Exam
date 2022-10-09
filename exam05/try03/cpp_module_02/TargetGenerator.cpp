#include "TargetGenerator.hpp"

ATarget *TargetGenerator::createTarget(std::string const &name) {
	std::map<std::string, ATarget *>::iterator it = arr.find(name);

	if (it != arr.end()) {
		return arr[name];
	}
	return NULL;
}
