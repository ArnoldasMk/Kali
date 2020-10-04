    #pragma once
#include "../interfaces.h"
#include "../Utils/draw.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../Utils/bonemaps.h"
#include "../Utils/xorstring.h"
#include "../Hooks/hooks.h"
     
    /* pasted from aaron, thanks <3. (actually pasted from some random uc thread)*/
     
    class dot {
    public:
    	dot( Vector2D p, Vector2D v ) {
    		m_vel = v;
    		m_pos = p;
    	}
     
    	void update( );
    	void draw( );
     
    	Vector2D m_pos, m_vel;
    };
     
    extern void dot_draw( );
    extern void dot_destroy( );
