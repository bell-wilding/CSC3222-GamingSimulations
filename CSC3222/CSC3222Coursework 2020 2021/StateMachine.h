#pragma once

#include <vector>
#include "Transition.h"

namespace NCL::CSC3222 {
	using namespace std;

	class StateMachine {
	public:
		StateMachine() {};
		~StateMachine() {};

		virtual void Update(float time) {
			activeState->Update(time);
			for (Transition* t : allTransitions) {
				if (t->sourceState != activeState) {
					continue;
				}
				if (t->CanTransition()) {
					activeState = t->destState;
					break;
				}
			}
		}

	protected:
		State* activeState = nullptr;
		vector<State*> allStates;
		vector<Transition*> allTransitions;
	};
}