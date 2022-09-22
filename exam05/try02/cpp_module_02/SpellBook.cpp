#include "SpellBook.hpp"

SpellBook::SpellBook() {}
SpellBook::~SpellBook() {
	std::map<std::string, ASpell *>::iterator it_begin = arr.begin();
	std::map<std::string, ASpell *>::iterator it_end = arr.end();

	while (it_begin != it_end) {
		delete it_begin->second;
		it_begin++;
	}
	arr.clear();
}

void SpellBook::learnSpell(ASpell *aspell_ptr) {
	if (aspell_ptr) {
		arr.insert(std::pair<std::string, ASpell *>(aspell_ptr->getName(), aspell_ptr->clone()));
	}
}

void SpellBook::forgetSpell(std::string const &name) {
	std::map<std::string, ASpell *>::iterator it = arr.find(name);

	if (it != arr.end()) {
		delete it->second;
	}
	arr.erase(name);
}

ASpell *SpellBook::createSpell(std::string const &name) {
	std::map<std::string, ASpell *>::iterator it = arr.find(name);

	if (it != arr.end()) {
		return arr[name];
	}
	return NULL;
}
