#include <SDL.h>

#include "Error.hpp"

void Error::report(const std::string &text)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", text.c_str(), nullptr);
}