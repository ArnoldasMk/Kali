//
// FGUI - Surface Renderer (Source Engine)
//

#pragma once

#define FGUI_IMPLEMENTATION
#include "FGUI.hpp"
#include "../interfaces.h"
// NOTE: make sure to use your surface interface pointer
// replace "ISurface" with your pointer

namespace FGUI_SURFACE
{

  inline void CreateFont(FGUI::FONT& _font, std::string _family, int _size, int _flags, bool _bold)
  {
    _font = surface->CreateFont();
    surface->SetFontGlyphSet(_font, _family.c_str(), _size, _bold ? 800 : 0, 0, 0, _flags);
  }

  inline FGUI::DIMENSION GetScreenSize()
  {
    static FGUI::DIMENSION dmTemporarySize = { 0, 0 };

    surface->GetScreenSize(dmTemporarySize.m_iWidth, dmTemporarySize.m_iHeight);

    return dmTemporarySize;
  }

  inline FGUI::DIMENSION GetTextSize(FGUI::FONT _font, std::string _text)
  {
    static FGUI::DIMENSION dmTemporarySize = { 0, 0 };

    surface->GetTextSize(_font, std::wstring(_text.begin(), _text.end()).c_str(), dmTemporarySize.m_iWidth, dmTemporarySize.m_iHeight);

    return dmTemporarySize;
  }

  inline void Rectangle(int _x, int _y, int _width, int _height, FGUI::COLOR _color)
  {
    surface->DrawSetColor(_color.m_ucRed, _color.m_ucGreen, _color.m_ucBlue, _color.m_ucAlpha);
    surface->DrawFilledRect(_x, _y, _x + _width, _y + _height);
  }

  inline void Outline(int _x, int _y, int _width, int _height, FGUI::COLOR _color)
  {
    surface->DrawSetColor(_color.m_ucRed, _color.m_ucGreen, _color.m_ucBlue, _color.m_ucAlpha);
    surface->DrawOutlinedRect(_x, _y, _x + _width, _y + _height);
  }

  inline void Gradient(int _x, int _y, int _width, int _height, FGUI::COLOR _color1, FGUI::COLOR _color2, bool _horizontal)
  {
    surface->DrawSetColor(_color1.m_ucRed, _color1.m_ucGreen, _color1.m_ucBlue, _color1.m_ucAlpha);
    surface->DrawFilledRectFade(_x, _y, _x + _width, _y +_height, 255, 255, _horizontal);

    surface->DrawSetColor(_color2.m_ucRed, _color2.m_ucGreen, _color2.m_ucBlue, _color2.m_ucAlpha);
    surface->DrawFilledRectFade(_x, _y, _x + _width, _y + _height, 0, 255, _horizontal);
  }

  inline void Line(int _from_x, int _from_y, int _to_x, int _to_y, FGUI::COLOR _color)
  {
    surface->DrawSetColor(_color.m_ucRed, _color.m_ucGreen, _color.m_ucBlue, _color.m_ucAlpha);
    surface->DrawLine(_from_x, _from_y, _to_x, _to_y);
  }

  inline void Text(int _x, int _y, FGUI::FONT _font, FGUI::COLOR _color, std::string _text)
  {
    surface->DrawColoredText(_font, _x, _y, _color.m_ucRed, _color.m_ucGreen, _color.m_ucBlue, _color.m_ucAlpha, _text.c_str());
  }

  // NOTE: call this function only once (preferably when you initialize your application)
  inline void OnEntryPoint()
  {
    FGUI::RENDER.CreateFont = FGUI_SURFACE::CreateFont;
    FGUI::RENDER.GetScreenSize = FGUI_SURFACE::GetScreenSize;
    FGUI::RENDER.GetTextSize = FGUI_SURFACE::GetTextSize;
    FGUI::RENDER.Rectangle = FGUI_SURFACE::Rectangle;
    FGUI::RENDER.Outline = FGUI_SURFACE::Outline;
    FGUI::RENDER.Line = FGUI_SURFACE::Line;
    FGUI::RENDER.Text = FGUI_SURFACE::Text;
    FGUI::RENDER.Gradient = FGUI_SURFACE::Gradient;
  }
}
