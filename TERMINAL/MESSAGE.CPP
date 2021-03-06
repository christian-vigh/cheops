/**************************************************************************/
/*                                                                        */
/*  Message.cpp  - Christian Vigh, 94/09/27.                              */
/*                                                                        */
/*  Impl�mentation des classes TMessageBase, TInputMessage,               */
/*  TOutputMessage et TMessageList.                                       */
/*                                                                        */
/*  MODIFICATIONS :                                                       */
/*                                                                        */
/*  02/07/97 (CV)                                                         */
/*      Affichage d'un curseur en forme de sablier sur r�ception d'un     */
/*      message dont la longueur est sup�rieure � 128 caract�res.         */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop
# include	"Escapes.h"

# include	"UnixApp.h"
# include	"ChopJet.h"
# include	"MnCommon.h"
# include	"ChopCfg.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h"





/**************************************************************************/
/*                                                                        */
/*  TMessageBase :: ComputeChecksum                                       */
/*      Calcule le Checksum d'un message.                                 */
/*                                                                        */
/**************************************************************************/

integer16    TMessageBase :: ComputeChecksum  ( TUnixMessage &  Message )

   {
	register unsigned int		checksum  ;
	register int			i ;
	register unsigned char far *	p ;


	checksum 	=  ( unsigned int ) (
				   SUM16 ( Message. ProcessId )		+
				   SUM16 ( Message. Function )		+
				   SUM16 ( Message. DataSize )
			     )  & 0xFFFF ;


	for  ( i = 0, p = ( unsigned char * ) Message. Data ;
			i < Message. DataSize ; i ++ )
		checksum +=  p [i] ;

	return ( checksum ) ;
     }






/**************************************************************************/
/*                                                                        */
/*   TOutputMessage :: Send                                               */
/*      Envoie un message et attent l'acknowledge.                        */
/*                                                                        */
/**************************************************************************/

