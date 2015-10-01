/**************************************************************************/
/*                                                                        */
/*  Keypad.cpp  -  Christian Vigh, 94/09/24.                              */
/*                                                                        */
/*  Implémentation de la classe TFunctionKeypad.                          */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop




# define	FUNCTION_KEY_WIDTH		80
# define	VINTERVAL			3
# define	HINTERVAL			5
# define	FUNCTION_KEY_ID			( Resource_Base + 2000 )




DEFINE_RESPONSE_TABLE1 ( TFunctionKeypad, TWindow )
	EV_COMMAND ( FUNCTION_KEY_ID +  0, F1  ),
	EV_COMMAND ( FUNCTION_KEY_ID +  1, F2  ),
	EV_COMMAND ( FUNCTION_KEY_ID +  2, F3  ),
	EV_COMMAND ( FUNCTION_KEY_ID +  3, F4  ),
	EV_COMMAND ( FUNCTION_KEY_ID +  4, F5  ),
	EV_COMMAND ( FUNCTION_KEY_ID +  5, F6  ),
	EV_COMMAND ( FUNCTION_KEY_ID +  6, F7  ),
	EV_COMMAND ( FUNCTION_KEY_ID +  7, F8  ),
	EV_COMMAND ( FUNCTION_KEY_ID +  8, F9  ),
	EV_COMMAND ( FUNCTION_KEY_ID +  9, F10 ),
	EV_COMMAND ( FUNCTION_KEY_ID + 10, F11 ),
	EV_COMMAND ( FUNCTION_KEY_ID + 11, F12 ),
END_RESPONSE_TABLE ;



/**************************************************************************/
/*                                                                        */
/*  Tableau des touches de fonction.                                      */
/*                                                                        */
/**************************************************************************/

static TFunctionKey	__FunctionKeys [ MAX_FUNCTION_KEYS ] =
   {
	{ "F1" , VK_F1 , NULL },
	{ "F2" , VK_F2 , NULL },
	{ "F3" , VK_F3 , NULL },
	{ "F4" , VK_F4 , NULL },
	{ "F5" , VK_F5 , NULL },
	{ "F6" , VK_F6 , NULL },
	{ "F7" , VK_F7 , NULL },
	{ "F8" , VK_F8 , NULL },
	{ "F9" , VK_F9 , NULL },
	{ "F10", VK_F10, NULL },
	{ "F11", VK_F11, NULL },
	{ "F12", VK_F12, NULL },
      } ;




/**************************************************************************/
/*                                                                        */
/*  TFunctionKeypad :: TFunctionKeypad                                    */
/*      Crée les boutons.                                                 */
/*                                                                        */
/**************************************************************************/

TFunctionKeypad :: TFunctionKeypad  ( TDisplayDevice *	device )
		: TWindow ( device -> TerminalWindowClient ) 
   {
	register int	width, height ;


	Disabled	=  TRUE ;
	Device		=  device ;

	width		=  Device -> CharacterWidth  * Device -> ScreenWidth ;
	height		=  Device -> CharacterHeight * Device -> ScreenHeight ;

// Attributs de la fenêtre
	Attr. Style     =  WS_CHILD ;
	Attr. X		=  width ;
	Attr. Y		=  0 ;
	Attr. W		=  FUNCTION_KEY_WIDTH + 1 + ( 2 * HINTERVAL ) ;
	Attr. H		=  height ;


// Allocation des touches de fonction
	FunctionKeys = new  TFunctionKey [ sizeof ( __FunctionKeys ) /
						sizeof ( TFunctionKey ) ] ;
	memcpy ( FunctionKeys, __FunctionKeys, sizeof ( __FunctionKeys ) ) ;
     }




/**************************************************************************/
/*                                                                        */
/*   TFunctionKeypad :: SetupWindow                                       */
/*      Crée les boutons                                                  */
/*                                                                        */
/**************************************************************************/

void	TFunctionKeypad :: SetupWindow  ( void )
   {
	int			height, interval, button_height ;


	TWindow :: SetupWindow ( ) ;
	

// Calcul de la largeur :
//	. 1 pour le trait vertical	+
//	. 2 * HINTERVAL (de chaque côté des boutons)    	+
//	. FUNCTION_KEY_WIDTH
	Width	=  1 + ( 2 * HINTERVAL ) + FUNCTION_KEY_WIDTH ;

	height		=  Device -> CharacterHeight * Device -> ScreenHeight ;
	interval	=  ( MAX_FUNCTION_KEYS + 1 ) * VINTERVAL ;
	button_height	=  ( height - interval ) / MAX_FUNCTION_KEYS ;


// Création des boutons
	for  ( int  i = 0 ; i < MAX_FUNCTION_KEYS ; i ++ )
	   {
		FunctionKeys [i]. Button = new  TButton ( this,
			FUNCTION_KEY_ID + i, FunctionKeys [i]. Label,
				/* x */  1 + HINTERVAL,
				/* y */  1 + ( ( VINTERVAL + button_height ) * ( i + 1 ) ) - button_height,
				/* w */  2 + FUNCTION_KEY_WIDTH,
				/* h */  2 + button_height ) ;
		FunctionKeys [i]. Button -> Attr. Style = WS_CHILD ;
		FunctionKeys [i]. Button -> Create ( ) ;
	     }
    }




