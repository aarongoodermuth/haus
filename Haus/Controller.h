#pragma once

#include "View.h"

class Controller
{
public:
	Controller::Controller(View* pview);

private:
	const View* _pview;
};

