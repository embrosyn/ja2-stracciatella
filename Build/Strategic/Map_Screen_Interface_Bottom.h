#ifndef __MAP_INTERFACE_BOTTOM
#define __MAP_INTERFACE_BOTTOM

#include "Button_System.h"
#include "Types.h"


#define MAX_MESSAGES_ON_MAP_BOTTOM  9
#define MESSAGE_BTN_SCROLL_TIME 100


enum ExitToWhere
{
	MAP_EXIT_TO_INVALID  = -1,
	MAP_EXIT_TO_LAPTOP   =  0,
	MAP_EXIT_TO_TACTICAL =  1,
	MAP_EXIT_TO_OPTIONS  =  2,
	MAP_EXIT_TO_LOAD,
	MAP_EXIT_TO_SAVE
};

static inline ExitToWhere operator ++(ExitToWhere& a)
{
	return a = (ExitToWhere)(a + 1);
}


// there's no button for entering SAVE/LOAD screen directly...
extern GUIButtonRef guiMapBottomExitButtons[3];

extern BOOLEAN fLapTop;
extern BOOLEAN fLeavingMapScreen;
extern BOOLEAN gfDontStartTransitionFromLaptop;
extern BOOLEAN gfStartMapScreenToLaptopTransition;


// function prototypes

void LoadMapScreenInterfaceBottom(void);
void DeleteMapScreenInterfaceBottom( void );
void RenderMapScreenInterfaceBottom( void );

// delete map bottom graphics
void DeleteMapBottomGraphics( void );

// load bottom graphics
void HandleLoadOfMapBottomGraphics( void );
GUIButtonRef MakeArrowButton( const INT32 grayed, const INT32 off, const INT32 on, const INT16 x, const INT16 y, const GUI_CALLBACK click, const wchar_t* const help );

// allowed to time compress?
BOOLEAN AllowedToTimeCompress( void );

void EnableDisAbleMapScreenOptionsButton( BOOLEAN fEnable );

// create and destroy masks to cover the time compression buttons as needed
void CreateDestroyMouseRegionMasksForTimeCompressionButtons();

BOOLEAN CommonTimeCompressionChecks( void );

bool AnyUsableRealMercenariesOnTeam();

void    RequestTriggerExitFromMapscreen(ExitToWhere);
BOOLEAN AllowedToExitFromMapscreenTo(ExitToWhere);
void    HandleExitsFromMapScreen(void);

void MapScreenMsgScrollDown( UINT8 ubLinesDown );
void MapScreenMsgScrollUp( UINT8 ubLinesUp );

void ChangeCurrentMapscreenMessageIndex( UINT8 ubNewMessageIndex );
void MoveToEndOfMapScreenMessageList( void );


// the dirty state of the mapscreen interface bottom
extern BOOLEAN fMapScreenBottomDirty;

#endif
