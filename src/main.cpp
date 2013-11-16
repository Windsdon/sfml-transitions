/*
 * main.cpp
 *
 *  Created on: 15/11/2013
 *      Author: Windsdon
 */

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

using namespace sf;
using namespace std;

class Transition {
	public:
		virtual bool isComplete() const = 0;
		virtual void render(const Texture *from, const Texture *to, RenderTarget *destination, double elapsed) = 0;
};

class TransitionFade: public Transition{
	public:
		TransitionFade(double duration): complete(false), duration(duration){

		}

		virtual bool isComplete() const {
			return complete;
		}

		virtual void render(const Texture *from, const Texture *to, RenderTarget *destination, double elapsed){
			Sprite toSprite(*to);
			Sprite fromSprite(*from);

			fromSprite.setColor(Color(255, 255, 255, 255 * (1 - elapsed/duration)));

			destination->draw(toSprite);
			destination->draw(fromSprite);
		}

	private:
		bool complete;
		double duration;

		TransitionFade(TransitionFade&);
};

class TransitionFadeBlocks: public Transition {
	public:
		TransitionFadeBlocks(double duration): TransitionFadeBlocks(duration, 16, 9){
		}

		TransitionFadeBlocks(double duration, int piecesX, int piecesY): complete(false), duration(duration), piecesX(piecesX), piecesY(piecesY){
			diagCount = piecesX + piecesY - 2;
			band = 1;
		}

		virtual bool isComplete() const {
			return complete;
		}

		virtual void render(const Texture *from, const Texture *to, RenderTarget *destination, double elapsed){
			Sprite toSprite(*to);
			Sprite fromSprite(*from);

			int blockW = from->getSize().x / piecesX;
			int blockH = from->getSize().y / piecesY;

			destination->draw(toSprite);

			double e = 1 - elapsed/duration;

			for (int i = 0; i < piecesX; ++i) {
				for (int j = 0; j < piecesY; ++j) {
					int k = diagCount - (i + j);
					double q = e*(diagCount - 1) - k;

					if(q < 0){
						q = 0;
					}else if(q > 1){
						q = 1;
					}

					fromSprite.setPosition(i * blockW, j * blockH);
					fromSprite.setTextureRect(
							Rect<int>(i * blockW, j * blockH, blockW, blockH));
					fromSprite.setColor(Color(255, 255, 255, 255 * q));

					destination->draw(fromSprite);
				}
			}
		}

	private:
		bool complete;
		double duration;
		int piecesX;
		int piecesY;
		int diagCount;
		int band;

		TransitionFadeBlocks(TransitionFadeBlocks&);
};

class TransitionBlackout: public Transition {
	public:
		TransitionBlackout(double duration): duration(duration), complete(false){

		}

		virtual bool isComplete() const {
			return complete;
		}


		virtual void render(const Texture *from, const Texture *to, RenderTarget *destination, double elapsed){
			double e = elapsed / duration;

			destination->clear(Color::Black);

			if(e < 0.5){
				Sprite fromSprite(*from);
				fromSprite.setColor(Color(255, 255, 255, 500*(0.5-e)));
				destination->draw(fromSprite);
			}else{
				Sprite toSprite(*to);
				toSprite.setColor(Color(255, 255, 255, 500*(e - 0.5)));
				destination->draw(toSprite);
			}
		}


	private:
		double duration;
		bool complete;

		TransitionBlackout(TransitionBlackout&);
};

int main(int argc, char **argv) {
	RenderWindow window(VideoMode(1280, 720), "Transition", Style::Close);
	window.setVerticalSyncEnabled(true);

	RenderTexture source;
	RenderTexture destination;
	source.create(1280, 720, false);
	destination.create(1280, 720, false);

	Clock time;
	time.restart();

	Clock transitionTimer;

	RectangleShape blue(Vector2f(1280, 720));
	blue.setFillColor(Color::Blue);

	RectangleShape green(Vector2f(1280, 720));
	green.setFillColor(Color::Green);

	TransitionBlackout fade(0.5);

	bool running = true;
	bool hasSwapped = false;
	while(running){

		Event e;
		while(window.pollEvent(e)){
			switch(e.type){
				case Event::Closed:
					running = false;
					break;
			}
		};

		window.clear();

		if(time.getElapsedTime() < seconds(1)){
			window.draw(blue);
		}else{
			if(!hasSwapped){
				transitionTimer.restart();
				hasSwapped = true;
			}

			if(transitionTimer.getElapsedTime() < seconds(0.5)){
				source.clear();
				destination.clear();
				source.draw(blue);
				destination.draw(green);
				fade.render(&(source.getTexture()), &(destination.getTexture()), &window, transitionTimer.getElapsedTime().asSeconds());
			}else{
				if(transitionTimer.getElapsedTime() < seconds(2)){
					window.draw(green);
				}else{
					hasSwapped = false;
					time.restart();
					//running = false;
				}
			}
		}

		window.display();
	}
}

