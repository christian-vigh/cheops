/**************************************************************************/
/*                                                                        */
/*  ChopMail.h  -  Christian Vigh, 95/11/15.                              */
/*                                                                        */
/*  Classe pour la gestion du courrier Unix.                              */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __CHOPMAIL_H__ )
#	  define    __CHOPMAIL_H__


# define	IDC_MAIL_BASE		1200 



/**************************************************************************/
/*                                                                        */
/*  TCheopsMailDialog -                                                   */
/*      Classe de base pour la lecture et l'envoi de courrier.            */
/*                                                                        */
/**************************************************************************/

# define	IDC_MAIL_EDIT		( IDC_MAIL_BASE + 0 )
# define	IDC_MAIL_CLOSE		( IDC_MAIL_BASE + 1 )




class	TCheopsMailDialog 	:  public  TDecoratedFrame 
   {
	protected :
		TEdit *			EditClient ;
		TTextButtonGadget *	SendButton ;
		TColorControlBar *	ControlBar ;
		TStatusBar *		StatusBar ;
		TCheopsMenuWindow *	CheopsMenuWindow ;



	public :
		TCheopsMailDialog  ( TCheopsMenuWindow *	window,
				     TMDIClient * 		client,
				     char *			title ) ;
	       ~TCheopsMailDialog  ( ) ;


	// Active (donne le focus) la fenêtre
		virtual void 	Activate 		( void ) ;

	// Fonctions nécessaires à cause du double héritage de cette classe
		BOOL		PreProcessMsg   	( MSG & ) ;
		BOOL		Create 			( void ) ;

	// Fermeture de la fenêtre
		virtual void  	CmOk			( void ) ;



	// Les classes dérivées peuvent redéfinir cette fonction pour
	// insérer leurs gadgets dans la barre d'icônes
		virtual void    InsertControlBarItems	( TControlBar &   sb )
		   { }


	DECLARE_RESPONSE_TABLE ( TCheopsMailDialog ) ;
     } ;





/**************************************************************************/
/*                                                                        */
/*  Classe  TCheopsReadMailDialog -                                       */
/*      Consultation du courrier Unix.                                    */
/*                                                                        */
/**************************************************************************/

# define	IDC_RMAIL_MESSAGE_LIST		( IDC_MAIL_BASE + 100 )
# define	IDC_RMAIL_COPY			( IDC_MAIL_BASE + 101 )
# define	IDC_RMAIL_DELETE		( IDC_MAIL_BASE + 102 )



class	TCheopsReadMailDialog 	:  public  TCheopsMailDialog
   {
	private :
		struct  TMailEntry
		   {
			long int		BeginOffset,
						EndOffset ;
			char *			From ;
			char *			Subject ;
			struct TMailEntry *	Next ;
		     } ;



		char *		MailFile ;
		TComboBox *	MailComboBox ;
		TMailEntry *	Head,
			   *	Tail ;


	public :
		TCheopsReadMailDialog  ( TCheopsMenuWindow *	chopmenu,
					 TMDIClient *		client,
					 char *			title,
					 char *			mail_file ) ;

	       ~TCheopsReadMailDialog  ( )
		   {
			unlink ( MailFile ) ;
			free ( MailFile ) ;
			CheopsMenuWindow -> ReadMailWindow = 0 ;
			FreeMailEntries ( ) ;
		     }


		virtual void 	Activate		( void ) ;
		virtual void	InsertControlBarItems   ( TControlBar &  sb ) ;
		
		void		CmCopy			( void ) ;
		void		CmDelete		( void ) ;
		void		CmComboBox		( uint ) ;


		virtual BOOL	Create	( void )
		   {
			register BOOL	Status = TCheopsMailDialog :: Create ( ) ;


			ReadMailEntries ( ) ;
			MailComboBox -> SetFocus ( ) ;
			return  ( Status ) ;
		     }


		virtual BOOL	PreProcessMsg		( MSG & ) ;
			

	// Crée une icône MDI
		void	EvSysCommand  ( uint  type, TPoint &  pos )
		   {
			if  ( type  ==  SC_MINIMIZE )
				CheopsMenuWindow -> IconifyMe ( this, 
					TResId ( "RMAIL_ICON" ) ) ;
			else
				TDecoratedFrame :: EvSysCommand ( type, pos ) ;
		     }




	private :
		void		FreeMailEntries 	( void ) ;
		void		ReadMailEntries		( void ) ;


	DECLARE_RESPONSE_TABLE ( TCheopsReadMailDialog ) ;
    } ;






/**************************************************************************/
/*                                                                        */
/*  Classe  TCheopsSendMailDialog -                                       */
/*      Envoi de courrier à Unix.                                         */
/*                                                                        */
/**************************************************************************/

# define	IDC_SMAIL_USER_LIST		( IDC_MAIL_BASE + 200 )
# define	IDC_SMAIL_SEND			( IDC_MAIL_BASE + 201 )


class	TCheopsSendMailDialog 	:  public  TCheopsMailDialog
   {
	private :
		TEdit *		UserListEdit ;


	public :
		TCheopsSendMailDialog  ( TCheopsMenuWindow *	chopmenu,
					 TMDIClient *		client,
					 char *			title ) ;


	       ~TCheopsSendMailDialog  ( )
		   {
			CheopsMenuWindow -> SendMailWindow = 0 ;
		    }	


		virtual BOOL	Create	( void )
		   {
			register BOOL	Status = TCheopsMailDialog :: Create ( ) ;


			UserListEdit -> SetFocus ( ) ;
			return  ( Status ) ;
		     }



	// Crée une icône MDI
		void	EvSysCommand  ( uint  type, TPoint &  pos )
		   {
			if  ( type  ==  SC_MINIMIZE )
				CheopsMenuWindow -> IconifyMe ( this, 
					TResId ( "SMAIL_ICON" ) ) ;
			else
				TDecoratedFrame :: EvSysCommand ( type, pos ) ;
		     }



		virtual void	InsertControlBarItems   ( TControlBar &  sb ) ;
		
		void		CmSend			( void ) ;
		void		CmChanged		( void ) ;


	DECLARE_RESPONSE_TABLE ( TCheopsSendMailDialog ) ;
    } ;




# endif		/*  __CHOPMAIL_H__  */

				     
