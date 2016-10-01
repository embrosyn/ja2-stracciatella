#include <exception>

#include "Directories.h"
#include "Font.h"
#include "GameLoop.h"
#include "HImage.h"
#include "Local.h"
#include "Timer_Control.h"
#include "Types.h"
#include "SaveLoadScreen.h"
#include "Video.h"
#include "Font_Control.h"
#include "Game_Clock.h"
#include "Render_Dirty.h"
#include "Text_Input.h"
#include "SaveLoadGame.h"
#include "WordWrap.h"
#include "StrategicMap.h"
#include "Finances.h"
#include "Cursors.h"
#include "VObject.h"
#include "Merc_Hiring.h"
#include "LaptopSave.h"
#include "Options_Screen.h"
#include "GameVersion.h"
#include "SysUtil.h"
#include "Overhead.h"
#include "GameScreen.h"
#include "GameSettings.h"
#include "Fade_Screen.h"
#include "English.h"
#include "Game_Init.h"
#include "Sys_Globals.h"
#include "Text.h"
#include "Message.h"
#include "Map_Screen_Interface.h"
#include "Map_Screen_Interface_Bottom.h"
#include "Multi_Language_Graphic_Utils.h"
#include "Campaign_Types.h"
#include "Button_System.h"
#include "Debug.h"
#include "JAScreens.h"
#include "VSurface.h"
#include "FileMan.h"
#include "Campaign_Init.h"
#include "UILayout.h"

#if defined JA2BETAVERSION
#include "Soldier_Init_List.h"
#endif

#include "ContentManager.h"
#include "GameInstance.h"

#define		SAVE_LOAD_TITLE_FONT								FONT14ARIAL
#define		SAVE_LOAD_TITLE_COLOR								FONT_MCOLOR_WHITE

#define		SAVE_LOAD_NORMAL_FONT								FONT12ARIAL
#define		SAVE_LOAD_NORMAL_COLOR							2//FONT_MCOLOR_DKWHITE//2//FONT_MCOLOR_WHITE
#define		SAVE_LOAD_NORMAL_SHADOW_COLOR				118//121//118//125
/*#define		SAVE_LOAD_NORMAL_FONT								FONT12ARIAL
#define		SAVE_LOAD_NORMAL_COLOR							FONT_MCOLOR_DKWHITE//2//FONT_MCOLOR_WHITE
#define		SAVE_LOAD_NORMAL_SHADOW_COLOR				2//125
*/

#define		SAVE_LOAD_QUICKSAVE_COLOR						2//FONT_MCOLOR_DKGRAY//FONT_MCOLOR_WHITE
#define		SAVE_LOAD_QUICKSAVE_SHADOW_COLOR		189//248//2

#define		SAVE_LOAD_EMPTYSLOT_COLOR						2//125//FONT_MCOLOR_WHITE
#define		SAVE_LOAD_EMPTYSLOT_SHADOW_COLOR		121//118

#define		SAVE_LOAD_HIGHLIGHTED_COLOR					FONT_MCOLOR_WHITE
#define		SAVE_LOAD_HIGHLIGHTED_SHADOW_COLOR	2

#define		SAVE_LOAD_SELECTED_COLOR						2//145//FONT_MCOLOR_WHITE
#define		SAVE_LOAD_SELECTED_SHADOW_COLOR			130//2



#define		SAVE_LOAD_NUMBER_FONT								FONT12ARIAL
#define		SAVE_LOAD_NUMBER_COLOR							FONT_MCOLOR_WHITE

#define		SLG_SELECTED_COLOR									FONT_MCOLOR_WHITE
#define		SLG_UNSELECTED_COLOR								FONT_MCOLOR_DKWHITE

#define		SLG_SAVELOCATION_WIDTH							605
#define		SLG_SAVELOCATION_HEIGHT							30//46
#define		SLG_FIRST_SAVED_SPOT_X							(STD_SCREEN_X + 17)
#define		SLG_FIRST_SAVED_SPOT_Y							(STD_SCREEN_Y + 49)
#define		SLG_GAP_BETWEEN_LOCATIONS						35//47



#define		SLG_DATE_OFFSET_X										13
#define		SLG_DATE_OFFSET_Y										11

#define		SLG_SECTOR_OFFSET_X									95//105//114
#define		SLG_SECTOR_WIDTH										98

#define		SLG_NUM_MERCS_OFFSET_X							196//190//SLG_DATE_OFFSET_X

#define		SLG_BALANCE_OFFSET_X								260//SLG_SECTOR_OFFSET_X

#define		SLG_SAVE_GAME_DESC_X								318//320//204
#define		SLG_SAVE_GAME_DESC_Y								SLG_DATE_OFFSET_Y//SLG_DATE_OFFSET_Y + 7

#define		SLG_TITLE_POS_X											(STD_SCREEN_X)
#define		SLG_TITLE_POS_Y											(STD_SCREEN_Y)

#define SLG_SAVE_CANCEL_POS_X   (226 + STD_SCREEN_X)
#define SLG_LOAD_CANCEL_POS_X   (329 + STD_SCREEN_X)
#define SLG_SAVE_LOAD_BTN_POS_X (123 + STD_SCREEN_X)
#define SLG_BTN_POS_Y           (438 + STD_SCREEN_Y)

#define		SLG_SELECTED_SLOT_GRAPHICS_NUMBER		3
#define		SLG_UNSELECTED_SLOT_GRAPHICS_NUMBER	2

#define		SLG_DOUBLE_CLICK_DELAY							500

//defines for saved game version status
enum
{
	SLS_HEADER_OK,
	SLS_SAVED_GAME_VERSION_OUT_OF_DATE,
	SLS_GAME_VERSION_OUT_OF_DATE,
	SLS_BOTH_SAVE_GAME_AND_GAME_VERSION_OUT_OF_DATE,
};


static BOOLEAN gfSaveLoadScreenEntry = TRUE;
static BOOLEAN gfSaveLoadScreenExit	= FALSE;
BOOLEAN        gfRedrawSaveLoadScreen = TRUE;

static ScreenID guiSaveLoadExitScreen = SAVE_LOAD_SCREEN;


//Contains the array of valid save game locations
static BOOLEAN* gbSaveGameArray = NULL;
// save game paths list
std::vector<std::string> saveGames = std::vector<std::string>();
// gbSaveGameArray size (due to INT16, up to +32767)
INT16 sNumSaveGames = -1;
// starting and ending indices for MPSLSM
INT16 sSaveBeginIndex = 0, 
	  sSaveEndIndex = NUM_SAVE_GAMES - 1;

static BOOLEAN gfDoingQuickLoad = FALSE;

//This flag is used to diferentiate between loading a game and saveing a game.
// gfSaveGame=TRUE		For saving a game
// gfSaveGame=FALSE		For loading a game
BOOLEAN		gfSaveGame=TRUE;

static BOOLEAN gfSaveLoadScreenButtonsCreated = FALSE;

static INT8 gbSelectedSaveLocation = -1;
static INT8 gbHighLightedLocation  = -1;

static SGPVObject* guiSlgBackGroundImage;
static SGPVObject* guiBackGroundAddOns;


// The string that will contain the game desc text
static wchar_t gzGameDescTextField[SIZE_OF_SAVE_GAME_DESC];


static BOOLEAN gfUserInTextInputMode = FALSE;
static UINT8   gubSaveGameNextPass   = 0;

static BOOLEAN gfStartedFadingOut = FALSE;


BOOLEAN		gfCameDirectlyFromGame = FALSE;


BOOLEAN		gfLoadedGame = FALSE;	//Used to know when a game has been loaded, the flag in gtacticalstatus might have been reset already

BOOLEAN		gfLoadGameUponEntry = FALSE;

static BOOLEAN gfHadToMakeBasementLevels = FALSE;


//
//Buttons
//
static BUTTON_PICS* guiSlgButtonImage;


// Cancel Button
static GUIButtonRef guiSlgCancelBtn;

// Save game Button
static BUTTON_PICS* guiSaveLoadImage;
static GUIButtonRef guiSlgSaveLoadBtn;

// previous and next page buttons
static GUIButtonRef guiGetPrevPageBtn,
                    guiGetNextPageBtn;

//Mouse regions for the currently selected save game
static MOUSE_REGION gSelectedSaveRegion[NUM_SAVE_GAMES];

static MOUSE_REGION gSLSEntireScreenRegion;


static void EnterSaveLoadScreen();
static void ExitSaveLoadScreen(void);
static void GetSaveLoadScreenUserInput(void);
static void RenderSaveLoadScreen(void);
static void SaveLoadGameNumber();


