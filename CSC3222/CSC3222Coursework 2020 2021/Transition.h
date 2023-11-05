#pragma once

#include "State.h"

namespace NCL::CSC3222 {
	class Transition {
	public:
		virtual bool CanTransition() = 0;
		State* sourceState;
		State* destState;
	};
}