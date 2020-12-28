#ifndef CAR_H_
#define CAR_H_

#include <Godot.hpp>
#include <Sprite.hpp>

namespace godot {

class car : public Sprite {
	
	GODOT_CLASS(car, Sprite)

public:
	static void _register_methods();

	car();
	~car();

	void _init();

	void _process(float delta);
};

}

#endif
