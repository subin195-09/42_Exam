#include "Warlock.hpp"

Warlock::Warlock(std::string const &name, std::string const &title) {
	this->name = name;
	this->title = title;
	std::cout << this->name << ": This looks like another boring day.\n";
}

Warlock::~Warlock() {
	std::cout << this->name << ": My job here is done!\n";

	std::map<std::string, ASpell *>::iterator it_begin = arr.begin();
	std::map<std::string, ASpell *>::iterator it_end = arr.end();

	if (it_begin != it_end) {
		delete it_begin->second;
		it_begin++;
	}
	arr.clear();
}

std::string const &Warlock::getName() const {
	return this->name;
}

std::string const &Warlock::getTitle() const {
	return this->title;
}

void Warlock::setTitle(std::string const  &title) {
	this->title = title;
}

void Warlock::introduce() const {
	std::cout << this->name << ": I am " << this->name << ", " << this->title << "!\n";
}

void Warlock::learnSpell(ASpell *aspell_ptr) {
	if (aspell_ptr) {
		arr.insert(std::pair<std::string, ASpell *>(aspell_ptr->getName(), aspell_ptr->clone()));
	}
}

void Warlock::forgetSpell(std::string name) {
	std::map<std::string, ASpell *>::iterator it = arr.find(name);

	if (it != arr.end()) {
		delete it->second;
	}
	arr.erase(name);
}

void Warlock::launchSpell(std::string name, ATarget const &atarget_ref) {
	ASpell *aspell = arr[name];

	if (aspell) {
		aspell->launch(atarget_ref);
	}
}
