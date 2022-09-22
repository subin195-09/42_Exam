#pragma once

#include <iostream>

class ATarget;

class ASpell {
	private:
		std::string name;
		std::string effects;
	public:
		ASpell();
		ASpell(ASpell const &other);
		ASpell *operator=(ASpell const &other);
		virtual ~ASpell();
		ASpell(std::string const &name, std::string const &effects);

		std::string const &getName() const;
		std::String const &getEffects() const;

		void launch(ATarget const &atarget_ref) const;

		virtual ASpell *clone() const = 0;

};

#include "ATarget.hpp"