ScreenID SaveLoadScreenHandle()
{
	if( gfSaveLoadScreenEntry )
	{
		EnterSaveLoadScreen();
		gfSaveLoadScreenEntry = FALSE;
		gfSaveLoadScreenExit = FALSE;

		PauseGame();

		//save the new rect
		BlitBufferToBuffer(FRAME_BUFFER, guiSAVEBUFFER, 0, 0, SCREEN_WIDTH, 439);
	}

	RestoreBackgroundRects();

	//to guarentee that we do not accept input when we are fading out
	if( !gfStartedFadingOut )
	{
		GetSaveLoadScreenUserInput();
	}
	else
		gfRedrawSaveLoadScreen = FALSE;

	//if we have exited the save load screen, exit
	if( !gfSaveLoadScreenButtonsCreated )
		return( guiSaveLoadExitScreen );

	RenderAllTextFields();

	if( gfRedrawSaveLoadScreen )
	{
		RenderSaveLoadScreen();
		MarkButtonsDirty( );
		RenderButtons();

		gfRedrawSaveLoadScreen = FALSE;
	}

	if( gubSaveGameNextPass != 0 )
	{
		gubSaveGameNextPass++;

		if( gubSaveGameNextPass == 5 )
		{
			gubSaveGameNextPass = 0;
			SaveLoadGameNumber();
		}
	}


	//If we are not exiting the screen, render the buttons
	if( !gfSaveLoadScreenExit && guiSaveLoadExitScreen == SAVE_LOAD_SCREEN )
	{
		// render buttons marked dirty
		RenderButtons( );
	}


	// ATE: Put here to save RECTS before any fast help being drawn...
	SaveBackgroundRects( );
	RenderButtonsFastHelp();

	ExecuteBaseDirtyRectQueue( );
	EndFrameBufferRender( );

	if ( HandleFadeOutCallback( ) )
	{
		return( guiSaveLoadExitScreen );
	}

	if ( HandleBeginFadeOut( SAVE_LOAD_SCREEN ) )
	{
		return( SAVE_LOAD_SCREEN );
	}


	if( gfSaveLoadScreenExit )
	{
		ExitSaveLoadScreen();
	}

	if ( HandleFadeInCallback( ) )
	{
		// Re-render the scene!
		RenderSaveLoadScreen();
	}

	if ( HandleBeginFadeIn( SAVE_LOAD_SCREEN ) )
	{
	}

	return( guiSaveLoadExitScreen );
}


static void DestroySaveLoadTextInputBoxes(void);


static void SetSaveLoadExitScreen(ScreenID const uiScreen)
{
	if( uiScreen == GAME_SCREEN )
	{
		EnterTacticalScreen( );
	}

	gfSaveLoadScreenExit	= TRUE;

	guiSaveLoadExitScreen = uiScreen;

	SetPendingNewScreen( uiScreen );

	if( gfDoingQuickLoad )
	{
		fFirstTimeInGameScreen = TRUE;
		SetPendingNewScreen( uiScreen );
	}

	ExitSaveLoadScreen();

	DestroySaveLoadTextInputBoxes();
}


static void LeaveSaveLoadScreen()
{
	ScreenID const exit_screen =
		gfCameDirectlyFromGame                     ? guiPreviousOptionScreen :
		guiPreviousOptionScreen == MAINMENU_SCREEN ? MAINMENU_SCREEN         :
		OPTIONS_SCREEN;
	SetSaveLoadExitScreen(exit_screen);
}


static GUIButtonRef MakeButton(BUTTON_PICS* const img, const wchar_t* const text, const INT16 x, const GUI_CALLBACK click)
{
	return CreateIconAndTextButton(img, text, OPT_BUTTON_FONT, OPT_BUTTON_ON_COLOR, DEFAULT_SHADOW, OPT_BUTTON_OFF_COLOR, DEFAULT_SHADOW, x, SLG_BTN_POS_Y, MSYS_PRIORITY_HIGH, click);
}


static void BtnSlgCancelCallback(GUI_BUTTON* btn, INT32 reason);
static void BtnSlgSaveLoadCallback(GUI_BUTTON* btn, INT32 reason);
static void ClearSelectedSaveSlot(void);
static void InitSaveGameArray(void);
static BOOLEAN LoadSavedGameHeader(INT8 bEntry, SAVED_GAME_HEADER* pSaveGameHeader);
static void SelectedSLSEntireRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason);
static void SelectedSaveRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason);
static void SelectedSaveRegionMovementCallBack(MOUSE_REGION* pRegion, INT32 reason);
static void StartFadeOutForSaveLoadScreen(void);

static void GetPrevPage(SDLKey Key = SDLK_PAGEUP);
static void GetNextPage(SDLKey Key = SDLK_PAGEDOWN);

static void GetPrevPageCallback(GUI_BUTTON *btn, INT32 reason)
{
	static INT32 iLastRepeatTime = 0;

	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		GetPrevPage();
	}
	else if (reason & MSYS_CALLBACK_REASON_LBUTTON_REPEAT)
	{
		if (GetJA2Clock() - iLastRepeatTime >= MESSAGE_BTN_SCROLL_TIME)
		{
			GetPrevPage();
			iLastRepeatTime = GetJA2Clock();
		}
	}
}


static void GetNextPageCallback(GUI_BUTTON *btn, INT32 reason)
{
	static INT32 iLastRepeatTime = 0;

	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		GetNextPage();
	}
	else if (reason & MSYS_CALLBACK_REASON_LBUTTON_REPEAT)
	{
		if (GetJA2Clock() - iLastRepeatTime >= MESSAGE_BTN_SCROLL_TIME)
		{
			GetNextPage();
			iLastRepeatTime = GetJA2Clock();
		}
	}
}


static wchar_t* GetContentAfterFirstNewline(const wchar_t *str) 
{
	INT8 start, end, string_length;
	for (start = 0; str[start] != L'\n'; start++);
	start++;
	for (end = 0; str[end] != L'\0'; end++);
	end++;

	string_length = end - start;

	// add 1 for proper string null termination
	wchar_t* retval = MALLOCNZ(wchar_t, string_length + 1);

	wcslcpy(retval, &str[start], string_length);

	return retval;
}


static void EnableDisablePrevNextPageBtns(void)
{
	// if on zeroth page, disable "get previous page" button
	if (!sSaveBeginIndex)
		DisableButton(guiGetPrevPageBtn);
	else
		EnableButton(guiGetPrevPageBtn);
	// enable the next button if:
	// not all saved games have been shown, or
	// we're saving, on the last page and an empty slot wouldn't fit otherwise
	if ((sSaveEndIndex < sNumSaveGames - 1) ||
		(gfSaveGame && sSaveEndIndex == sNumSaveGames - 1))
		EnableButton(guiGetNextPageBtn);
	// if on the last page (previous conditions don't apply), disable the 
	// "get next page" button
	else
		DisableButton(guiGetNextPageBtn);
}


