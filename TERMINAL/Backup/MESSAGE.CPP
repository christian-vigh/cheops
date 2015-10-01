/**************************************************************************/
/*                                                                        */
/*  Message.cpp  - Christian Vigh, 94/09/27.                              */
/*                                                                        */
/*  Implémentation des classes TMessageBase, TInputMessage,               */
/*  TOutputMessage et TMessageList.                                       */
/*                                                                        */
/*  MODIFICATIONS :                                                       */
/*                                                                        */
/*  02/07/97 (CV)                                                         */
/*      Affichage d'un curseur en forme de sablier sur réception d'un     */
/*      message dont la longueur est supérieure à 128 caractères.         */
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


 // Il faut lire les données de manière synchrone
	SetState ( Processing ) ;
	BeginSend ( ) ;


// Envoi du début du message. On attaque direcetement Connection -> SendData
// plutôt que Message -> SendData, pour que le caractère escape du début du
// message ne soit pas remplacé par une suite de deux carcatères
	AddToSend ( MESSAGE_HEADER, - MESSAGE_HEADER_SIZE ) ;

// Envoi du checksum
	I16toCHAR ( Message. Checksum, buffer ) ;
	AddToSend ( buffer, 2 ) ;

// Envoi du numéro de processus...
	I16toCHAR ( Message. ProcessId, buffer ) ;
	AddToSend ( buffer, 2 ) ;

// ... du numéro de fonction
	I16toCHAR ( Message. Function, buffer ) ;
	AddToSend ( buffer, 2 ) ;

// ... de la taille des données
	I16toCHAR ( Message. DataSize, buffer ) ;
	AddToSend ( buffer, 2 ) ;

// ... des données
	if  ( Message. DataSize )
		AddToSend ( Message. Data, Message. DataSize ) ;

// et enfin, de la séquence de fin du message
	AddToSend ( MESSAGE_TRAILER, - MESSAGE_TRAILER_SIZE ) ;

// On attend un acknowledge
	EndSend ( ) ;
	SetState ( Complete ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   TOutputMessage :: BeginSend, AddToSend, EndSend                      */
/*      Ces trois fonctions permettent l'envoi d'un message en un seul    */
/*      bloc (cependant, les messages excédant STATIC_MESSAGE_SIZE octets */
/*      seront envoyés en plusieurs partie).                              */
/*      BeginSend initialise les données de manière a pouvoir démarrer    */
/*      l'envoi d'un nouveau message.                                     */
/*      AddToSend ajoute au buffer les données spécifiées en paramètre.   */
/*      Le paramètre 'size' indique, en valeur absolue, la taille des     */
/*      données à envoyer. S'il est positif, les caractères de contrôles  */
/*      sont "échappés". S'il est négatif, ils ne le seront pas (utile    */
/*      notamment pour l'envoi de l'en-tête et de la fin du message).     */
/*      Au passage, si AddToSend détecte que son buffer interne est       */
/*      rempli, il envoie son contenu.                                    */
/*      EndSend permet l'envoi effectif du message ainsi collecté par     */
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
/*      Traite le prochain caractère reçu afin de l'ajouter au message.   */
/*      REMARQUE IMPORTANTE : cette fonction ne marche que parceque les   */
/*      TUnixMessage.                                                     */
/*                                                                        */
/**************************************************************************/

# define	IsInData(idx)		( ( offsetof ( TUnixMessage, Data ) )  ==  idx )



void 	TInputMessage :: Add  ( unsigned char  c )
   {
	register BOOL		ReleaseCursor = FALSE ;


// Si c'est le caractère d'échappement, on retient ce fait pour le prochain
// appel
	if  ( c  ==  ESCAPED_CHARACTER  &&  ! LastCharWasEscaped )
	   {
		LastCharWasEscaped = TRUE ;
		return ;
	    }

// Si le précédent caractère était le caractère d'échappement, il faut donner
// à 'c' sa valeur réelle
	if  ( LastCharWasEscaped )
	   {
		LastCharWasEscaped = FALSE ;

		if  ( c ==  TELNET_CHARACTER )
			c = IAC ;
		else if  ( c !=  ESCAPED_CHARACTER )
			c = c - ESCAPED_BASE ;
	     }

// Selon l'état...
	switch  ( State )
	   {

	// Tout premier appel
		case	Zeroed :
			State = InFields ;

	// Un caractère dans les champs autres que Data
		case 	InFields :
			Table [ CharacterCount ++ ] = c ;

			if  ( IsInData ( CharacterCount ) )
			   {
				CHARtoI16 ( Message. Checksum	 , Table ) ;
				CHARtoI16 ( Message. ProcessId   , Table +  2 ) ;
				CHARtoI16 ( Message. Function    , Table +  4 ) ;
				CHARtoI16 ( Message. DataSize    , Table +  6 ) ;

			// Gérer le sablier dans le cas de longs messages
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


	// Un caractère dans le champ Data
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
			break ;		// Y a rien à faire
	    }


	if  ( TheCheopsMenuWindowObject  &&  HourglassCursor  &&  ReleaseCursor )
	   {
		TheCheopsMenuWindowObject -> ReleaseCapture ( ) ;
		HourglassCursor = FALSE ;
		SetCursor ( LoadCursor ( NULL, IDC_ARROW ) ) ;
	     }
	return ;
    }




