/**************************************************************************/
/*                                                                        */
/*  Menu.cpp  -  Christian Vigh, 95/10/18.                                */
/*                                                                        */
/*  Programme de test pour le menu Cheops.                                */
/*                                                                        */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include	"MNCommon.h"
# include	"ChopCfg.h"
# include	"ChopJet.h"
# include	"ChopTree.h"
# include	"ChopUp.h"
# include	"ChopMenu.h"



/**************************************************************************/
/*                                                                        */
/*  Application.                                                          */
/*                                                                        */
/**************************************************************************/


class	TTestMenuApplication	:  public  TApplication
    {
	public :
		TTestMenuApplication	( LPSTR  	name ) 
			: TApplication ( name )
		   { }

		virtual void	InitMainWindow		( void ) ;
     } ;



TCheopsMenuWindow *	CheopsMenuWindow ;
TTestMenuApplication *	TestMenuApplication ;



TCheopsUserConfig  	UserConfiguration  =
   {
	"Chartier Informatique",
	"laser",
	1995,
	7,
	"/users/ci8/bin",
	"/users/descrip/ci8",
	"/donnees/evron"
     } ;



char * __printers [] = 
	{ "lp", "laser", "entree" } ;


		   
TCheopsUserInfo		UserInfo  =
   {
	"Vigh",
	"tty2a",
	TRUE,
	__printers,
	3,
    } ;



TCheopsModule		CheopsModules [ ] =
  {
	{ "pers"  , "Carthage"  , RGB (  69, 138, 143 ) },
	{ "entree", "Thèbes"    , RGB ( 174,  39,  32 ) },
	{ "eco"   , "Syracuse"	, RGB ( 200, 190,   4 ) },
	{ "compta", "Jéricho"   , RGB (  28, 174,  79 ) },
	{ "param" , "Paramètres", -1 }

    } ;

int			CheopsModuleCount	=  sizeof ( CheopsModules ) /
						   sizeof ( TCheopsModule ) ;






/**************************************************************************/
/*                                                                        */
/*  TTestMenuApplication :: InitMainWindow  :                             */
/*      Crée la fenêtre principale de l'application.                      */
/*                                                                        */
/**************************************************************************/

void	TTestMenuApplication :: InitMainWindow  ( void )
   {
// Créer l'objet configuration
	CheopsConfiguration = new TCheopsConfiguration ;
	
// Création de la fenêtre principale
	CheopsMenuWindow = new  TCheopsMenuWindow ( 0, "Cheops pour Windows" ) ;
	SetMainWindow ( CheopsMenuWindow ) ;

// Ajouter les infos utilisateur
	CheopsMenuWindow -> SetUserInfo ( UserInfo, UserConfiguration ) ;

// Puis la liste des modules
	CheopsMenuWindow -> SetModules  ( CheopsModules, CheopsModuleCount ) ;

// Et enfin la hiérarchie de menus
	CheopsMenuWindow -> BuildMenuTree ( fopen ( "/terminal/bin/dev/Menu.lst", "r" ) ) ;


	nCmdShow = SW_SHOWMAXIMIZED ;	// OWL fait le Create et le Show pour nous...
    }





/**************************************************************************/
/*                                                                        */
/*   Programme principal.                                                 */
/*                                                                        */
/**************************************************************************/

int	OwlMain  ( int  /* argc */, char **  /* argv */ )
   {
	register int		status ;


	TestMenuApplication =  new  TTestMenuApplication ( "TESTMENU" ) ;
	status = TestMenuApplication -> Run ( ) ;
	delete  TestMenuApplication ;
	

	return ( status ) ;
    }
