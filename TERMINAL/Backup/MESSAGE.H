/**************************************************************************/
/*                                                                        */
/*  Message.h  -  Christian Vigh, 94/09/27.                               */
/*                                                                        */
/*  Classe pour la gestion des messages.                                  */
/*                                                                        */
/**************************************************************************/


# define	MAX_TIMEOUT_TICKS		5000
# define	STATIC_MESSAGE_SIZE		1024
# define	TIMEOUT_CHAR			( SYNC_CHAR + 1 )


/**************************************************************************/
/*                                                                        */
/*   Classe de base pour les messages entrants/sortants.                  */
/*                                                                        */
/**************************************************************************/

class	TMessageBase
   {
	public :
		TUnixMessage 		Message ;

	// Etat d'un message
		typedef enum
		   {
				Zeroed,    	// Mis à zero
				InFields,	// Réception des champs du message
				InData,		// et des données variables
				Complete,	// Message bien reçu/émis
				Overflowed,	// Trop de caractères en réception
				TimeOut,	// Attente trop longue
				Processing,	// En cours d'émission
				BadChecksum	// Mauvais checksum
		     }  TMessageState ;



	protected :
		TMessageState		State ;


	public :
		TMessageBase	( )
		   { }


		TMessageState		GetState  ( void )
		   {
			return ( State ) ;
		    }


		void		SetState  ( TMessageState  state )
		   {
			State = state ;
		    }

		integer16	ComputeChecksum  ( TUnixMessage &  msg ) ;
    } ;



/**************************************************************************/
/*                                                                        */
/*   Classe  TOutputMessage :                                             */
/*      Classe utilisée pour l'envoi de messages.                         */
/*                                                                        */
/**************************************************************************/


class	TOutputMessage		:  public  TMessageBase
   {
	private :
		char		BufferedMessage [ STATIC_MESSAGE_SIZE ] ;
		int		BufferedMessageSize ;
		TConnection *	Connection ;

		void		AddCharToSend  ( char   c ) ;



	public :
		TOutputMessage		( integer16	 function,
					  char *	 data,
					  integer16	 data_size,
					  integer16	 pid,
					  TConnection *	 conn )
			: TMessageBase ( )
		   {
			Message. ProcessId	=  pid ;
			Message. Function	=  function ;
			Message. Data		=  data ;
			Message. DataSize	=  data_size ;
			Connection		=  conn ;

			Message. Checksum	=  ComputeChecksum ( Message ) ;
			SetState  ( Complete ) ;

                        BufferedMessageSize 	=  0 ;
		    }


		void	BeginSend	( void ) ;
                void	AddToSend 	( char *  buffer, int  size ) ;
		void	Send  		( void ) ;
		void	EndSend		( void ) ;
      } ;




/**************************************************************************/
/*                                                                        */
/*  Classe TInputMessage :                                                */
/*      Gère les messages entrants.                                       */
/*                                                                        */
/**************************************************************************/

class	TInputMessage 	:  public  TMessageBase
   {
	private :
		char				Buffer [ STATIC_MESSAGE_SIZE ] ;
		char				Table  [ sizeof ( TUnixMessage ) ] ;
		BOOL				LastCharWasEscaped ;
		int				CharacterCount ;
		BOOL				HourglassCursor ;



	public :
		TInputMessage		( )
			: TMessageBase ( )
		   {
			Message. Data = NULL ;
			HourglassCursor = FALSE ;
			Zero ( ) ;
		    }

		void		Add	 	( unsigned char  c ) ;
		void            Zero 		( void )
		   {
			SetState ( Zeroed ) ;
			CharacterCount		=  0 ;
			LastCharWasEscaped	=  FALSE ;

			if  ( Message. Data  !=  NULL  &&
					Message. DataSize > STATIC_MESSAGE_SIZE )
				delete []  Message. Data ;
			Message. Data = NULL ;
		    }
     } ;