void	TOutputMessage :: Send  ( void )
   {
	char			buffer [ 10 ] ;


 // Il faut lire les donn�es de mani�re synchrone
	SetState ( Processing ) ;
	BeginSend ( ) ;


// Envoi du d�but du message. On attaque direcetement Connection -> SendData
// plut�t que Message -> SendData, pour que le caract�re escape du d�but du
// message ne soit pas remplac� par une suite de deux carcat�res
	AddToSend ( MESSAGE_HEADER, - MESSAGE_HEADER_SIZE ) ;

// Envoi du checksum
	I16toCHAR ( Message. Checksum, buffer ) ;
	AddToSend ( buffer, 2 ) ;

// Envoi du num�ro de processus...
	I16toCHAR ( Message. ProcessId, buffer ) ;
	AddToSend ( buffer, 2 ) ;

// ... du num�ro de fonction
	I16toCHAR ( Message. Function, buffer ) ;
	AddToSend ( buffer, 2 ) ;

// ... de la taille des donn�es
	I16toCHAR ( Message. DataSize, buffer ) ;
	AddToSend ( buffer, 2 ) ;

// ... des donn�es
	if  ( Message. DataSize )
		AddToSend ( Message. Data, Message. DataSize ) ;

// et enfin, de la s�quence de fin du message
	AddToSend ( MESSAGE_TRAILER, - MESSAGE_TRAILER_SIZE ) ;

// On attend un acknowledge
	EndSend ( ) ;
	SetState ( Complete ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   TOutputMessage :: BeginSend, AddToSend, EndSend                      */
/*      Ces trois fonctions permettent l'envoi d'un message en un seul    */
/*      bloc (cependant, les messages exc�dant STATIC_MESSAGE_SIZE octets */
/*      seront envoy�s en plusieurs partie).                              */
/*      BeginSend initialise les donn�es de mani�re a pouvoir d�marrer    */
/*      l'envoi d'un nouveau message.                                     */
/*      AddToSend ajoute au buffer les donn�es sp�cifi�es en param�tre.   */
/*      Le param�tre 'size' indique, en valeur absolue, la taille des     */
/*      donn�es � envoyer. S'il est positif, les caract�res de contr�les  */
/*      sont "�chapp�s". S'il est n�gatif, ils ne le seront pas (utile    */
/*      notamment pour l'envoi de l'en-t�te et de la fin du message).     */
/*      Au passage, si AddToSend d�tecte que son buffer interne est       */
/*      rempli, il envoie son contenu.                                    */
/*      EndSend permet l'envoi effectif du message ainsi collect� par     */
/*      AddToSend.                                                        */
/*                                                                        */
/**************************************************************************/

void	TOutputMessage :: BeginSend  ( void )
   {
	BufferedMessageSize = 0 ;
    }


void	TOutputMessage :: EndSend  ( void )
   {
	if  ( BufferedMessageSize )
	   {
		Connection -> SendData ( BufferedMessage, BufferedMessageSize ) ;
		BufferedMessageSize = 0 ;
	    }
    }



void	TOutputMessage :: AddToSend  ( char *  buffer, int  size )
   {
	register BOOL	escape_characters ;
	register int	i ;


	if  ( size  < 0 )
	   {
		escape_characters =  FALSE ;
		size   		  =  - size ;
	    }
	else
		escape_characters =  TRUE ;


	for  ( i = 0 ; i < size ; i ++, buffer ++ )
	   {
		if  ( escape_characters )
		   {
			if  ( isescaped ( * buffer ) )
			   {
				AddCharToSend ( ESCAPED_CHARACTER ) ;
				AddCharToSend ( ESCAPED_BASE + * buffer ) ;
			    }
			else if  ( ( unsigned char ) * buffer  ==  ESCAPED_CHARACTER )
			   {
				AddCharToSend ( ESCAPED_CHARACTER ) ;
				AddCharToSend ( ESCAPED_CHARACTER ) ;
			    }
			else if  ( ( unsigned char ) * buffer 	==  IAC )
				{
				AddCharToSend ( ESCAPED_CHARACTER ) ;
				AddCharToSend ( TELNET_CHARACTER ) ;
				 }
			else
				AddCharToSend ( * buffer ) ;
		     }
		else
			AddCharToSend ( * buffer ) ;
	      }
     }




void	TOutputMessage :: AddCharToSend  ( char  c )
   {
	if  ( BufferedMessageSize  ==  STATIC_MESSAGE_SIZE )
		EndSend ( ) ;
	BufferedMessage [ BufferedMessageSize ++ ] = c ;
    }




/**************************************************************************/
/*                                                                        */
/*  TInputMessage :: Add                                                  */
/*      Traite le prochain caract�re re�u afin de l'ajouter au message.   */
/*      REMARQUE IMPORTANTE : cette fonction ne marche que parceque les   */
/*      TUnixMessage.                                                     */
/*                                                                        */
/**************************************************************************/

# define	IsInData(idx)		( ( offsetof ( TUnixMessage, Data ) )  ==  idx )



void 	TInputMessage :: Add  ( unsigned char  c )
   {
	register BOOL		ReleaseCursor = FALSE ;


// Si c'est le caract�re d'�chappement, on retient ce fait pour le prochain
// appel
	if  ( c  ==  ESCAPED_CHARACTER  &&  ! LastCharWasEscaped )
	   {
		LastCharWasEscaped = TRUE ;
		return ;
	    }

// Si le pr�c�dent caract�re �tait le caract�re d'�chappement, il faut donner
// � 'c' sa valeur r�elle
	if  ( LastCharWasEscaped )
	   {
		LastCharWasEscaped = FALSE ;

		if  ( c ==  TELNET_CHARACTER )
			c = IAC ;
		else if  ( c !=  ESCAPED_CHARACTER )
			c = c - ESCAPED_BASE ;
	     }

// Selon l'�tat...
	switch  ( State )
	   {

	// Tout premier appel
		case	Zeroed :
			State = InFields ;

	// Un caract�re dans les champs autres que Data
		case 	InFields :
			Table [ CharacterCount ++ ] = c ;

			if  ( IsInData ( CharacterCount ) )
			   {
				CHARtoI16 ( Message. Checksum	 , Table ) ;
				CHARtoI16 ( Message. ProcessId   , Table +  2 ) ;
				CHARtoI16 ( Message. Function    , Table +  4 ) ;
				CHARtoI16 ( Message. DataSize    , Table +  6 ) ;

			// G�rer le sablier dans le cas de longs messages
				if  ( TheCheopsMenuWindowObject )
				   {
					if  ( Message. DataSize  >  128 )
					   {
						HourglassCursor = TRUE ;
						SetCursor ( LoadCursor ( NULL, ( LPSTR ) IDC_WAIT ) ) ;
						TheCheopsMenuWindowObject -> SetCapture ( ) ;
					     }
				      }

				if  ( Message. DataSize )
				   {
					if  ( Message. DataSize  <  STATIC_MESSAGE_SIZE )
						Message. Data = Buffer ;
					else if  ( ( unsigned int ) Message. DataSize > MAX_MESSAGE_SIZE  ||
					      ( unsigned int ) Message. DataSize  ==  0  ||
					      ( Message. Data = new char [ (int) Message. DataSize ] )  ==  NULL )
					   {
						CharacterCount = 0 ;
						State = Overflowed ;
						ReleaseCursor = TRUE ;
						break ;
					    }

					State 		  = InData ;
					CharacterCount 	  = 0 ;
					State = InData ;
				    }
				else
				    {
					Message. Data = NULL ;
					State = Complete ;
					ReleaseCursor = TRUE ;
				     }
			     }
			break ;


	// Un caract�re dans le champ Data
		case	InData :
			if  ( CharacterCount + 1  ==   Message. DataSize )
			   {
				State = Complete ;
				Message. Data [ CharacterCount ++ ] = c ;

				if  ( ComputeChecksum ( Message )  !=
						Message. Checksum )
					State = BadChecksum ;
				ReleaseCursor = TRUE ;
			     }
			else if  ( CharacterCount + 1 > Message. DataSize )
			   {
				State = Overflowed ;
				ReleaseCursor = TRUE ;
			    }
			else
				Message. Data [ CharacterCount ++ ] = c ;
			break ;



		case	TimeOut :
		case	BadChecksum :
		case	Overflowed :
			ReleaseCursor = TRUE ;			
			break ;


		case	Complete :
			State = Overflowed ;
			ReleaseCursor = TRUE ;
			break ;		// Y a rien � faire
	    }


	if  ( TheCheopsMenuWindowObject  &&  HourglassCursor  &&  ReleaseCursor )
	   {
		TheCheopsMenuWindowObject -> ReleaseCapture ( ) ;
		HourglassCursor = FALSE ;
		SetCursor ( LoadCursor ( NULL, IDC_ARROW ) ) ;
	     }
	return ;
    }




