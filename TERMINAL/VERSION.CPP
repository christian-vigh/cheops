/**************************************************************************/
/*                                                                        */
/*  Version.cpp  -  Christian Vigh, 95/07/27.                             */
/*                                                                        */
/*  Boîte de message "A propos de..." contenant les infos de version.     */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop



/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/******                                                              ******/
/******   TApplicationVersion :                                      ******/
/******         Classe pour la gestion des infos de version.         ******/
/******                                                              ******/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

class 	TApplicationVersion 
   {
	protected :
	    LPBYTE      	TransferBlock;
	    void FAR *		FileVersionData;


	public :
		TApplicationVersion ( TModule *  module ) ;

		~TApplicationVersion ( )
		    {
			if  ( FileVersionData )  
				delete []  FileVersionData ;
		     }
			

		BOOL   GetProductName    ( LPSTR &   ProductName ) ;
		BOOL   GetProductVersion ( LPSTR &   ProductVersion ) ;
		BOOL   GetCopyright      ( LPSTR &   Copyright ) ;
		BOOL   GetDebug 	 ( LPSTR &   Debug ) ;


	private :

	// Ne pas permettre la copie de cet objet
		TApplicationVersion 	          ( const TApplicationVersion & ) ;
		TApplicationVersion & operator =  ( const TApplicationVersion & ) ;
     } ;



/**************************************************************************/
/*                                                                        */
/*  Constructeur de la classe.                                            */
/*                                                                        */
/**************************************************************************/

TApplicationVersion :: TApplicationVersion ( TModule *  module )
   {
	char    	ApplicationFileName [ 255 ],
			SubBlockName        [ 255 ] ;
	DWORD   	VersionHandle ;
	UINT    	Size ;
	DWORD		VersionSize ;


	FileVersionData = 0 ;

	module -> GetModuleFileName ( ApplicationFileName, 
			sizeof ( ApplicationFileName ) ) ;
	OemToAnsi ( ApplicationFileName, ApplicationFileName ) ;
	VersionSize = :: GetFileVersionInfoSize (
				ApplicationFileName, & VersionHandle ) ;


	if  ( VersionSize ) 
	   {
		FileVersionData  = ( void FAR * ) new 
					char [ ( UINT ) VersionSize ] ;

		if  ( :: GetFileVersionInfo ( ApplicationFileName, 
				VersionHandle, VersionSize, FileVersionData ) )  
		   {
			strcpy ( SubBlockName, "\\VarFileInfo\\Translation" ) ; 

			if  ( ! :: VerQueryValue ( FileVersionData, 
					SubBlockName, 
					( void FAR * FAR * ) & TransferBlock, 
					& Size ) )  
			   {
				delete FileVersionData ;
				FileVersionData = 0 ;
			    } 
			else
			// Swap the words so wsprintf will print the lang-charset in the correct format.
				* ( DWORD * ) TransferBlock = 
					MAKELONG ( 
						HIWORD ( * ( DWORD * ) TransferBlock ), 
						LOWORD ( * ( DWORD * ) TransferBlock ) ) ;
		    }
	    }
    }



/**************************************************************************/
/*                                                                        */
/*  TApplicationVersion :: GetProductName                                 */
/*      Récupère le nom de l'application.                                 */
/*                                                                        */
/**************************************************************************/

