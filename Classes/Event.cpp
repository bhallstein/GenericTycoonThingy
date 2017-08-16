#include "Event.hpp"

Event::Event() {
	// Constructor
}
Event::~Event() {
	// Destructor
}

void Event::loadFromSFEvent(sf::Event *sf_event) {
	if (sf_event->Type == sf::Event::Closed) {
		type = CLOSED;
	}
	else if (sf_event->Type == sf::Event::KeyPressed) {
		type = KEYPRESS;
		key  = keyFromSFKeyCode(sf_event->Key.Code);
	}
	else if (sf_event->Type == sf::Event::MouseMoved) {
		type = MOUSEMOVE;
		x = sf_event->MouseMove.X;
		y = sf_event->MouseMove.Y;
	}
	else if (sf_event->Type == sf::Event::MouseButtonPressed) {
		type = sf_event->MouseButton.Button == sf::Mouse::Left ? LEFTCLICK : RIGHTCLICK;
		x = sf_event->MouseButton.X;
		y = sf_event->MouseButton.Y;
	}
	else {
		type = UNKNOWNEVENT;
	}
}

void Event::convertCoords(int block_width, int block_height) {
	a = (float) (x%block_width) / (float) block_width;
	b = (float) (y%block_height) / (float) block_height;
	x = x/block_width;
	y = y/block_height;
}


// This is a horrendous function to convert SFML key codes to DBT’s own.
// At some point a swap-out of SFML will obviate the need for it.
dbt_event_code keyFromSFKeyCode(sf::Keyboard::Key sf_key_code) {
	dbt_event_code k;
	switch(sf_key_code) {
		case sf::Keyboard::A:
			k = K_A;
			break;
		case sf::Keyboard::B:
			k = K_B;
			break;
		case sf::Keyboard::C:
			k = K_C;
			break;
		case sf::Keyboard::D:
			k = K_D;
			break;
		case sf::Keyboard::E:
			k = K_E;
			break;
		case sf::Keyboard::F:
			k = K_F;
			break;
		case sf::Keyboard::G:
			k = K_G;
			break;
		case sf::Keyboard::H:
			k = K_H;
			break;
		case sf::Keyboard::I:
			k = K_I;
			break;
		case sf::Keyboard::J:
			k = K_J;
			break;
		case sf::Keyboard::K:
			k = K_K;
			break;
		case sf::Keyboard::L:
			k = K_L;
			break;
		case sf::Keyboard::M:
			k = K_M;
			break;
		case sf::Keyboard::N:
			k = K_N;
			break;
		case sf::Keyboard::O:
			k = K_O;
			break;
		case sf::Keyboard::P:
			k = K_P;
			break;
		case sf::Keyboard::Q:
			k = K_Q;
			break;
		case sf::Keyboard::R:
			k = K_R;
			break;
		case sf::Keyboard::S:
			k = K_S;
			break;
		case sf::Keyboard::T:
			k = K_T;
			break;
		case sf::Keyboard::U:
			k = K_U;
			break;
		case sf::Keyboard::V:
			k = K_V;
			break;
		case sf::Keyboard::W:
			k = K_W;
			break;
		case sf::Keyboard::X:
			k = K_X;
			break;
		case sf::Keyboard::Y:
			k = K_Y;
			break;
		case sf::Keyboard::Z:
			k = K_Z;
			break;
		case sf::Keyboard::Num0:
			k = K_0;
			break;
		case sf::Keyboard::Num1:
			k = K_1;
			break;
		case sf::Keyboard::Num2:
			k = K_2;
			break;
		case sf::Keyboard::Num3:
			k = K_3;
			break;
		case sf::Keyboard::Num4:
			k = K_4;
			break;
		case sf::Keyboard::Num5:
			k = K_5;
			break;
		case sf::Keyboard::Num6:
			k = K_6;
			break;
		case sf::Keyboard::Num7:
			k = K_7;
			break;
		case sf::Keyboard::Num8:
			k = K_8;
			break;
		case sf::Keyboard::Num9:
			k = K_9;
			break;
		case sf::Keyboard::Escape:
			k = K_ESC;
			break;
		case sf::Keyboard::Return:
			k = K_RETURN;
			break;
		default:
			k = K_OTHER;
			break;
	}
	return k;
}
