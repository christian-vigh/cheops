/**************************************************************************/
/*                                                                        */
/*   UnixApp.cpp  -  Christian Vigh, 94/10/03.                            */
/*                                                                        */
/*   Implémentation de la classe TUnixApplication.                        */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop


# include	"UnixApp.h"
# include	"Event.h"
# include	"Icon.h"
# include	"UnixDlg.h"
# include	"ChopJet.h"
# include	"Cheops.h"
# include	"Unix.h"
# include	"TextFile.h"
# include       "editfile.h"
# include	"MNCommon.h"
# include	"ChopCfg.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h"


/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: TUnixApplication                                 */
/*      Constructeur de la classe.                                        */
/*                                                                        */
/**************************************************************************/

TUnixApplication :: TUnixApplication	( char *		name,
					  integer16		appid,
					  integer16		pid,
					  TConnection *		connection )
   {
	ApplicationName [ MAX_APPLICATION_NAME_LENGTH - 1 ] = 0 ;
	strncpy ( ApplicationName, name, MAX_APPLICATION_NAME_LENGTH - 1 ) ;

	ApplicationId 		=  appid ;
	ProcessId		=  pid ;
	Connection 		=  connection ;

	for  ( int  i = 0 ; i < MAX_APPLICATION_OBJECTS ; i ++ )
		ApplicationObjects [i]  =  NULL ;
    }





/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: ~TUnixApplication                                */
/*      Détruit la file de messages et les objets associés à cette        */
/*      application.                                                      */
/*                                                                        */
/**************************************************************************/

TUnixApplication :: ~TUnixApplication  ( )
   {
	for  ( int  i = 0 ; i < MAX_APPLICATION_OBJECTS ; i ++ )
	   {
		if  ( ApplicationObjects [i]  !=  NULL )
			DeleteObject ( i + 1 ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: AddObject                                        */
/*      Ajoute un objet à l'application, et retourne son identificateur.  */
/*                                                                        */
/**************************************************************************/

int	TUnixApplication :: AddObject  ( TWindow *  object )
   {
	for  ( int  i = 0 ; i < MAX_APPLICATION_OBJECTS ; i ++ )
	   {
		if  ( ApplicationObjects [i]  ==  NULL )
		   {
			ApplicationObjects [i] = object ;
			return ( i + 1 ) ;
		    }
	    }

	return ( -1 ) ;
     }


/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: SwapObject                                       */
/*      Interchange un objet avec un autre			  	  */
/*                                                                        */
/**************************************************************************/

void	TUnixApplication :: SwapObject  ( int source_id, int dest_id )
   {
	TWindow *	tempo ;

	source_id --;
	dest_id -- ;

	tempo = ApplicationObjects [ dest_id ] ;
	ApplicationObjects [ dest_id ] = ApplicationObjects [ source_id ] ;
	ApplicationObjects [ source_id ] = tempo ;
     }



/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: GetObject                                        */
/*      Retourne un pointeur sur l'objet spécifié par 'object_id'.        */
/*                                                                        */
/**************************************************************************/

TWindow *	TUnixApplication :: GetObject  ( int  object_id )
   {
	object_id -- ;

	if  ( object_id >= 0  &&  object_id < MAX_APPLICATION_OBJECTS )
		return ( ApplicationObjects [ object_id ] ) ;
	else
		return ( NULL ) ;
     }



/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: GetObjectIndex                                   */
/*      Retourne l'index de l'objet spécifié par hwnd, ou -1 s'il         */
/*      n'appartient pas à l'application.                                 */
/*                                                                        */
/**************************************************************************/

int  TUnixApplication :: GetObjectIndex  ( HWND  hwnd )
   {
	for  ( int  i = 0 ; i < MAX_APPLICATION_OBJECTS ; i ++ )
	   {
		if  ( ApplicationObjects [i]  &&
				ApplicationObjects [i] -> HWindow  ==  hwnd )
			return ( i + 1 ) ;
	     }

	return ( -1 ) ;
     }



/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: DeleteObject                                     */
/*      Supprime l'object spécifié.                                       */
/*                                                                        */
/**************************************************************************/

void	TUnixApplication :: DeleteObject  ( int  object_id, BOOL erase )
   {
	HWND		Candidate ;


	object_id -- ;

	if  ( object_id >= 0  &&  object_id < MAX_APPLICATION_OBJECTS  &&
		ApplicationObjects [ object_id ]  !=  TerminalWindow  &&
		ApplicationObjects [ object_id ]  !=  NULL )
	   {
		if  ( erase ) 
		   {
			Candidate = ApplicationObjects [ object_id ] -> HWindow ;


			if  ( Candidate )
			   {
				:: SendMessage ( Candidate, WM_ENDAPPLICATION, 0, 0L ) ;
				:: PostMessage ( Candidate, WM_CLOSE, 0, 0L ) ;
			    }
		     }

		ApplicationObjects [ object_id ] = NULL ;
	    }
     }



void	TUnixApplication :: DeleteObject ( TWindow *  object )
   {
	for  ( register int  i = 0 ; i < MAX_APPLICATION_OBJECTS ; i ++ )
	   {
		if  ( ApplicationObjects [i]  ==  object )
		   {
			ApplicationObjects [i] = 0 ;
			return ;
		    }
	    }
     }


