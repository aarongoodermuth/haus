#pragma once

#include <optional>
#include "Cards.h"

class View
{
public:
	virtual void Show(std::optional<Suit> optsuit) = 0;
};

