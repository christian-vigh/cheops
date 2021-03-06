/**************************************************************************/
/*                                                                        */
/*  MenuDet.cpp  -  Christian Vigh, 95/10.                                */
/*                                                                        */
/*  Impl�mentation de la classe TMenuDetachGadget.                        */
/*                                                                        */
/**************************************************************************/


# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"

# define	BITMAP_RASTEROP 		0x810975L




/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/


TMenuDetachGadget :: TMenuDetachGadget  ( )
	: TGadget ( -1 )
   {
	Pressed 	= FALSE ;
	SavedCursor 	= 0 ;
	Bitmap 		= new  TBitmap ( * :: Module, TResId ( "NEEDLE_BITMAP" ) ) ;
	Bitmap -> GetObject ( BitmapInfo ) ;
	Height  	= BitmapInfo. bmHeight + 
				( 2 * GetSystemMetrics ( SM_CYBORDER ) ) ;
   }




/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TMenuDetachGadget :: ~TMenuDetachGadget ( )
   {
	if  ( Bitmap )
		delete  Bitmap ;
    }




/**************************************************************************/
/*                                                                        */
/*   TDetachMenuGadget :: GetDesiredSize -                                */
/*      Retourne la taille d�sir�e.                                       */
/*                                                                        */
/**************************************************************************/

void	TMenuDetachGadget :: GetDesiredSize  ( TSize &  Size )
   {
	Size. cx = 0 ;
	Size. cy = Height ;
    }

	


/**************************************************************************/
/*                                                                        */
/*  TMenuDetachGadget :: Paint -                                          */
/*      Dessine le gadget.                                                */
/*                                                                        */
/**************************************************************************/

void	TMenuDetachGadget :: Paint  ( TDC &  dc ) 
   {
	TRect			PaintRect ( 0, 0, Bounds. Width ( ),
						Bounds. Height ( ) ) ;
	COLORREF		Color = ( Pressed ) ?
						GetSysColor ( COLOR_BTNSHADOW ) :
						GetSysColor ( COLOR_BTNFACE ) ;
	TBrush			FillBrush ( Color ) ;


	dc. FillRect ( PaintRect, FillBrush ) ;

	if  ( Bitmap )
	  {
		TMemoryDC	MemDC ;
		TBrush		Brush  ( RGB ( 255, 255, 255 ) ) ;

		MemDC. SelectObject ( * Bitmap ) ;
		dc. SelectObject ( Brush ) ;

	// Le code ROP utilis� a �t� trouv� au pif 
	// En fait, tous les codes commen�ant par 8 donnent le m�me r�sultat
	// Le seul petit probl�me, c'est que les deux rouges de l'image sont
	// remplac�s par un seul
		dc. BitBlt ( 4 * GetSystemMetrics ( SM_CXBORDER ),	
			     GetSystemMetrics ( SM_CYBORDER ),
				BitmapInfo. bmWidth,
				BitmapInfo. bmHeight,
				MemDC, 0, 0,
				BITMAP_RASTEROP ) ;

		dc. RestoreBrush ( ) ;
		MemDC. RestoreBitmap ( ) ;
	   }
     }





/**************************************************************************/
/*                                                                        */
/*  TMenuDetachGadget :: LButtonDown -                                    */
/*      Change l'apparence du curseur et pr�pare le d�placement.          */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

