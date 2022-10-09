#include "ASpell.hpp"

ASpell::ASpell() {}
ASpell::ASpell(ASpell const &other) {
	*this = other;
}
ASpell *ASpell::operator=(ASpell const &other) {
	this->name = other.name;
	this->effects = other.effects;
	return *this;
}
ASpell::~ASpell() {}
ASpell::ASpell(std::string const &name, std::string const &effects) {
	this->name = name;
	this->effects = effects;
}

void ASpell::launch(ATarget const &atarget_ref) const {
	atarget_ref.getHitBySpell(*this);
}

