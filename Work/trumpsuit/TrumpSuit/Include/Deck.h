#pragma once

#include "Hand.h"


// A deck is hand composed of the standard 52 playing cards.
class CDeck	:	public CHand
{
public:
	// Shuffles the cards.
	void Shuffle();

private:
	// Required by Shuffle.
	int GenerateRandom(int iRange);

public:
	CDeck();
	~CDeck();
};