BOOL   TApplicationVersion :: GetProductName ( LPSTR &  ProductName )
   {
	UINT    	Size;
	char    	BlockName [ 255 ] ;


	wsprintf ( BlockName, "\\StringFileInfo\\%08lx\\%s", 
		* ( DWORD * ) TransferBlock, ( LPSTR ) "ProductName" ) ;

	return  ( ( FileVersionData ) ? 
			:: VerQueryValue ( FileVersionData, BlockName, 
				( void FAR *  FAR * ) & ProductName, & Size ) : 
				false ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TApplicationVersion :: GetProductVersion                              */
/*      Récupère la version de l'application.                             */
/*                                                                        */
/**************************************************************************/

BOOL   TApplicationVersion :: GetProductVersion ( LPSTR &  ProductVersion )
   {
	UINT    	Size;
	char    	BlockName [ 255 ] ;


	wsprintf ( BlockName, "\\StringFileInfo\\%08lx\\%s", 
		* ( DWORD * ) TransferBlock, ( LPSTR ) "ProductVersion" ) ;

	return  ( ( FileVersionData ) ? 
			:: VerQueryValue ( FileVersionData, BlockName, 
				( void FAR *  FAR * ) & ProductVersion, & Size ) : 
				false ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TApplicationVersion :: GetCopyright                                   */
/*      Récupère le copyright de l'application.                           */
/*                                                                        */
/**************************************************************************/

BOOL   TApplicationVersion :: GetCopyright ( LPSTR &  Copyright )
   {
	UINT    	Size;
	char    	BlockName [ 255 ] ;


	wsprintf ( BlockName, "\\StringFileInfo\\%08lx\\%s", 
		* ( DWORD * ) TransferBlock, ( LPSTR ) "LegalCopyright" ) ;

	return  ( ( FileVersionData ) ? 
			:: VerQueryValue ( FileVersionData, BlockName, 
				( void FAR *  FAR * ) & Copyright, & Size ) : 
				false ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TApplicationVersion :: GetDebug                                       */
/*      Récupère le "special build".                                      */
/*                                                                        */
/**************************************************************************/

BOOL   TApplicationVersion :: GetDebug ( LPSTR &  Debug )
   {
	UINT    	Size;
	char    	BlockName [ 255 ] ;


	wsprintf ( BlockName, "\\StringFileInfo\\%08lx\\%s", 
		* ( DWORD * ) TransferBlock, ( LPSTR ) "SpecialBuild" ) ;

	return  ( ( FileVersionData ) ? 
			:: VerQueryValue ( FileVersionData, BlockName, 
				( void FAR *  FAR * ) & Debug, & Size ) : 
				false ) ;
    }






/**************************************************************************/
/*                                                                        */
/*   AboutDialog :: SetupWindow                                           */
/*      Initialise la boîte de dialogue "A propos de" avec les infos de   */
/*      version.                                                          */
/*                                                                        */
/**************************************************************************/

void  TAboutDialog :: SetupWindow  ( void )
   {
	LPSTR 		ProductName 	= 0, 
			ProductVersion 	= 0, 
			Copyright 	= 0, 
			Debug 		= 0 ;
			


// Récupération des contrôles correspondant aux VERSIONINFOs
	TStatic *	VersionControl 	 =  new  TStatic ( this, IDC_VERSION  , 255 ) ;
	TStatic *	CopyrightControl =  new  TStatic ( this, IDC_COPYRIGHT, 255 ) ;
	TStatic *	DebugControl 	 =  new  TStatic ( this, IDC_DEBUG    , 255 ) ;
	TStatic *	DateControl	 =  new  TStatic ( this, IDC_DATE     , 255 ) ;
	

// Initialisation de TDialog
	TDialog :: SetupWindow ( ) ;


// Récupération des infos de version
	TApplicationVersion 	ApplicationVersion ( GetModule ( ) ) ;


// Récupération du nom de produit
	if  ( ApplicationVersion. GetProductName ( ProductName ) && 
		ApplicationVersion. GetProductVersion ( ProductVersion ) )  
	   {

		char    	buffer	 [ 255 ] ;
		char	    	VersionName [ 128 ] ;

		* buffer = * VersionName = 0 ;

		VersionControl -> GetText ( VersionName, 
					sizeof ( VersionName ) ) ;
		wsprintf ( buffer, "%s %s %s", 
			ProductName, VersionName, ProductVersion);

		VersionControl -> SetText ( buffer ) ;

		wsprintf ( buffer, "(%s - %s)", __DATE__, __TIME__ ) ;
		DateControl -> SetText ( buffer ) ;
	     }


// Le copyright
	if  ( ApplicationVersion. GetCopyright ( Copyright ) )  
		CopyrightControl -> SetText ( Copyright ) ;

// SpecialBuild, si présent
	if  ( ApplicationVersion. GetDebug ( Debug ) )
		DebugControl -> SetText ( Debug ) ;
    }