/**************************************************************************/
/*                                                                        */
/*  TFunctionKeypad :: ResizeWindow -                                     */
/*      Ajuste la taille des touches de fonction par rapport à la taille  */
/*      actuelle de la fenêtre Terminal.                                  */
/*                                                                        */
/**************************************************************************/

void	TFunctionKeypad :: ResizeWindow  ( void )
   {
	register int	width  	=  Device -> CharacterWidth  * Device -> ScreenWidth,
			height	=  Device -> CharacterHeight * Device -> ScreenHeight ;
	register int	i,
			interval,
			button_height ;




	SetWindowPos ( 0, width, 0, 0, 0, 
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE ) ;

	interval	=  ( MAX_FUNCTION_KEYS + 1 ) * VINTERVAL ;
	button_height	=  ( height - interval ) / MAX_FUNCTION_KEYS ;

	for  ( i = 0 ; i < MAX_FUNCTION_KEYS ; i ++ )
		FunctionKeys [i]. Button -> SetWindowPos ( 0,
				/* x */  1 + HINTERVAL,
				/* y */  1 + ( ( VINTERVAL + button_height ) * ( i + 1 ) ) - button_height,
				/* w */  2 + FUNCTION_KEY_WIDTH,
				/* h */  2 + button_height,
				SWP_NOZORDER | SWP_NOACTIVATE ) ;

     }





/**************************************************************************/
/*                                                                        */
/*   TFunctionKeypad :: ~TFunctionKeypad                                  */
/*      Supprime les boutons.                                             */
/*                                                                        */
/**************************************************************************/

TFunctionKeypad :: ~TFunctionKeypad  ( )
   {
	register int	i ;


	for  ( i = 0 ; i < MAX_FUNCTION_KEYS ; i ++ )
	   {
		delete  FunctionKeys [i]. Button ;
	    }

	delete []  FunctionKeys ;
    }




/**************************************************************************/
/*                                                                        */
/*  TFunctionKeypad :: Enable, Disable :                                  */
/*      Affiche/cache le pavé de touches de fonction.                     */
/*                                                                        */
/**************************************************************************/

void	TFunctionKeypad :: PaintRectangle ( void )
   {
	TRect		Rect ;
	HDC		WindowDC ;
	HBRUSH		RectangleBrush ;


	if  ( Disabled )
		return ;

// Puis tracer une ligne de séparation
	TerminalWindow -> GetClientRect ( Rect ) ;

	Rect. left ++ ;
	Rect. top  -- ;
	Rect. right ++ ;
	Rect. bottom -- ;

	WindowDC = GetDC ( HWindow ) ;
	RectangleBrush = :: CreateSolidBrush ( GetSysColor ( COLOR_WINDOWFRAME ) ) ;
	:: FrameRect ( WindowDC, & Rect, RectangleBrush ) ;
	DeleteObject ( RectangleBrush ) ;
	ReleaseDC ( HWindow, WindowDC ) ;
    }



void	TFunctionKeypad :: Enable  ( void )
   {
	Disabled = FALSE ;
	Device -> AdjustWindowSize ( ) ;

	Show ( SW_SHOW ) ;
	PaintRectangle ( ) ;

	for  ( int  i = 0 ; i < MAX_FUNCTION_KEYS ; i ++ )
		FunctionKeys [i]. Button -> ShowWindow ( SW_SHOW ) ;

	Invalidate ( ) ;
	UpdateWindow ( ) ;
    }


void	TFunctionKeypad :: Disable  ( void )
   {
	Disabled = TRUE ;
	Device -> AdjustWindowSize ( ) ;   
	Show ( SW_HIDE ) ;
	
	for  ( int  i = 0 ; i < MAX_FUNCTION_KEYS ; i ++ )
		FunctionKeys [i]. Button -> ShowWindow ( SW_HIDE ) ;
    }

    
   

/**************************************************************************/
/*                                                                        */
/*  TFunctionKeypad :: SetText                                            */
/*      Définit le label de la touche de fonction spécifiée.              */
/*                                                                        */
/**************************************************************************/

void	TFunctionKeypad :: SetText  ( int  key, char * label )
   {
	if  ( key >= 0  &&  key < MAX_FUNCTION_KEYS )
	   {
		register TFunctionKey *		fk	=  FunctionKeys + key ;

		
		strncpy ( fk -> Label,  label, MAX_LABEL_LENGTH ) ;
		fk -> Label [ MAX_LABEL_LENGTH ] = 0 ;
		fk -> Button -> SetWindowText ( fk -> Label ) ;
	    }
    }




     
/**************************************************************************/
/*                                                                        */
/*  TFunctionKeypad :: Command                                            */
/*      Relaie les appuis sur les boutons vers la fenêtre principale.     */
/*                                                                        */
/**************************************************************************/

void	TFunctionKeypad :: Command  ( WPARAM  id )
   {

// Rendre le focus à la fenêtre mère...
	:: SetFocus ( Parent -> HWindow ) ;
	:: SetActiveWindow ( Parent -> HWindow ) ;

// Et lui indiquer quelle touche a été pressée
	TerminalWindow -> HandleMessage (
		WM_KEYDOWN, FunctionKeys [id]. VirtualKeyCode, 0L ) ;
	TerminalWindow -> HandleMessage (
		WM_KEYUP, FunctionKeys [id]. VirtualKeyCode, 0L ) ;
    }                                                                
