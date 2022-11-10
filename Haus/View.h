#pragma once

#include <optional>
#include "Cards.h"

class View
{
public:
	virtual void Show() = 0;
};


class NoView : public View
{
public:
	void Show() override {}
};