static void EnterSaveLoadScreen()
{
	// This is a hack to get sector names, but if the underground sector is NOT loaded
	if (!gpUndergroundSectorInfoHead)
	{
		BuildUndergroundSectorInfoList();
		gfHadToMakeBasementLevels = TRUE;
	}
	else
	{
		gfHadToMakeBasementLevels = FALSE;
	}

	guiSaveLoadExitScreen = SAVE_LOAD_SCREEN;
	InitSaveGameArray();
	EmptyBackgroundRects();

	// should do until localized
    // TODO: #embrosyn: do proper l10n in langRes files?
	const wchar_t *pGetPrevPageText = GetContentAfterFirstNewline(pMapScreenBottomFastHelp[5]);
	const wchar_t *pGetNextPageText = GetContentAfterFirstNewline(pMapScreenBottomFastHelp[6]);

	guiGetPrevPageBtn = MakeArrowButton(9, 0, 2, SLG_LOAD_CANCEL_POS_X + 252, STD_SCREEN_Y + 447, GetPrevPageCallback, pGetPrevPageText);
	guiGetNextPageBtn = MakeArrowButton(10, 1, 3, SLG_LOAD_CANCEL_POS_X + 277, STD_SCREEN_Y + 447, GetNextPageCallback, pGetNextPageText);

	EnableDisablePrevNextPageBtns();
	
	// If the user has asked to load the selected save
	if (gfLoadGameUponEntry)
	{
		// Make sure the save is valid
		INT8 const last_slot = gGameSettings.bLastSavedGameSlot;
		if (last_slot != -1 && gbSaveGameArray[sSaveBeginIndex + last_slot])
		{
			gbSelectedSaveLocation = last_slot;
			StartFadeOutForSaveLoadScreen();
		}
		else
		{ // else the save is not valid, so do not load it
			gfLoadGameUponEntry = FALSE;
		}
	}

	// Load main background and add ons graphic
	guiSlgBackGroundImage = AddVideoObjectFromFile(INTERFACEDIR "/loadscreen.sti");
	guiBackGroundAddOns   = AddVideoObjectFromFile(GetMLGFilename(MLG_LOADSAVEHEADER));

	guiSlgButtonImage = LoadButtonImage(INTERFACEDIR "/loadscreenaddons.sti", 6, 9);
	guiSlgCancelBtn   = MakeButton(guiSlgButtonImage, zSaveLoadText[SLG_CANCEL], SLG_LOAD_CANCEL_POS_X, BtnSlgCancelCallback);

	// Either the save or load button
	INT32          gfx;
	wchar_t const* text;
	if (gfSaveGame)
	{
		gfx  = 5;
		text = zSaveLoadText[SLG_SAVE_GAME];
	}
	else
	{
		gfx  = 4;
		text = zSaveLoadText[SLG_LOAD_GAME];
	}
	guiSaveLoadImage  = UseLoadedButtonImage(guiSlgButtonImage, gfx, gfx + 3);
	guiSlgSaveLoadBtn = MakeButton(guiSaveLoadImage, text, SLG_SAVE_LOAD_BTN_POS_X, BtnSlgSaveLoadCallback);
	guiSlgSaveLoadBtn->SpecifyDisabledStyle(GUI_BUTTON::DISABLED_STYLE_HATCHED);

	// if we find ourselves on an empty load page which isn't the zeroth, try fetching an earlier one
	if (!gfSaveGame && sSaveBeginIndex != 0 && !gbSaveGameArray[sSaveBeginIndex])
	{
		sSaveBeginIndex -= NUM_SAVE_GAMES;
		sSaveEndIndex -= NUM_SAVE_GAMES;
	}

	UINT16 const x = SLG_FIRST_SAVED_SPOT_X;
	UINT16       y = SLG_FIRST_SAVED_SPOT_Y;
	for (INT8 i = 0; i != NUM_SAVE_GAMES; ++i)
	{
		MOUSE_REGION& r = gSelectedSaveRegion[i];
		MSYS_DefineRegion(&r, x, y, x + SLG_SAVELOCATION_WIDTH, y + SLG_SAVELOCATION_HEIGHT, MSYS_PRIORITY_HIGH, CURSOR_NORMAL, SelectedSaveRegionMovementCallBack, SelectedSaveRegionCallBack);
		MSYS_SetRegionUserData(&r, 0, i);

		// We cannot load a game that has not been saved
		if (!gfSaveGame)
		{
			if (gbSaveGameArray[i + sSaveBeginIndex]) r.Enable();
			else r.Disable();
		}
		else
		{
			// zeroth (quick save, on the zeroth page) slot should be enabled only if not on the zeroth page
			if (i == 0 && sSaveBeginIndex != 0)
				r.Enable();

			else if (gbSaveGameArray[i + sSaveBeginIndex])
				r.Enable();

			// make the next empty slot clickable
			// going all the way to the last slot
			else if (i > 0)
			{
				// looking at the previous slot
				if (gbSaveGameArray[i - 1 + sSaveBeginIndex])
					r.Enable();
				else
					r.Disable();
			}

			else r.Disable();
		}
		y += SLG_GAP_BETWEEN_LOCATIONS;
	}

	// Create the screen mask to enable ability to right click to cancel the save game
	MSYS_DefineRegion(&gSLSEntireScreenRegion, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MSYS_PRIORITY_HIGH - 10, CURSOR_NORMAL, MSYS_NO_CALLBACK, SelectedSLSEntireRegionCallBack);

	ClearSelectedSaveSlot();

	RemoveMouseRegionForPauseOfClock();

	gbHighLightedLocation  = -1;
	gzGameDescTextField[0] = '\0';

	// If the last saved game slot is ok, set the selected slot to the last saved slot
	INT8 const last_slot = gGameSettings.bLastSavedGameSlot;
	if (last_slot != -1            &&
			gbSaveGameArray[last_slot + sSaveBeginIndex] &&
			(!gfSaveGame || last_slot != 0)) // If it is not the quicksave slot, and we are loading
	{
		SAVED_GAME_HEADER SaveGameHeader;
		if (LoadSavedGameHeader(last_slot, &SaveGameHeader))
		{
			wcscpy(gzGameDescTextField, SaveGameHeader.sSavedGameDesc);
			gbSelectedSaveLocation = last_slot;
		}
		else
		{
			gGameSettings.bLastSavedGameSlot = -1;
		}
	}

	EnableButton(guiSlgSaveLoadBtn, gbSelectedSaveLocation != -1);

	RenderSaveLoadScreen();

	// Save load buttons are created
	gfSaveLoadScreenButtonsCreated = TRUE;

	gfDoingQuickLoad   = FALSE;
	gfStartedFadingOut = FALSE;

	DisableScrollMessages();

	gfLoadedGame = FALSE;

	if (gfLoadGameUponEntry)
	{
		guiSlgCancelBtn->uiFlags   |= BUTTON_FORCE_UNDIRTY;
		guiSlgSaveLoadBtn->uiFlags |= BUTTON_FORCE_UNDIRTY;
		FRAME_BUFFER->Fill(0);
	}

	gfGettingNameFromSaveLoadScreen = FALSE;
}


static void ExitSaveLoadScreen(void)
{
	INT8	i;


	gfLoadGameUponEntry = FALSE;

	if( !gfSaveLoadScreenButtonsCreated )
		return;

	gfSaveLoadScreenExit = FALSE;
	gfSaveLoadScreenEntry = TRUE;

	UnloadButtonImage( guiSlgButtonImage );

	RemoveButton( guiSlgCancelBtn );

	//Remove the save / load button
//	if( !gfSaveGame )
	{
		RemoveButton( guiSlgSaveLoadBtn );
		UnloadButtonImage( guiSaveLoadImage );
	}

	RemoveButton(guiGetPrevPageBtn);
	RemoveButton(guiGetNextPageBtn);

	for(i=0; i<NUM_SAVE_GAMES; i++)
	{
	  MSYS_RemoveRegion( &gSelectedSaveRegion[i]);
	}

	DeleteVideoObject(guiSlgBackGroundImage);
	DeleteVideoObject(guiBackGroundAddOns);

	//Destroy the text fields ( if created )
	DestroySaveLoadTextInputBoxes();

  MSYS_RemoveRegion( &gSLSEntireScreenRegion );

	gfSaveLoadScreenEntry = TRUE;
	gfSaveLoadScreenExit = FALSE;

	if( !gfLoadedGame )
	{
		UnLockPauseState( );
		UnPauseGame();
	}

	gfSaveLoadScreenButtonsCreated = FALSE;

	gfCameDirectlyFromGame = FALSE;

	//unload the basement sectors
	if( gfHadToMakeBasementLevels )
		TrashUndergroundSectorInfo();

	gfGettingNameFromSaveLoadScreen = FALSE;
}


static void DisplaySaveGameList(void);


static void RenderSaveLoadScreen(void)
{
	// If we are going to be instantly leaving the screen, don't draw the numbers
	if (gfLoadGameUponEntry) return;

	BltVideoObject(FRAME_BUFFER, guiSlgBackGroundImage, 0, STD_SCREEN_X, STD_SCREEN_Y);

	// Display the Title
	UINT16 const gfx = gfSaveGame ? 1 : 0;
	BltVideoObject(FRAME_BUFFER, guiBackGroundAddOns, gfx, SLG_TITLE_POS_X, SLG_TITLE_POS_Y);

	DisplaySaveGameList();
	InvalidateScreen();
}


static bool GetGameDescription()
{
	INT8 const id = GetActiveFieldID();
	if (id == 0 || id == -1) return false;

	wcslcpy(gzGameDescTextField, GetStringFromField(id), lengthof(gzGameDescTextField));
	return true;
}


static void DisplayOnScreenNumber(BOOLEAN display);
static BOOLEAN DisplaySaveGameEntry(INT8 bEntryID);
static void MoveSelectionDown();
static void MoveSelectionUp();
// TODO: #embrosyn: use custom enum instead? VS lags due to SDLKey usage...
static void GetPrevNextPageFlush(SDLKey Key);
static void ResetSaveLoadTextInputBoxes();
static void SetSelection(UINT8 ubNewSelection);


