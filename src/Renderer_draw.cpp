#include "Renderer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

SDL_Surface* Renderer::screen;
bool Renderer::loadedFonts = false;
TTF_Font* Renderer::fonts[3];
int Renderer::fontHeight[3];


//a utility function to convert an int (usually defined as hex) to the SDL color struct
SDL_Color getSDLColor(int& color)
{
	char* c = (char*)(&color);

	SDL_Color ret;
	ret.r = *(c + 2);
	ret.g = *(c + 1);
	ret.b = *(c + 0);

	return ret;
}

void Renderer::drawRect(int x, int y, int h, int w, int color)
{
	SDL_Rect rect = {x, y, h, w};
	SDL_FillRect(Renderer::screen, &rect, color);
}

bool Renderer::loadFonts()
{
	std::string fontPath = "LinLibertine_R.ttf";

	//if our font isn't foud, make a last-ditch effort to load a system font
	if(!boost::filesystem::exists(fontPath))
	{
		std::cerr << "Error - " << fontPath << " font not found. Falling back to ";
		fontPath = "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf";
		std::cerr << fontPath << " (which may not exist).\n";
	}

	int sizeArray[] = {Renderer::getScreenWidth() * 0.015, Renderer::getScreenWidth() * 0.03, Renderer::getScreenWidth() * 0.062};


	//the three here should be the font count but, again, I don't remember the syntax
	for(unsigned int i = 0; i < 3; i++)
	{
		TTF_Font* font = TTF_OpenFont(fontPath.c_str(), sizeArray[i]);
		if(!font)
		{
			std::cerr << "Error - could not load font!\n";
			std::cerr << TTF_GetError() << "\n";
			return false;
		}
		fonts[i] = font;
		TTF_SizeText(font, "HEIGHT", NULL, &fontHeight[i]); //gets the height of the string "HEIGHT" in this font (in pixels)
	}

	loadedFonts = true;
	return true;
}

int Renderer::getFontHeight(FontSize size)
{
	if(!loadedFonts)
		loadFonts();

	return fontHeight[size];
}

void Renderer::drawText(std::string text, int x, int y, int color, FontSize fontsize)
{
	if(!loadedFonts)
		loadFonts();

	TTF_Font* font = fonts[fontsize];

	//SDL_Color is a struct of four bytes, with the first three being colors. An int is four bytes.
	//So, we can just pretend the int is an SDL_Color.
	//SDL_Color* sdlcolor = (SDL_Color*)&color;
	SDL_Color sdlcolor = getSDLColor(color);

	SDL_Surface* textSurf = TTF_RenderText_Blended(font, text.c_str(), sdlcolor);
	if(textSurf == NULL)
	{
		std::cerr << "Error - could not render text \"" << text << "\" to surface!\n";
		std::cerr << TTF_GetError() << "\n";
		return;
	}

	SDL_Rect dest = {x, y};
	SDL_BlitSurface(textSurf, NULL, screen, &dest);
	SDL_FreeSurface(textSurf);
}

void Renderer::drawCenteredText(std::string text, int xOffset, int y, int color, FontSize fontsize)
{
	if(!loadedFonts)
		loadFonts();

	TTF_Font* font = fonts[fontsize];

	int w, h;
	TTF_SizeText(font, text.c_str(), &w, &h);

	int x = (int)getScreenWidth() - w;
	x *= 0.5;

	x += xOffset * 0.5;

	drawText(text, x, y, color, fontsize);
}

//this could probably be optimized
//draws text and ensures it's never longer than xLen
void Renderer::drawWrappedText(std::string text, int xStart, int yStart, int xLen, int color, FontSize fontsize)
{
	if(!loadedFonts)
		loadFonts();

	TTF_Font* font = fonts[fontsize];

	int y = yStart;

	std::string line, word, temp;
	int w, h;
	size_t space, newline;

	while(text.length() > 0 || !line.empty()) //while there's text or we still have text to render
	{
		space = text.find(' ', 0);
		if(space == std::string::npos)
			space = text.length() - 1;


		word = text.substr(0, space + 1);

		//check if the next word contains a newline
		newline = word.find('\n', 0);
		if(newline != std::string::npos)
		{
			word = word.substr(0, newline);
			text.erase(0, newline + 1);
		}else{
			text.erase(0, space + 1);
		}

		temp = line + word;

		TTF_SizeText(font, temp.c_str(), &w, &h);

		//if we're on the last word and it'll fit on the line, just add it to the line
		if((w <= xLen && text.length() == 0) || newline != std::string::npos)
		{
			line = temp;
			word = "";
		}


		//if the next line will be too long or we're on the last of the text, render it
		if(w > xLen || text.length() == 0 || newline != std::string::npos)
		{
			//render line now
			if(w > 0) //make sure it's not blank
				drawText(line, xStart, y, color, fontsize);

			//increment y by height and some extra padding for the next line
			y += h + 4;

			//move the word we skipped to the next line
			line = word;
		}else{
			//there's still space, continue building the line
			line = temp;
		}

	}
}