/**************************************************************************/
/*                                                                        */
/*  Event.cpp  -  Christian Vigh, 94/10/26.                               */
/*                                                                        */
/*  Implémentation des classes TEvent et TEventQueue.                     */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include	"UnixApp.h"
# include	"Event.h"




/**************************************************************************/
/*                                                                        */
/*  TEvent :: Send                                                        */
/*      Envoie le message à Unix.                                         */
/*                                                                        */
/**************************************************************************/

void	TEvent :: Send  ( void )
   {
	WTNextEvent *		Event ;
	register int		size ;


// Calcul de la taille du message
	size = sizeof ( WTNextEvent ) - 1 ;

	if  ( ( int ) LParamSize  ==  UWC_LPARAM_NOT_POINTER )
		size += sizeof ( INTEGER32 ) ;
	else
		size += LParamSize ;

// Allocation du machin
	Event = ( WTNextEvent * ) new  char [ size ] ;

// Recopie des données du message
	I16toCHAR ( UWS_OK       , Event -> Status ) ;
	I16toCHAR ( ApplicationId, Event -> ApplicationId ) ;
	I16toCHAR ( ObjectId     , Event -> ObjectId ) ;

	I16toCHAR ( MessageId , Event -> Message. Message ) ;
	I16toCHAR ( WParam    , Event -> Message. WParam ) ;
	I16toCHAR ( LParamSize, Event -> Message. LParamSize ) ;

	if  ( ( int ) LParamSize  ==  UWC_LPARAM_NOT_POINTER )
		I32toCHAR ( LParam, Event -> Message. LParam ) ;
	else
		memcpy ( Event -> Message. LParam, ( void * ) LParam,
				LParamSize ) ;

// Construction du message 
/*	TOutputMessage		Reply  ( FunctionId,
					 ( char * ) Event,
					 size,
					 ProcessId,
					 ApplicationTable -> GetApplicationOrigin ( ApplicationId )  ) ;

	Reply. Send ( ) ;*/


	TUnixMessage 		Msg ;
	TUnixApplication *	App = ApplicationTable -> Get ( ApplicationId ) ;


	Msg. Function  =  FunctionId ;
	Msg. ProcessId =  ProcessId ;

	ApplicationTable -> Answer ( App, & Msg, ( char * ) Event, size ) ;

// C'est fini, on fait le ménage
	delete []  ( char * )  Event ;
    }





/**************************************************************************/
/*                                                                        */
/*  TEventQueue :: Kill                                                   */
/*      Supprime les messages associés au process id spécifié, ou tous    */
/*      les messasges si le process id est à -1.                          */
/*                                                                        */
/**************************************************************************/

void TEventQueue :: Kill ( int  process_id )
   {
	register TEventQueueEntry *	p  =  EventQueue,
				  *	q ;

	if  ( process_id  ==  -1 )
	   {
		while  ( p  !=  NULL )
		   {
			q = p -> Next ;
			delete  p -> Event ;
			delete  p ;
			p = q ;
		    }

		EventQueue = NULL ;
	     }
	else
	    {
		while  ( p  !=  NULL )
		   {
			q = p -> Next ;

			if  ( p -> Event -> ProcessId  ==  process_id )
			   {
				if  ( p -> Previous  !=  NULL )
					p -> Previous -> Next = p -> Next ;

				if  ( p -> Next  !=  NULL )
					p -> Next -> Previous = p -> Previous ;

				if  ( p  ==  EventQueue )
					EventQueue = p -> Next ;
				delete  p -> Event ;
				delete  p ;
			     }

			p = q ;
		     }
              }
     }




/**************************************************************************/
/*                                                                        */
/*  TEventQueue :: KillFocusMessages                                      */
/*      Supprime les messages SET/KILLFOCUS associés au process id        */
/*      spécifié ou à tous les process id si prcess_id est à -1.          */
/*                                                                        */
/**************************************************************************/

void TEventQueue :: KillFocusMessages ( int  process_id )
   {
	register TEventQueueEntry *	p  =  EventQueue,
				  *	q ;

	while  ( p  !=  NULL )
	   {
		q = p -> Next ;

		if  ( ( p -> Event -> ProcessId  ==  process_id  ||
			process_id  ==  -1 )  &&
				IsFocusMessage ( p -> Event -> MessageId,
					p -> Event -> WParam,
					( LPARAM ) p -> Event -> LParam ) )
		   {
			if  ( p -> Previous  !=  NULL )
				p -> Previous -> Next = p -> Next ;

			if  ( p -> Next  !=  NULL )
				p -> Next -> Previous = p -> Previous ;

			if  ( p  ==  EventQueue )
				EventQueue = p -> Next ;
			delete  p -> Event ;
			delete  p ;
		     }

		p = q ;
	      }
     }




/**************************************************************************/
/*                                                                        */
/*  TEventQueue :: Request :                                              */
/*      Envoie le prochain message du type demandé.                       */
/*                                                                        */
/**************************************************************************/