static void GetSaveLoadScreenUserInput(void)
{
	static BOOLEAN fWasCtrlHeldDownLastFrame = FALSE;

	// If we are going to be instantly leaving the screen, dont draw the numbers
	if (gfLoadGameUponEntry) return;

	DisplayOnScreenNumber(_KeyDown(ALT));

	if (_KeyDown(CTRL) || fWasCtrlHeldDownLastFrame)
	{
		DisplaySaveGameEntry(gbSelectedSaveLocation);
	}
	fWasCtrlHeldDownLastFrame = _KeyDown(CTRL);

	SGPPoint mouse_pos;
	GetMousePos(&mouse_pos);

	InputAtom e;
	while (DequeueEvent(&e))
	{
		MouseSystemHook(e.usEvent, mouse_pos.iX, mouse_pos.iY);
		if (HandleTextInput(&e)) continue;

		if (e.usEvent == KEY_DOWN)
		{
			switch (e.usParam)
			{
				case '1': SetSelection( 1); break;
				case '2': SetSelection( 2); break;
				case '3': SetSelection( 3); break;
				case '4': SetSelection( 4); break;
				case '5': SetSelection( 5); break;
				case '6': SetSelection( 6); break;
				case '7': SetSelection( 7); break;
				case '8': SetSelection( 8); break;
				case '9': SetSelection( 9); break;
				case '0': SetSelection(10); break;
			}
		}
		else if (e.usEvent == KEY_UP)
		{
			switch (e.usParam)
			{
				case 'a':
					if (_KeyDown(ALT) && !gfSaveGame)
					{
						INT8 const slot = GetNumberForAutoSave(TRUE);
						if (slot == -1) break;

						guiLastSaveGameNum     = slot;
						gbSelectedSaveLocation = SAVE__END_TURN_NUM;
						StartFadeOutForSaveLoadScreen();
					}
					break;

				case 'b':
					if (_KeyDown(ALT) && !gfSaveGame)
					{
						INT8 const slot = GetNumberForAutoSave(FALSE);
						if (slot == -1) break;

						guiLastSaveGameNum     = 1 - slot;
						gbSelectedSaveLocation = SAVE__END_TURN_NUM;
						StartFadeOutForSaveLoadScreen();
					}
					break;

				case SDLK_UP:   MoveSelectionUp();   break;
				case SDLK_DOWN: MoveSelectionDown(); break;

				case SDLK_PAGEUP: GetPrevPage(); break;
				case SDLK_PAGEDOWN: GetNextPage(); break;

				case SDLK_ESCAPE:
					if (gbSelectedSaveLocation == -1)
					{
						LeaveSaveLoadScreen();
					}
					else
					{ // Reset selected slot
						gbSelectedSaveLocation = -1;
						gfRedrawSaveLoadScreen = TRUE;
						DestroySaveLoadTextInputBoxes();
						DisableButton(guiSlgSaveLoadBtn);
					}
					break;

				case SDLK_RETURN:
					if (!gfSaveGame)
					{
						SaveLoadGameNumber();
					}
					else if (GetGameDescription())
					{
						SetActiveField(0);
						DestroySaveLoadTextInputBoxes();
						SaveLoadGameNumber();
					}
					else if (gbSelectedSaveLocation != -1)
					{
						SaveLoadGameNumber();
					}
					else
					{
						gfRedrawSaveLoadScreen = TRUE;
					}
					break;
			}
		}
	}
}


static UINT8 CompareSaveGameVersion(INT8 bSaveGameID);
static void ConfirmSavedGameMessageBoxCallBack(MessageBoxReturnValue);
static void LoadSavedGameWarningMessageBoxCallBack(MessageBoxReturnValue);
static void SaveGameToSlotNum(void);


static void SaveLoadGameNumber()
{
	INT8 const save_slot_id = gbSelectedSaveLocation;
	if (save_slot_id < 0 || NUM_SAVE_GAMES <= save_slot_id) return;

	if (gfSaveGame)
	{
		GetGameDescription();

		// If there is save game in the slot, ask for confirmation before overwriting
		if (gbSaveGameArray[save_slot_id + sSaveBeginIndex])
		{
			wchar_t sText[512];
			swprintf(sText, lengthof(sText), zSaveLoadText[SLG_CONFIRM_SAVE], save_slot_id + sSaveBeginIndex);
			DoSaveLoadMessageBox(sText, SAVE_LOAD_SCREEN, MSG_BOX_FLAG_YESNO, ConfirmSavedGameMessageBoxCallBack);
		}
		else
		{ // else do NOT put up a confirmation
			SaveGameToSlotNum();
		}
	}
	else
	{
		// Check to see if the save game headers are the same
		UINT8 const ret = CompareSaveGameVersion(save_slot_id);
		if (ret != SLS_HEADER_OK)
		{
			wchar_t const* const msg =
				ret == SLS_GAME_VERSION_OUT_OF_DATE       ? zSaveLoadText[SLG_GAME_VERSION_DIF] :
				ret == SLS_SAVED_GAME_VERSION_OUT_OF_DATE ? zSaveLoadText[SLG_SAVED_GAME_VERSION_DIF] :
				zSaveLoadText[SLG_BOTH_GAME_AND_SAVED_GAME_DIF];
			DoSaveLoadMessageBox(msg, SAVE_LOAD_SCREEN, MSG_BOX_FLAG_YESNO, LoadSavedGameWarningMessageBoxCallBack);
		}
		else
		{
			StartFadeOutForSaveLoadScreen();
		}
	}
}


void DoSaveLoadMessageBoxWithRect(wchar_t const* const zString, ScreenID const uiExitScreen, MessageBoxFlags const usFlags, MSGBOX_CALLBACK const ReturnCallback, SGPBox const* const centering_rect)
{
	// do message box and return
	DoMessageBox(MSG_BOX_BASIC_STYLE, zString, uiExitScreen, usFlags, ReturnCallback, centering_rect);
}


void DoSaveLoadMessageBox(wchar_t const* const zString, ScreenID const uiExitScreen, MessageBoxFlags const usFlags, MSGBOX_CALLBACK const ReturnCallback)
{
	DoSaveLoadMessageBoxWithRect(zString, uiExitScreen, usFlags, ReturnCallback, NULL);
}


static void InitSaveGameArray(void)
{
	// list of all saved games
	saveGames = FindFilesInDir(GCM->getSavedGamesFolder(), ".sav", false, true, false);
	
	INT16 cnt = -1, i;

	// number of saved games (up to 32767, due to INT16)
	sNumSaveGames = saveGames.size();

	// seek for a quicksave in the save game list
	for (i = 0; i < sNumSaveGames; ++i)
	{
		if (saveGames[i] == "QuickSave.sav") 
		{
			// move it to the zeroth place, if it's not already where it should
			// be (at position 0)
			if (i != 0) 
			{
				std::string tmp = saveGames[i];
				saveGames.erase(saveGames.begin() + i);
				saveGames.insert(saveGames.begin(), tmp);
			}
			cnt = 0;
			break;
		}
	}
	
	// if there was no quick save, add a dummy slot at the beginning ...
	if (cnt == -1)
	{
		std::string tmp("");
		saveGames.insert(saveGames.begin(), tmp);

		cnt = 1;
		sNumSaveGames++;
	}
	
	// gbSaveGameArray size calculation: decimal part is truncated due to integer 
	// division, which is compensated by adding 1; also, if there is exactly 
	// n * NUM_SAVE_GAMES, adding 1 will ensure the next empty save page 
	// renders properly as the gbSaveGameArray will be zero-filled with MALLOCNZ
	INT16 saveArraySize = (sNumSaveGames / NUM_SAVE_GAMES + 1) * NUM_SAVE_GAMES;

	// allocate save game array
	gbSaveGameArray = MALLOCNZ(BOOLEAN, saveArraySize);

	// ... and store a FALSE in the gbSaveGameArray
	if (cnt == 1)
		gbSaveGameArray[0] = FALSE;

	INT16 sSaveBeginIndexTmp = sSaveBeginIndex;

	for (sSaveBeginIndex = 0; sSaveBeginIndex < sNumSaveGames; sSaveBeginIndex += NUM_SAVE_GAMES)
	{
		for (; cnt != NUM_SAVE_GAMES; ++cnt)
		{
			SAVED_GAME_HEADER SaveGameHeader;
			gbSaveGameArray[cnt + sSaveBeginIndex] = LoadSavedGameHeader(cnt, &SaveGameHeader);
		}

		cnt = 0;
	}

	sSaveBeginIndex = sSaveBeginIndexTmp;
}


static void DisplaySaveGameList(void)
{
	for (INT8 i = 0; i != NUM_SAVE_GAMES; i++)
	{ // Display all the information from the header
		DisplaySaveGameEntry(i);
	}
}


