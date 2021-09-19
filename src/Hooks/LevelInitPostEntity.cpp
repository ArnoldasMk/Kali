#include "hooks.h"

#include "../interfaces.h"
#include "../Utils/util.h"

typedef void (* LevelInitPostEntityFn)( void );

/* Called when loading a new level */
void Hooks::LevelInitPostEntity( ) {

    clientVMT->GetOriginalMethod<LevelInitPostEntityFn>( 6 )();
}