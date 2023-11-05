#pragma once

namespace NCL::CSC3222 {
	class State {
	public:
		virtual void Update(float time) = 0;
	};
}
