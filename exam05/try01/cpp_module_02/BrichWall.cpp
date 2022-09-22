#include "BrickWall.hpp"

BrickWall::BrickWall(): ATarget("Inconspicous Red-birck Wall") {}

BrickWall::~BrickWall() {}

ATarget *BrickWall::clone() const {
	return (new BrickWall());
}