void	TEventQueue :: Request  ( int  appid, int  ProcessId )
   {
	register TEvent *		p ;
	register TEventQueueEntry *	eq	=  EventQueue ;
	register BOOL			found	=  FALSE ;


	while  ( eq  !=  NULL )
	   {
		p = eq -> Event ;

		if  ( p -> ProcessId  ==  ProcessId )  
		   {
			p -> Send ( ) ;

			if  ( eq -> Previous  !=  NULL )
				eq -> Previous -> Next = eq -> Next ;

			if  ( eq -> Next  !=  NULL )
				eq -> Next -> Previous = eq -> Previous ;

			if  ( eq  ==  EventQueue )
				EventQueue = eq -> Next ;

			delete  p ;
			delete  eq ;
			found = TRUE ;
			break ;
		      }

		eq = eq -> Next ;
	    }


	if  ( ! found )
		EventRequested [ appid ] = TRUE ;
     }
		   



/**************************************************************************/
/*                                                                        */
/*   TEventQueue :: Enqueue                                               */
/*      Envoie directement le message si une demande a été effectuée,     */
/*      ou bien le place dans la file.                                    */
/*                                                                        */
/**************************************************************************/

void	TEventQueue :: Enqueue  ( int		application_id,
				  int		object_id,
				  int		process_id,
				  int		message_id,
				  WPARAM	wparam,
				  LPARAM	lparam,
				  int		lparam_size,
				  int		function_id )
    {
	register TEvent *		p ;
	register TEventQueueEntry *  	eq	=  EventQueue ;

	
	if  ( EventRequested [ application_id ] ) // Il faut l'envoyer directement
	   {
		TEvent		EV ( application_id, object_id,
					process_id, message_id, wparam, lparam,
					    lparam_size, function_id ) ;

		EventRequested [ application_id ] = FALSE ;
		EV. Send ( ) ;
	     }
	else				// Sinon il faut le stocker
	   {
		p = new TEvent ( application_id, object_id,
				   process_id, message_id, wparam, lparam,
				      lparam_size, function_id ) ;

		if  ( eq  ==  NULL )
		   {
			eq = EventQueue = new TEventQueueEntry ;
			eq -> Next = eq -> Previous = NULL ;
		    }
		else
		   {
			while  ( eq -> Next  !=  NULL )
				eq = eq -> Next ;

			eq -> Next = new TEventQueueEntry ;
			eq -> Next -> Previous = eq ;
			eq -> Next -> Next = NULL ;
			eq = eq -> Next ;
		     }

		eq -> Event = p ;
	      }
      }


/**************************************************************************/
/*                                                                        */
/*   TEventQueue :: IsSetFocus						  */
/*	Contrôle l'existance d'un SetFocus dans la file d'événement.	  */
/*   TEventQueue :: SetFocus                                              */
/*      Met a jour la derniere demande de positionnement.                 */
/*                                                                        */
/**************************************************************************/
BOOL	TEventQueue :: IsSetFocus  ( )
   {
	register TEventQueueEntry *  	eq     =  EventQueue ;

	while  ( eq )
	   {
		if  ( IsFocusMessage ( eq -> Event -> MessageId,
				eq -> Event -> WParam, ( LPARAM ) eq -> Event -> LParam ) )
			return  ( TRUE ) ;

		eq = eq -> Next ;
	    }

	return  ( FALSE ) ;
    }

BOOL	TEventQueue :: SetFocus  ( int		process_id,
				    WPARAM	wparam,
				    LPARAM	lparam )
   {
	register TEventQueueEntry *  	eq	=  EventQueue ;
	char				result = FALSE ;

	if  ( eq  ==  NULL )
		return ( result ) ;

	while  ( eq -> Next )
		eq = eq -> Next ;

	while  ( ! result && eq && eq -> Event -> ProcessId  ==  process_id )
	   {
		if  ( IsFocusMessage ( eq -> Event -> MessageId,
				eq -> Event -> WParam, ( LPARAM ) eq -> Event -> LParam ) )
		   {
			eq -> Event -> WParam = wparam ;
			eq -> Event -> Duplicate ( lparam, UWC_LPARAM_NOT_POINTER ) ;
			result = TRUE ;
		    }
		else
			eq = eq -> Previous ;

	    }

	return ( result ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TEventQueue :: Flush -                                                */
/*      Envoie de manière inconditionnelle tous les évènements restant    */
/*      en attente.                                                       */
/*      Cette fonction est utilisée par le menu afin que les paramètres   */
/*      (menus flottants) et l'évènement "quitter le menu".               */
/*                                                                        */
/**************************************************************************/

void	TEventQueue :: Flush  ( void )
   {
	while  ( EventQueue )
	   {
		EventQueue -> Event -> Send ( ) ;
		EventQueue = EventQueue -> Next ;
	    }
     }