static BOOLEAN DisplaySaveGameEntry(INT8 const entry_idx)
{
	if (entry_idx == -1) return TRUE;
	// If we are going to be instantly leaving the screen, dont draw the numbers
	if (gfLoadGameUponEntry) return TRUE;
	// If we are currently fading out, leave
	if (gfStartedFadingOut) return TRUE;

	UINT16 const bx = SLG_FIRST_SAVED_SPOT_X;
	UINT16 const by = SLG_FIRST_SAVED_SPOT_Y + SLG_GAP_BETWEEN_LOCATIONS * entry_idx;

	bool const is_selected = entry_idx == gbSelectedSaveLocation;
	bool const save_exists = gbSaveGameArray[sSaveBeginIndex + entry_idx];
	bool const prev_save_exists = entry_idx + sSaveBeginIndex > 0  ? 
		gbSaveGameArray[sSaveBeginIndex + entry_idx - 1] : FALSE;

	// Background
	UINT16 const gfx = is_selected ?
		SLG_SELECTED_SLOT_GRAPHICS_NUMBER : SLG_UNSELECTED_SLOT_GRAPHICS_NUMBER;
	BltVideoObject(FRAME_BUFFER, guiBackGroundAddOns, gfx, bx, by);

	SGPFont  font = SAVE_LOAD_NORMAL_FONT;
	UINT8 foreground;
	UINT8 shadow;
	if (entry_idx == 0 && gfSaveGame && sSaveBeginIndex == 0)
	{ // The QuickSave slot
		FRAME_BUFFER->ShadowRect(bx, by, bx + SLG_SAVELOCATION_WIDTH, by + SLG_SAVELOCATION_HEIGHT);
		foreground = SAVE_LOAD_QUICKSAVE_COLOR;
		shadow     = SAVE_LOAD_QUICKSAVE_SHADOW_COLOR;
	}
	else if (is_selected)
	{ // The currently selected location
		foreground = SAVE_LOAD_SELECTED_COLOR;
		shadow     = SAVE_LOAD_SELECTED_SHADOW_COLOR;
	}
	else if (entry_idx == gbHighLightedLocation)
	{ // The highlighted slot
		foreground = SAVE_LOAD_HIGHLIGHTED_COLOR;
		shadow     = SAVE_LOAD_HIGHLIGHTED_SHADOW_COLOR;
	}
	else if (save_exists)
	{ // The file exists
		foreground = SAVE_LOAD_NORMAL_COLOR;
		shadow     = SAVE_LOAD_NORMAL_SHADOW_COLOR;
	}
	else if (gfSaveGame && prev_save_exists)
	{ // We are saving a game
		foreground = SAVE_LOAD_EMPTYSLOT_COLOR;
		shadow     = SAVE_LOAD_EMPTYSLOT_SHADOW_COLOR;
	}
	else
	{
		FRAME_BUFFER->ShadowRect(bx, by, bx + SLG_SAVELOCATION_WIDTH, by + SLG_SAVELOCATION_HEIGHT);
		foreground = SAVE_LOAD_QUICKSAVE_COLOR;
		shadow     = SAVE_LOAD_QUICKSAVE_SHADOW_COLOR;
	}
	SetFontShadow(shadow);

	if (save_exists || is_selected)
	{ // Setup the strings to be displayed
		SAVED_GAME_HEADER header;
		if (gfSaveGame && is_selected)
		{ // The user has selected a spot to save.  Fill out all the required information
			wcscpy(header.sSavedGameDesc, gzGameDescTextField);
			header.uiDay                     = GetWorldDay();
			header.ubHour                    = GetWorldHour();
			header.ubMin                     = guiMin;
			GetBestPossibleSectorXYZValues(&header.sSectorX, &header.sSectorY, &header.bSectorZ);
			header.ubNumOfMercsOnPlayersTeam = NumberOfMercsOnPlayerTeam();
			header.iCurrentBalance           = LaptopSaveInfo.iCurrentBalance;
			header.sInitialGameOptions       = gGameOptions;
		}
		else if (!LoadSavedGameHeader(entry_idx, &header))
		{
			return FALSE;
		}

		UINT16 x = bx;
		UINT16 y = by + SLG_DATE_OFFSET_Y;
		if (is_selected)
		{ // This is the currently selected location, move the text up a bit
			x++;
			y--;
		}

		if (!gfSaveGame && _KeyDown(CTRL) && is_selected)
		{ // The user is LOADING and holding down the CTRL key, display the additional info
			// Create a string for difficulty level
			wchar_t difficulty[256];
			swprintf(difficulty, lengthof(difficulty), L"%ls %ls", gzGIOScreenText[GIO_EASY_TEXT + header.sInitialGameOptions.ubDifficultyLevel - 1], zSaveLoadText[SLG_DIFF]);

			// Make a string containing the extended options
			wchar_t options[256];
			swprintf(options, lengthof(options), L"%20ls     %22ls     %22ls     %22ls",
				difficulty,
				/*gzGIOScreenText[GIO_TIMED_TURN_TITLE_TEXT + header.sInitialGameOptions.fTurnTimeLimit + 1],*/
				header.sInitialGameOptions.fIronManMode ? gzGIOScreenText[GIO_IRON_MAN_TEXT] : gzGIOScreenText[GIO_SAVE_ANYWHERE_TEXT],
				header.sInitialGameOptions.fGunNut      ? zSaveLoadText[SLG_ADDITIONAL_GUNS] : zSaveLoadText[SLG_NORMAL_GUNS],
				header.sInitialGameOptions.fSciFi       ? zSaveLoadText[SLG_SCIFI]           : zSaveLoadText[SLG_REALISTIC]
			);

			// The date
			DrawTextToScreen(options, x + SLG_DATE_OFFSET_X, y, 0, font, foreground, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
		}
		else
		{ // Display the Saved game information
			// The date
			wchar_t date[128];
			swprintf(date, lengthof(date), L"%ls %d, %02d:%02d", pMessageStrings[MSG_DAY], header.uiDay, header.ubHour, header.ubMin);
			DrawTextToScreen(date, x + SLG_DATE_OFFSET_X, y, 0, font, foreground, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);

			// The sector
			wchar_t location[128];
			if (header.sSectorX != -1 && header.sSectorY != -1 && header.bSectorZ >= 0)
			{
				gfGettingNameFromSaveLoadScreen = TRUE;
				GetSectorIDString(header.sSectorX, header.sSectorY, header.bSectorZ, location, lengthof(location), FALSE);
				gfGettingNameFromSaveLoadScreen = FALSE;
			}
			else if (header.uiDay * NUM_SEC_IN_DAY + header.ubHour * NUM_SEC_IN_HOUR + header.ubMin * NUM_SEC_IN_MIN <= STARTING_TIME)
			{
				wcslcpy(location, gpStrategicString[STR_PB_NOTAPPLICABLE_ABBREVIATION], lengthof(location));
			}
			else
			{
				wcslcpy(location, gzLateLocalizedString[STR_LATE_14], lengthof(location));
			}
			ReduceStringLength(location, lengthof(location), SLG_SECTOR_WIDTH, font);
			DrawTextToScreen(location, x + SLG_SECTOR_OFFSET_X, y, 0, font, foreground, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);

			// Number of mercs on the team
			// If only 1 merc is on the team use "merc" else "mercs"
			UINT8          const n_mercs = header.ubNumOfMercsOnPlayersTeam;
			wchar_t const* const merc    = n_mercs == 1 ?
				MercAccountText[MERC_ACCOUNT_MERC] :
				pMessageStrings[MSG_MERCS];
			wchar_t merc_count[128];
			swprintf(merc_count, lengthof(merc_count), L"%d %ls", n_mercs, merc);
			DrawTextToScreen(merc_count, x + SLG_NUM_MERCS_OFFSET_X, y, 0, font, foreground, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);

			// The balance
			wchar_t balance[128];
			SPrintMoney(balance, header.iCurrentBalance);
			DrawTextToScreen(balance, x + SLG_BALANCE_OFFSET_X, y, 0, font, foreground, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);

			if (save_exists || (gfSaveGame && !gfUserInTextInputMode && is_selected))
			{
				// The saved game description
				DrawTextToScreen(header.sSavedGameDesc, x + SLG_SAVE_GAME_DESC_X, y, 0, font, foreground, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
			}
		}
	}
	else
	{
		// If this is the quick save slot
		wchar_t const* const txt = entry_idx + sSaveBeginIndex == 0 ?
			pMessageStrings[MSG_EMPTY_QUICK_SAVE_SLOT] :
			pMessageStrings[MSG_EMPTYSLOT];
		DrawTextToScreen(txt, bx, by + SLG_DATE_OFFSET_Y, 609, font, foreground, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
	}

	// Reset the shadow color
	SetFontShadow(DEFAULT_SHADOW);

	InvalidateRegion(bx, by, bx + SLG_SAVELOCATION_WIDTH, by + SLG_SAVELOCATION_HEIGHT);
	return TRUE;
}


static BOOLEAN LoadSavedGameHeader(const INT8 bEntry, SAVED_GAME_HEADER* const header)
{
	// make sure the entry is valid
	if (0 <= bEntry + sSaveBeginIndex && bEntry + sSaveBeginIndex < sNumSaveGames)
	{
		char zSavedGameName[512];
		CreateSavedGameFileNameFromNumber(bEntry, zSavedGameName);

		try
		{
			bool stracLinuxFormat;
			AutoSGPFile f(GCM->openUserPrivateFileForReading(zSavedGameName));
			ExtractSavedGameHeaderFromFile(f, *header, &stracLinuxFormat);
			endof(header->zGameVersionNumber)[-1] =  '\0';
			endof(header->sSavedGameDesc)[-1]     = L'\0';
			return TRUE;
		}
		catch (...) { /* Handled below */ }

		gbSaveGameArray[bEntry + sSaveBeginIndex] = FALSE;
	}
	memset(header, 0, sizeof(*header));
	return FALSE;
}

static void BtnSlgCancelCallback(GUI_BUTTON* const btn, INT32 const reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		LeaveSaveLoadScreen();
	}
}


static void BtnSlgSaveLoadCallback(GUI_BUTTON* btn, INT32 reason)
{
	if(reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		SaveLoadGameNumber();
	}
}


static void DisableSelectedSlot(void);
static void InitSaveLoadScreenTextInputBoxes(void);
static void RedrawSaveLoadScreenAfterMessageBox(MessageBoxReturnValue);


static void SelectedSaveRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		UINT8	bSelected = (UINT8)MSYS_GetRegionUserData( pRegion, 0 );
		static UINT32	uiLastTime = 0;
		UINT32	uiCurTime = GetJA2Clock();

/*
		//If we are saving and this is the quick save slot
		if( gfSaveGame && bSelected == 0 )
		{
			//Display a pop up telling user what the quick save slot is
			DoSaveLoadMessageBox(pMessageStrings[MSG_QUICK_SAVE_RESERVED_FOR_TACTICAL], SAVE_LOAD_SCREEN, MSG_BOX_FLAG_OK, RedrawSaveLoadScreenAfterMessageBox);
			return;
		}

		SetSelection( bSelected );
*/

		//If we are saving and this is the quick save slot
		// ... and we are on the first page
		if( gfSaveGame && bSelected == 0 && sSaveBeginIndex == 0 )
		{
			//Display a pop up telling user what the quick save slot is
			DoSaveLoadMessageBox(pMessageStrings[MSG_QUICK_SAVE_RESERVED_FOR_TACTICAL], SAVE_LOAD_SCREEN, MSG_BOX_FLAG_OK, RedrawSaveLoadScreenAfterMessageBox);
			return;
		}

		//if the user is selecting an unselected saved game slot
		if( gbSelectedSaveLocation != bSelected )
		{
			//Destroy the previous region
			DestroySaveLoadTextInputBoxes();

			gbSelectedSaveLocation = bSelected;

			//Reset the global string
			gzGameDescTextField[0] = '\0';

			//Init the text field for the game desc
			InitSaveLoadScreenTextInputBoxes();

			//If we are Loading the game
//			if( !gfSaveGame )
			{
				//Enable the save/load button
				EnableButton( guiSlgSaveLoadBtn );
			}

			//If we are saving the game, disbale the button
//			if( gfSaveGame )
//					DisableButton( guiSlgSaveLoadBtn );
//			else
			{
				//Set the time in which the button was first pressed
				uiLastTime = GetJA2Clock();
			}

			gfRedrawSaveLoadScreen = TRUE;

			uiLastTime = GetJA2Clock();
		}

		//the user is selecting the selected save game slot
		else
		{
			//if we are saving a game
			if( gfSaveGame )
			{
				//if the user is not currently editing the game desc
				if( !gfUserInTextInputMode )
				{
					if( ( uiCurTime - uiLastTime ) < SLG_DOUBLE_CLICK_DELAY )
					{
						//Load the saved game
						SaveLoadGameNumber();
					}
					else
					{
						uiLastTime = GetJA2Clock();
					}

					InitSaveLoadScreenTextInputBoxes();

					gfRedrawSaveLoadScreen = TRUE;

				}
				else
				{
					if (GetGameDescription())
					{
						SetActiveField(0);

						DestroySaveLoadTextInputBoxes();

//						gfRedrawSaveLoadScreen = TRUE;

//						EnableButton( guiSlgSaveLoadBtn );

						gfRedrawSaveLoadScreen = TRUE;


						if( ( uiCurTime - uiLastTime ) < SLG_DOUBLE_CLICK_DELAY )
						{
							gubSaveGameNextPass = 1;
						}
						else
						{
							uiLastTime = GetJA2Clock();
						}
					}
				}
			}
			//else we are loading
			else
			{
				if( ( uiCurTime - uiLastTime ) < SLG_DOUBLE_CLICK_DELAY )
				{
					//Load the saved game
					SaveLoadGameNumber();
				}
				else
				{
					uiLastTime = GetJA2Clock();
				}
			}
		}
	}
	else if (iReason & MSYS_CALLBACK_REASON_RBUTTON_UP)
	{
		DisableSelectedSlot();
	}
}


