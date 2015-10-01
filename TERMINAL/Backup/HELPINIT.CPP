/**************************************************************************/
/*                                                                        */
/*  HelpInit.cpp  -  Christian Vigh, 95/08/11.                            */
/*                                                                        */
/*  Définition des structures ^pour l'initialisation de l'aide.           */
/*                                                                        */
/**************************************************************************/



# include	"AllTerm.h"
# pragma	hdrstop

# include	"TermHelp.rh"



/**************************************************************************/
/*                                                                        */
/*   InitializeHelp -                                                     */
/*      Charge les contextes pour l'aide en ligne.                        */
/*                                                                        */
/**************************************************************************/

static  THelp :: THotkey	hot [ ]  =
   {
	{ VK_F1, THelp :: Control, THelp :: Normal },
	{ VK_F1, THelp :: Shift  , THelp :: Contextual }
    } ;




void	InitializeHelp  ( void )
   {

// Hotkeys
	Help -> SetHotkeys ( hot, sizeof ( hot ) / sizeof ( THelp :: THotkey ) ) ;


/***
	La HelpWindowProc de THelp cause une GPF lorsque l'appli est lancée 
	depuis l'IDE, et qu'elle est désactivée au profit d'une autre tâche
	 (la GPF provient de l'IDE, non de TERMINAL.EXE).
	Cette GPF se produit lorsque la proc accède à TheHelpObject, qui n'est 
	pas NULL....
	Conclusion : il semblerait que l'ide se mélange un peu les crayons
	avec les segments de données.

// Contextes
	Help -> AddHelp ( TerminalWindow, Configuration -> HelpFileName,
				TResId ( IDH_TERMINAL_WINDOW ) ) ;

// Pour la classe TGeneralOptionsDialog
	Help -> AddHelp ( "TGeneralOptionsDialog", Configuration -> HelpFileName,
				TResId ( IDH_GENERAL_OPTIONS_DIALOG ) ) ;
 ***/
     }	
