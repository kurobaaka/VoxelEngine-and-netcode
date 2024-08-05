#include <SFML/Graphics.hpp>

using namespace sf;

int main() {
	RenderWindow win(VideoMode(900, 900), "SFML Window");

	while(win.isOpen()) {
		Event ev;
		while (win.pollEvent(ev)) {
			if (ev.type == Event::Closed)
				win.close();
		}
		win.clear();
		win.display();
	}
}