static void SelectedSaveRegionMovementCallBack(MOUSE_REGION* pRegion, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LOST_MOUSE )
	{
		INT8 bTemp = gbHighLightedLocation;
		gbHighLightedLocation = -1;
//		DisplaySaveGameList();
		DisplaySaveGameEntry( bTemp );
	}
	else if( reason & MSYS_CALLBACK_REASON_GAIN_MOUSE )
	{
		// if we are saving
		if (gfSaveGame)
		{
			// and this is not the zeroth page, leave
			if (sSaveBeginIndex != 0)
				return;

			// or, it is the zeroth page and not the quick save slot, leave too
			else if (MSYS_GetRegionUserData(pRegion, 0) + sSaveBeginIndex != 0)
				return;
		}

		gbHighLightedLocation = (UINT8)MSYS_GetRegionUserData( pRegion, 0 );
		DisplaySaveGameEntry( gbHighLightedLocation );//, usPosY );
	}
}


static void InitSaveLoadScreenTextInputBoxes(void)
{
	if (gbSelectedSaveLocation == -1)              return;
	if (!gfSaveGame)                               return;
	// If we are exiting, don't create the fields
	if (gfSaveLoadScreenExit)                      return;
	if (guiSaveLoadExitScreen != SAVE_LOAD_SCREEN) return;

	InitTextInputMode();
	SetTextInputCursor(CUROSR_IBEAM_WHITE);
	SetTextInputFont(FONT12ARIALFIXEDWIDTH);
	Set16BPPTextFieldColor(Get16BPPColor(FROMRGB(0, 0, 0)));
	SetBevelColors(Get16BPPColor(FROMRGB(136, 138, 135)), Get16BPPColor(FROMRGB(24, 61, 81)));
	SetTextInputRegularColors(FONT_WHITE, 2);
	SetTextInputHilitedColors(2, FONT_WHITE, FONT_WHITE);
	SetCursorColor(Get16BPPColor(FROMRGB(255, 255, 255)));

	AddUserInputField(NULL);

	// If we are modifying a previously modifed string, use it
	if (!gbSaveGameArray[gbSelectedSaveLocation + sSaveBeginIndex])
	{
		gzGameDescTextField[0] = '\0';
	}
	else if (gzGameDescTextField[0] == '\0')
	{
		SAVED_GAME_HEADER SaveGameHeader;
		LoadSavedGameHeader(gbSelectedSaveLocation, &SaveGameHeader);
		wcscpy(gzGameDescTextField, SaveGameHeader.sSavedGameDesc);
	}

	// Game Desc Field
	INT16 const x = SLG_FIRST_SAVED_SPOT_X + SLG_SAVE_GAME_DESC_X;
	INT16 const y = SLG_FIRST_SAVED_SPOT_Y + SLG_SAVE_GAME_DESC_Y - 5 + SLG_GAP_BETWEEN_LOCATIONS * gbSelectedSaveLocation;
	AddTextInputField(x, y, SLG_SAVELOCATION_WIDTH - SLG_SAVE_GAME_DESC_X - 7, 17, MSYS_PRIORITY_HIGH + 2, gzGameDescTextField, 46, INPUTTYPE_FULL_TEXT);
	SetActiveField(1);

	gfUserInTextInputMode = TRUE;
}


static void DestroySaveLoadTextInputBoxes(void)
{
	gfUserInTextInputMode = FALSE;
	KillAllTextInputModes();
	SetTextInputCursor( CURSOR_IBEAM );
}


static void SetSelection(UINT8 const new_selection)
{
	// If we are loading and there is no entry, return
 	if (!gfSaveGame && !gbSaveGameArray[new_selection + sSaveBeginIndex]) return;

	gfRedrawSaveLoadScreen = TRUE;
	DestroySaveLoadTextInputBoxes();

	INT8 const old_slot = gbSelectedSaveLocation;
	gbSelectedSaveLocation = new_selection;

	if (gfSaveGame && old_slot != new_selection)
	{
		DestroySaveLoadTextInputBoxes();

		// Null out the current description
		gzGameDescTextField[0] = '\0';

		//Init the text field for the game desc
		InitSaveLoadScreenTextInputBoxes();
	}

	EnableButton(guiSlgSaveLoadBtn);
}


static UINT8 CompareSaveGameVersion(INT8 bSaveGameID)
{
	UINT8 ubRetVal=SLS_HEADER_OK;

	SAVED_GAME_HEADER SaveGameHeader;

	//Get the heade for the saved game
	LoadSavedGameHeader( bSaveGameID, &SaveGameHeader );

	// check to see if the saved game version in the header is the same as the current version
	if( SaveGameHeader.uiSavedGameVersion != guiSavedGameVersion )
	{
		ubRetVal = SLS_SAVED_GAME_VERSION_OUT_OF_DATE;
	}

	if (strcmp(SaveGameHeader.zGameVersionNumber, g_version_number)!= 0)
	{
		if( ubRetVal == SLS_SAVED_GAME_VERSION_OUT_OF_DATE )
			ubRetVal = SLS_BOTH_SAVE_GAME_AND_GAME_VERSION_OUT_OF_DATE;
		else
			ubRetVal = SLS_GAME_VERSION_OUT_OF_DATE;
	}

	return( ubRetVal );
}