void	TMenuDetachGadget :: LButtonDown ( uint  modkeys, TPoint &  pos )
   {
	HCURSOR  Cursor = :: LoadCursor ( * :: Module, TResId ( "DETACH_CURSOR" ) ) ;


	TGadget :: LButtonDown  ( modkeys, pos ) ;
	Pressed = TRUE ;

// Changer le curseur d'apparence
	if  ( Cursor ) 
		SavedCursor =:: SetCursor ( Cursor ) ;

// R�cup�rer la position du curseur et calculer la diff�rence entre cette position
// et l'origine du gadget
	TRect		WindowRect 	=  Window -> GetWindowRect ( ) ;


	:: GetCursorPos  ( & MousePosition ) ;
	DeltaX = MousePosition. x - WindowRect. left ;
	DeltaY = MousePosition. y - WindowRect. top ;



// Capturer les messages souris
	Window -> GadgetSetCapture ( * this ) ;
	DrawSelection ( MousePosition ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TDetachMenuGadget :: LButtonUp -                                      */
/*      Appelle la fonction EvDrop de la fen�tre gadget si le point       */
/*      de rel�chement est en dehors de cette fen�tre.                    */
/*                                                                        */
/**************************************************************************/

void	TMenuDetachGadget :: LButtonUp  ( uint  modkeys,  TPoint &  pos )
   {
	TGadget :: LButtonUp ( modkeys, pos ) ;

	if  ( ! Pressed ) 
		return ;

	Pressed = FALSE ;

	if  ( SavedCursor )
	   {
		:: SetCursor ( SavedCursor ) ;
		SavedCursor = 0 ;
	    }

	Window -> GadgetReleaseCapture ( * this ) ;
	DrawSelection ( MousePosition ) ;	// effacer le cadre
	

// V�rifier si le point de rel�chement est bien en dehors de la fen�tre gadget
	TPoint		CursorPos ;
	TRect		WindowRect	=  Window -> GetWindowRect ( ) ;


	:: GetCursorPos ( & CursorPos ) ;
	CursorPos. x  -=  DeltaX ;
	CursorPos. y  -=  DeltaY ;


//	if  ( WindowRect. Contains ( CursorPos ) )
//		return ;	// Mauvaise pioche

// Le point d'arriv�e est bien en dehors de la fen�tre : on la pr�vient
// (dans la mesure o� il s'agit d'une TGadgetMenuWindow
	register TGadgetMenuWindow *	w	=  TYPESAFE_DOWNCAST ( Window, TGadgetMenuWindow ) ;

	if  ( w )
		w -> HandleMessage ( WM_MENUDROPPED, 0, ( LPARAM ) & CursorPos ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TMenuDetachGadget :: MouseMove -                                      */
/*      D�place le rectangle s'il s'agit d'un glisser-d�placer.           */
/*                                                                        */
/**************************************************************************/

void	TMenuDetachGadget :: MouseMove  ( uint  modkeys, TPoint &  pos )
   {
	TGadget :: MouseMove ( modkeys, pos ) ;


	if  ( Pressed )
	   {
		TPoint		CursorPos ;

		:: GetCursorPos ( & CursorPos ) ;


		if  ( CursorPos  !=  MousePosition )
		   {
			DrawSelection ( MousePosition ) ;	// Effacer le rectangle pr�c�dent
			DrawSelection ( CursorPos ) ;		// Dessiner le nouveau
			MousePosition = CursorPos ;  		// et sauvegarder sa position
		     }
	     }
      }






/**************************************************************************/
/*                                                                        */
/*  TMenuDetachGadget :: DrawSelection -                                  */
/*      Trace une s�lection � la position sp�cifi�e.                      */
/*                                                                        */
/**************************************************************************/

void	TMenuDetachGadget :: DrawSelection  ( TPoint &  pos )
   {
	TRect		Rect	=  Window -> GetWindowRect ( ) ;
	TPen		Pen ( RGB ( 255, 255, 255 ) ) ;
	TScreenDC	DC ;



// Calculer le rectangle de tra�age
	TRect		Frame ( pos. x, pos. y,
				pos. x + Rect. Width ( ),
				pos. y + Rect. Height ( ) ) ;
	int		ROP2 = DC. GetROP2 ( ) ;	// Sauvegarder l'ancien mode raster


// Ajuster l'origine du machin
	Frame. Offset ( - DeltaX, - DeltaY ) ;

// Tracer une bordure de la taille de la fen�tre menu
	DC. SetROP2 ( R2_XORPEN ) ;
	DC. SelectObject ( Pen ) ;
	DC. SelectStockObject ( NULL_BRUSH ) ;		// Pour ne pas peindre l'int�rieur du rectangle
	DC. Rectangle ( Frame ) ;
	DC. RestorePen ( ) ;
	DC. RestoreBrush ( ) ;
	DC. SetROP2 ( ROP2 ) ;
    }
		 

