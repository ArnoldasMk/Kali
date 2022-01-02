#include "particles.h"
#include "../interfaces.h"
#include "../Utils/draw.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../Utils/bonemaps.h"
#include "../Utils/xorstring.h"
#include "../Hooks/hooks.h"

std::vector<dot *> dots = {};

void dot::update()
{
        auto opacity = Settings::UI::mainColor.Color().Value.w / 255.0f;

        m_pos += m_vel * (opacity);
}

void dot::draw()
{
        int opacity = 55.0f * (Settings::UI::mainColor.Color().Value.w / 255.0f);

        // Draw::AddRectFilled( m_pos.x - 2, m_pos.y - 2, 2, 2, ImColor( opacity, 255, 255, 255 ) );

        auto t = std::find(dots.begin(), dots.end(), this);
        if (t == dots.end())
        {
                return;
        }

        for (auto i = t; i != dots.end(); i++)
        {
                if ((*i) == this)
                        continue;

                auto dist = (m_pos - (*i)->m_pos).Length();

                if (dist < 128)
                {
                        int alpha = opacity * (dist / 128);
                        Draw::AddLine(m_pos.x - 1, m_pos.y - 2, (*i)->m_pos.x - 2, (*i)->m_pos.y - 1, ImColor(alpha, 255, 255, 255));
                }
        }
}
void dot_draw()
{
        struct screen_size
        {
                int x, y;
        };
        screen_size sc;
        // engine->GetScreenSize(sc.x,sc.y);
        sc.x = 0;
        sc.y = 0;

        int s = rand() % 24;

        if (s == 0)
        {
                dots.push_back(new dot(Vector2D(rand() % (int)sc.x, -16), Vector2D((rand() % 7) - 3, rand() % 3 + 1)));
        }
        else if (s == 1)
        {
                dots.push_back(new dot(Vector2D(rand() % (int)sc.x, (int)sc.y + 16), Vector2D((rand() % 7) - 3, -1 * (rand() % 3 + 1))));
        }
        else if (s == 2)
        {
                dots.push_back(new dot(Vector2D(-16, rand() % (int)sc.y), Vector2D(rand() % 3 + 1, (rand() % 7) - 3)));
        }
        else if (s == 3)
        {
                dots.push_back(new dot(Vector2D((int)sc.x + 16, rand() % (int)sc.y), Vector2D(-1 * (rand() % 3 + 1), (rand() % 7) - 3)));
        }

        auto alph = 135.0f * (Settings::UI::mainColor.Color().Value.w / 255.0f);
        auto a_int = (int)(alph);

        // Draw::AddRectFilled( 0, 0, sc.x, sc.y, ImColor( a_int, 0, 0, 0 ) );

        for (auto i = dots.begin(); i < dots.end();)
        {
                if ((*i)->m_pos.y < -20 || (*i)->m_pos.y > sc.y + 20 || (*i)->m_pos.x < -20 || (*i)->m_pos.x > sc.x + 20)
                {
                        delete (*i);
                        i = dots.erase(i);
                }
                else
                {
                        (*i)->update();
                        i++;
                }
        }

        for (auto i = dots.begin(); i < dots.end(); i++)
        {
                (*i)->draw();
        }
}

void dot_destroy()
{
        for (auto i = dots.begin(); i < dots.end(); i++)
        {
                delete (*i);
        }

        dots.clear();
}
