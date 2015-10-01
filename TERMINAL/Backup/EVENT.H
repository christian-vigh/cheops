/**************************************************************************/
/*                                                                        */
/*  Event.h  -  Christian Vigh, 94/10/26.                                 */
/*                                                                        */
/*  Classe pour la gestion des demandes d'évènement en provenance d'Unix. */
/*                                                                        */
/**************************************************************************/



/**************************************************************************/
/*                                                                        */
/*   Classe  TEvent :                                                     */
/*      Gère un évènement destiné à Unix, en calquant les informations    */
/*      fournies par Windows à une WindowProc.                            */
/*                                                                        */
/**************************************************************************/

class	TEvent
   {
	public :
		int		ApplicationId,
				ObjectId ;
		int		ProcessId ;
		int		MessageId ;
		WPARAM		WParam ;
		char far *	LParam ;
		int		LParamSize ;
		int		FunctionId ;


	public :

	// Constructeur : mémorise l'id (Windows) de l'application, de
	// l'objet, du numéro de processus, du paramètre wparam,
	// et effectue une copie des données si lparam est un pointeur.
		TEvent	( int		application_id,
			  int		object_id,
			  int		process_id,
			  int		message_id,
			  WPARAM	wparam,
			  LPARAM	lparam,
			  int		lparam_size,
			  int		function_id	=  UWF_NEXT_EVENT )
		   {
			ApplicationId 	= application_id ;
			ObjectId	= object_id ;
			ProcessId	= process_id ;
			MessageId	= message_id ;
			WParam		= wparam ;
			FunctionId	= function_id ;
			Duplicate ( lparam, ( int ) lparam_size ) ;
		    }




	// Destructeur : libère la mémoire éventuellement allouée pour la
	// copie des données pointées par lparam.
	       ~TEvent  ( )
		  {
			if  ( LParamSize  !=  UWC_LPARAM_NOT_POINTER )
				delete [] ( char far * ) LParam ;
		   }



	// Duplicate :
	//	Duplique les donnnées pointées par lparam si ce dernier est
	//	un pointeur.
		void	Duplicate ( LPARAM  lparam, int  lparam_size )
		   {
			LParamSize = lparam_size ;

			if  ( LParamSize  ==  UWC_LPARAM_NOT_POINTER )
				LParam = ( char far * ) lparam ;
			else
			   {
				LParam = ( char far * ) new  char [ lparam_size ] ;
				memcpy ( LParam, ( void * ) lparam, lparam_size ) ;
			    }
		     }
								

	// Send :
	//	Envoie le message de réponse UWF_NEXT_EVENT
		void	Send  ( void ) ;
     } ;





/**************************************************************************/
/*                                                                        */
/*  TEventQueue :                                                         */
/*      Gère une file de messages en attente.                             */
/*                                                                        */
/**************************************************************************/

class	TEventQueue
   {
	private :
		typedef struct   TEventQueueEntry
		   {
			TEvent *			Event ;
			struct TEventQueueEntry *       Next,
						* 	Previous ;
		    }  TEventQueueEntry ;


		TEventQueueEntry *	EventQueue ;
		BOOL			EventRequested [ MAX_UNIX_APPLICATIONS ] ;
		integer16		RequestingPid ;


	public :
		TEventQueue ( )
		   {
			memset ( EventRequested, 0, sizeof ( EventRequested ) ) ;
			EventQueue	=  NULL ;
		     }


	       ~TEventQueue ( )
		  {
			Kill ( -1 ) ;
		   }


		void	Kill     	   ( int		process_id ) ;
		void 	KillFocusMessages  ( int		process_id ) ;


		void	Enqueue  ( int		application_id,
				   int		object_id,
				   int		process_id,
				   int		message_id,
				   WPARAM	wparam	     =  0,
				   LPARAM	lparam	     =  0,
				   int		lparam_size  =  UWC_LPARAM_NOT_POINTER,
				   int		function_id  =  UWF_NEXT_EVENT ) ;



		BOOL	IsSetFocus( ) ;
		BOOL	SetFocus  ( int		process_id,
				    WPARAM	wparam,
				    LPARAM	lparam ) ;

		void	Request  ( int		appid,
				   int 		pid ) ;

		void	Flush	 ( void ) ;

		BOOL	vide  ( )
		   {
                   	return ( EventQueue  ==  NULL ) ;
		    }
      } ;


