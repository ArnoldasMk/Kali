#include "fonts.h"

#include "Utils/draw.h"
#include "settings.h"
#include "interfaces.h"

HFont esp_font = 0;
HFont icon_font;
HFont astrium;

static HFont CreateFont( const char* fontName, int size, int flag ) {
	HFont newFont = surface->CreateFont();
	surface->SetFontGlyphSet( newFont, fontName, size, 0, 0, 0, flag );
	return newFont;
}

void Fonts::SetupFonts()
{
	esp_font = CreateFont("Verdana",
								Settings::UI::Fonts::ESP::size,
								Settings::UI::Fonts::ESP::flags);
icon_font = CreateFont("menu_font", 25, 0x80);
astrium = CreateFont("AstriumWep", 25, 0x010 | 0x080);
}

