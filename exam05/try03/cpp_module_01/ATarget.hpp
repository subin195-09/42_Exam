#pragma once

#include <iostream>

class ASpell;

class ATarget {
	private:
		std::string type;
	public:
		ATarget();
		ATarget(ATarget const &other);
		virtual ~ATarget();
        ATarget &operator=(ATarget const &other);
		ATarget(std::string const &type);

		std::string const &getType() const;

		void getHitBySpell(ASpell const &aspell_ref) const;

		virtual ATarget *clone() const = 0;

};

#include "ASpell.hpp"