static void LoadSavedGameDeleteAllSaveGameMessageBoxCallBack(MessageBoxReturnValue);


static void LoadSavedGameWarningMessageBoxCallBack(MessageBoxReturnValue const bExitValue)
{
	// yes, load the game
  if( bExitValue == MSG_BOX_RETURN_YES )
	{
		//Setup up the fade routines
		StartFadeOutForSaveLoadScreen();
	}

	//The user does NOT want to continue..
	else
	{
		//ask if the user wants to delete all the saved game files
		DoSaveLoadMessageBox(zSaveLoadText[SLG_DELETE_ALL_SAVE_GAMES], SAVE_LOAD_SCREEN, MSG_BOX_FLAG_YESNO, LoadSavedGameDeleteAllSaveGameMessageBoxCallBack);
	}
}


static void DeleteAllSaveGameFile(void);


static void LoadSavedGameDeleteAllSaveGameMessageBoxCallBack(MessageBoxReturnValue const bExitValue)
{
	// yes, Delete all the save game files
  if( bExitValue == MSG_BOX_RETURN_YES )
	{
		DeleteAllSaveGameFile( );
		gfSaveLoadScreenExit = TRUE;
	}

	SetSaveLoadExitScreen( OPTIONS_SCREEN );

	gbSelectedSaveLocation=-1;
}


static void DeleteAllSaveGameFile(void)
{
	UINT8	cnt;

	for (sSaveBeginIndex = 0; sSaveBeginIndex < sNumSaveGames; sSaveBeginIndex += NUM_SAVE_GAMES)
	{
		for (cnt = 0; cnt < NUM_SAVE_GAMES; cnt++)
		{
			DeleteSaveGameNumber(cnt);
		}
	}

	sSaveBeginIndex = 0;
	sSaveEndIndex = NUM_SAVE_GAMES - 1;

	gGameSettings.bLastSavedGameSlot = -1;

	InitSaveGameArray();
}


void DeleteSaveGameNumber(UINT8 const save_slot_id)
{
	char filename[512];
	CreateSavedGameFileNameFromNumber(save_slot_id, filename);
	FileDelete(filename);
}


static void DisplayOnScreenNumber(BOOLEAN display)
{
	// Start at 1 - don't diplay it for the quicksave
	for (INT8 bLoopNum = 1; bLoopNum < NUM_SAVE_GAMES; ++bLoopNum)
	{
		const UINT16 usPosX = STD_SCREEN_X + 6;
		const UINT16 usPosY = SLG_FIRST_SAVED_SPOT_Y + SLG_GAP_BETWEEN_LOCATIONS * bLoopNum;

		BlitBufferToBuffer(guiSAVEBUFFER, FRAME_BUFFER, usPosX, usPosY + SLG_DATE_OFFSET_Y, 10, 10);

		if (display)
		{
			const INT8 bNum = (bLoopNum == 10 ? 0 : bLoopNum);
			wchar_t zTempString[16];
			swprintf(zTempString, lengthof(zTempString), L"%2d", bNum);
			DrawTextToScreen(zTempString, usPosX, usPosY + SLG_DATE_OFFSET_Y, 0, SAVE_LOAD_NUMBER_FONT, SAVE_LOAD_NUMBER_COLOR, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
		}

		InvalidateRegion(usPosX, usPosY + SLG_DATE_OFFSET_Y, usPosX + 10, usPosY + SLG_DATE_OFFSET_Y + 10);
	}
}


static void DoneFadeInForSaveLoadScreen(void);
static void FailedLoadingGameCallBack(MessageBoxReturnValue);


static void DoneFadeOutForSaveLoadScreen(void)
{
	// Make sure we DON'T reset the levels if we are loading a game
	gfHadToMakeBasementLevels = FALSE;

	try
	{
		LoadSavedGame(gbSelectedSaveLocation);

#ifdef JA2BETAVERSION
		ValidateSoldierInitLinks(1);
#endif
		gFadeInDoneCallback = DoneFadeInForSaveLoadScreen;

		ScreenID const screen = guiScreenToGotoAfterLoadingSavedGame;
		SetSaveLoadExitScreen(screen);
		if (screen == MAP_SCREEN)
		{ // We are to go to map screen after loading the game
			FadeInNextFrame();
		}
		else
		{ // We are to go to the Tactical screen after loading
			PauseTime(FALSE);
			FadeInGameScreen();
		}
	}
	catch (std::exception const& e)
	{
		wchar_t msg[512];
		swprintf(msg, lengthof(msg), zSaveLoadText[SLG_LOAD_GAME_ERROR], e.what());
		DoSaveLoadMessageBox(msg, SAVE_LOAD_SCREEN, MSG_BOX_FLAG_OK, FailedLoadingGameCallBack);
		NextLoopCheckForEnoughFreeHardDriveSpace();
	}
	gfStartedFadingOut = FALSE;
}


static void DoneFadeInForSaveLoadScreen(void)
{
	//Leave the screen
	//if we are supposed to stay in tactical, due nothing,
	//if we are supposed to goto mapscreen, leave tactical and go to mapscreen

	if( guiScreenToGotoAfterLoadingSavedGame == MAP_SCREEN )
	{
		if( !gfPauseDueToPlayerGamePause )
		{
			UnLockPauseState( );
			UnPauseGame( );
		}
	}

	else
	{
		//if the game is currently paused
		if( GamePaused() )
		{
			//need to call it twice
			HandlePlayerPauseUnPauseOfGame();
			HandlePlayerPauseUnPauseOfGame();
		}

//		UnLockPauseState( );
//		UnPauseGame( );

	}
}


static void SelectedSLSEntireRegionCallBack(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_RBUTTON_UP)
	{
		DisableSelectedSlot();
	}
}


static void DisableSelectedSlot(void)
{
	//reset selected slot
	gbSelectedSaveLocation = -1;
	gfRedrawSaveLoadScreen = TRUE;
	DestroySaveLoadTextInputBoxes();

	if( !gfSaveGame )
		DisableButton( guiSlgSaveLoadBtn );

	//reset the selected graphic
	ClearSelectedSaveSlot();
}


static void ConfirmSavedGameMessageBoxCallBack(MessageBoxReturnValue const bExitValue)
{
	Assert( gbSelectedSaveLocation != -1 );

  if( bExitValue == MSG_BOX_RETURN_YES )
	{
		SaveGameToSlotNum();
	}
}


#ifdef JA2BETAVERSION

void ErrorDetectedInSaveCallback(MessageBoxReturnValue)
{
	gFadeInDoneCallback = DoneFadeInForSaveLoadScreen;
	ScreenID const screen = guiScreenToGotoAfterLoadingSavedGame;
	SetSaveLoadExitScreen(screen);
	if (screen == MAP_SCREEN)
	{ // We are to go to map screen after loading the game
		FadeInNextFrame();
	}
	else
	{ // We are to go to the Tactical screen after loading
		FadeInGameScreen();
	}
	gfStartedFadingOut = TRUE;
}

#endif


static void FailedLoadingGameCallBack(MessageBoxReturnValue const bExitValue)
{
	// yes
  if( bExitValue == MSG_BOX_RETURN_OK )
	{
		//if the current screen is tactical
		if( guiPreviousOptionScreen == MAP_SCREEN )
		{
			SetPendingNewScreen( MAINMENU_SCREEN );
		}
		else
		{
			LeaveTacticalScreen( MAINMENU_SCREEN );
		}

		SetSaveLoadExitScreen( MAINMENU_SCREEN );


		//We want to reinitialize the game
		ReStartingGame();
	}
}


void DoQuickSave()
{
	if (SaveGame(-1, L"")) return;

	if (guiPreviousOptionScreen == MAP_SCREEN)
		DoMapMessageBox(MSG_BOX_BASIC_STYLE, zSaveLoadText[SLG_SAVE_GAME_ERROR], MAP_SCREEN, MSG_BOX_FLAG_OK, NULL);
	else
		DoMessageBox(MSG_BOX_BASIC_STYLE, zSaveLoadText[SLG_SAVE_GAME_ERROR], GAME_SCREEN, MSG_BOX_FLAG_OK, NULL, NULL);
}


void DoQuickLoad()
{
	// If there is no save in the quick save slot
	InitSaveGameArray();
	if (!gbSaveGameArray[0]) return;

	// Set the selection to be the quick save slot
	gbSelectedSaveLocation = 0;

	StartFadeOutForSaveLoadScreen();
	gfDoingQuickLoad = TRUE;
}


bool AreThereAnySavedGameFiles()
{
	InitSaveGameArray();

	INT16 sSaveBeginIndexTmp = sSaveBeginIndex;

	BOOLEAN gbSavedGameExists = FALSE;

	char filename[512];

	for (sSaveBeginIndex = 0; sSaveBeginIndex < sNumSaveGames; sSaveBeginIndex += NUM_SAVE_GAMES)
	{
		for (INT8 i = 0; i != NUM_SAVE_GAMES; ++i)
		{
			CreateSavedGameFileNameFromNumber(i, filename);
			if (GCM->doesGameResExists(filename))
			{
				gbSavedGameExists = TRUE;
				break;
			}
		}
		if (gbSavedGameExists)
			break;
	}
	
	sSaveBeginIndex = sSaveBeginIndexTmp;

	return gbSavedGameExists;
}


static void RedrawSaveLoadScreenAfterMessageBox(MessageBoxReturnValue const bExitValue)
{
	gfRedrawSaveLoadScreen = TRUE;
}


static void MoveSelectionDown()
{
	INT8 const slot = gbSelectedSaveLocation;
	if (gfSaveGame)
	{ // We are saving, any slot other then the quick save slot is valid
		if (slot == -1)
		{
			SetSelection(1);
		}
		else if (slot < NUM_SAVE_GAMES - 1)
		{
			// move selection down if there is a save in the next slot (slot + 1, overwriting an old save)
			// or if the save existed in the current slot (creating a new save)
			if (gbSaveGameArray[slot + 1 + sSaveBeginIndex] || gbSaveGameArray[slot + sSaveBeginIndex])
				SetSelection(slot + 1);
		}
		else if (slot == 10)
		{
			GetNextPage(SDLK_DOWN);
		}

	}
	else
	{
		INT32 i = slot != -1 ? slot + 1 : 0;
		if (i >= NUM_SAVE_GAMES)
			GetNextPage(SDLK_DOWN);
		else if (gbSaveGameArray[i + sSaveBeginIndex])
			SetSelection(i);
	}
}


static void MoveSelectionUp()
{
	INT8 const slot = gbSelectedSaveLocation;
	if (gfSaveGame)
	{ // We are saving, any slot other then the quick save slot is valid
		if (slot == -1)
		{
			SetSelection(NUM_SAVE_GAMES - 1);
		}
		else if (slot > 1)
		{
			SetSelection(slot - 1);
		}
		else if (sSaveBeginIndex > 0)
		{
			if (slot > 0)
				SetSelection(slot - 1);
			else if (slot == 0)
				GetPrevPage(SDLK_UP);
		}
	}
	else
	{
		INT32 i = slot != -1 ? slot - 1 : NUM_SAVE_GAMES - 1;
		if (i < 0)
			GetPrevPage(SDLK_UP);
		else if (gbSaveGameArray[i + sSaveBeginIndex])
			SetSelection(i);
	}
}


static void GetPrevPage(SDLKey Key /* = SDLK_PAGEUP */)
{
	if (sSaveBeginIndex - NUM_SAVE_GAMES >= 0)
	{
		sSaveBeginIndex -= NUM_SAVE_GAMES;
		sSaveEndIndex -= NUM_SAVE_GAMES;

		GetPrevNextPageFlush(Key);
	}
	else if (gbSelectedSaveLocation != 0 && gbSaveGameArray[0] && !gfSaveGame)
		SetSelection(0);
	else if (gbSelectedSaveLocation != 1 && gbSaveGameArray[1] && gfSaveGame)
		SetSelection(1);
}


static void GetNextPage(SDLKey Key /* = SDLK_PAGEDOWN */)
{
	// go to the next page if there's more to show
	// or if we're saving and the next empty slot won't fit the current page
	if ((sSaveEndIndex < sNumSaveGames - 1) || 
		(gfSaveGame && sSaveEndIndex == sNumSaveGames - 1))
	{
		sSaveBeginIndex += NUM_SAVE_GAMES;
		sSaveEndIndex += NUM_SAVE_GAMES;

		GetPrevNextPageFlush(Key);
	}
	else 
	{
		INT16 last_slot;

		for (last_slot = NUM_SAVE_GAMES - 1; !gbSaveGameArray[last_slot + sSaveBeginIndex]; last_slot--);
		
		if (!gfSaveGame)
		{
			if (last_slot != gbSelectedSaveLocation)
				SetSelection(last_slot);
		}
		else
		{
			if (last_slot < NUM_SAVE_GAMES - 1)
			{
				// don't set the next empty slot if it was already set
				if (last_slot + 1 != gbSelectedSaveLocation)
					SetSelection(last_slot + 1);
			}
		}
	}
}


static void GetPrevNextPageFlush(SDLKey Key)
{
	EnableDisablePrevNextPageBtns();

	// disable regions which are no longer hoverable
	UINT16 const x = SLG_FIRST_SAVED_SPOT_X;
	UINT16       y = SLG_FIRST_SAVED_SPOT_Y;

	for (INT16 i = 0; i != NUM_SAVE_GAMES; ++i)
	{
		MOUSE_REGION& r = gSelectedSaveRegion[i];

		// We cannot load a game that has not been saved
		if (!gfSaveGame) 
		{ 
			if (gbSaveGameArray[i + sSaveBeginIndex]) r.Enable(); 
			else r.Disable();
		}
		else
		{
			// zeroth (quick save, on the zeroth page) slot should be enabled only if not on the zeroth page
			if (i == 0 && sSaveBeginIndex != 0)
				r.Enable();

			else if (gbSaveGameArray[i + sSaveBeginIndex])
				r.Enable();

			// make the next empty slot clickable
			// going all the way to the last slot
			else if (i > 0)
			{
				// looking at the previous slot
				if (gbSaveGameArray[i - 1 + sSaveBeginIndex])
					r.Enable();
				else
					r.Disable();
			}

			else r.Disable();
		}
		y += SLG_GAP_BETWEEN_LOCATIONS;
	}
	

	// we've come to a new page, let's check if current save slot is highlightable
	if (!gbSaveGameArray[gbHighLightedLocation + sSaveBeginIndex])
		gbHighLightedLocation = -1;


	// the selection might also need some shifting
	if (Key == SDLK_UP)
		SetSelection(NUM_SAVE_GAMES - 1);
	else if (Key == SDLK_DOWN)
		SetSelection(0);
	else if (!gbSaveGameArray[gbSelectedSaveLocation + sSaveBeginIndex])
	{
		INT16 last_slot;

		for (last_slot = NUM_SAVE_GAMES - 1; 
			 !gbSaveGameArray[last_slot + sSaveBeginIndex] && last_slot != -1; 
			 last_slot--);

		if (!gfSaveGame)
		{
			if (last_slot != gbSelectedSaveLocation)
				SetSelection(last_slot);
		}
		else
		{
			if (last_slot < NUM_SAVE_GAMES - 1)
			{
				// don't set the next empty slot if it was already set
				if (last_slot + 1 != gbSelectedSaveLocation)
					SetSelection(last_slot + 1);
				else
				{
					ResetSaveLoadTextInputBoxes();
				}
			}
		}
	}
	else if (gfSaveGame)
	{
		// if on the first page, the quick save slot should not be selectable from the save screen
		if (sSaveBeginIndex == 0 && gbSelectedSaveLocation == 0)
		{
			if (gbSaveGameArray[1])
			{
				SetSelection(1);
			}
		}
		else
		{	
			// otherwise reload the text fields
			ResetSaveLoadTextInputBoxes();
		}
	}

	RenderSaveLoadScreen();
	MarkButtonsDirty();
	RenderButtons();
}


static void ResetSaveLoadTextInputBoxes(void) 
{
	DestroySaveLoadTextInputBoxes();

	// Null out the current description
	gzGameDescTextField[0] = '\0';

	//Init the text field for the game desc
	InitSaveLoadScreenTextInputBoxes();
}


static void ClearSelectedSaveSlot(void)
{
	gbSelectedSaveLocation = -1;
}


static void SaveGameToSlotNum(void)
{
	//Redraw the save load screen
	RenderSaveLoadScreen();

	//render the buttons
	MarkButtonsDirty( );
	RenderButtons();

	if( !SaveGame( gbSelectedSaveLocation, gzGameDescTextField ) )
	{
		DoSaveLoadMessageBox(zSaveLoadText[SLG_SAVE_GAME_ERROR], SAVE_LOAD_SCREEN, MSG_BOX_FLAG_OK, NULL);
	}

	SetSaveLoadExitScreen( guiPreviousOptionScreen );
}


static void StartFadeOutForSaveLoadScreen(void)
{
	//if the game is paused, and we are in tactical, unpause
	if( guiPreviousOptionScreen == GAME_SCREEN )
	{
		PauseTime( FALSE );
	}

	gFadeOutDoneCallback = DoneFadeOutForSaveLoadScreen;

	FadeOutNextFrame( );
	gfStartedFadingOut = TRUE;
}
