/**************************************************************************/
/*                                                                        */
/*   dlgobjs.h    -  Christian VIGH / Mohamed Bentahar, 97/12/26.         */
/*                                                                        */
/*   Objets pour la gestion des controles supplémentaires (CHECKBOX,      */
/*   RADIOBUTTON, etc.)                                                   */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __DLGOBJS_H__ )
#	  define    __DLGOBJS_H__



//
//  Les boutons n'envoient pas de message de notification pour les messages
//  SETFOCUS/KILLFOCUS
# define	BN_SETFOCUS			CBN_SETFOCUS
# define	BN_KILLFOCUS			CBN_KILLFOCUS


//
//  Classe  TDialogCheckbox -
//	Implémentation d'un champ Cheops booléen sous forme de Checkbox.
//

class	TDialogCheckBox 	: public  TCheckBox
   {
	public :
		TDialogEntryData 	FieldData ;
		BOOL			UseStdGetText ;


	public :
		TDialogCheckBox  ( TWindow *		parent,
				   int			resid,
				   TDialogEntryData &	data,
				   DLGITEMTEMPLATE & )
			: TCheckBox ( parent, resid, 0, 0 )
		   {
			FieldData      = data ;
			UseStdGetText  = FALSE ;
		     }


		void	EvSetText	( const char * 	text ) ;
		void	EvGetText	( uint		buflen,
					  char *	text ) ;

		LRESULT DefWindowProc   ( uint, WPARAM, LPARAM ) ;

		void	EvSetFocus	( HWND  lost ) ;
		void	EvKillFocus 	( HWND  gained ) ;

		DECLARE_RESPONSE_TABLE ( TDialogCheckBox ) ;
      } ;



//
//  Classe  TDialogEdit -
//	Gère l'affichage pour les dates, numériques, etc.
//

class	TDialogEdit 	:  public  TEdit
   {
	public :
		TDialogEntryData 	FieldData ;
		BOOL			UseStdGetText ;


	public :
		TDialogEdit      ( TWindow *		parent,
				   int			resid,
				   TDialogEntryData &	data,
				   DLGITEMTEMPLATE &    tmpl ) ;

		void		SetAlignment	( int		alignment ) ;
		virtual void	EvSetText	( const char *  title ) ;
		virtual void	EvGetText	( uint		length,
						  char *	title ) ;

		virtual void	EvSetFocus	( HWND		lost ) ;
		virtual void	EvKillFocus	( HWND		gained ) ;

		virtual void	EvLButtonDown	( uint		modkeys,
						  TPoint &	pt ) ; 
		uint		EvMouseActivate ( HWND	top,
						  uint	hittest,
						  uint	msg ) ;

		LRESULT DefWindowProc   ( uint, WPARAM, LPARAM ) ;

	DECLARE_RESPONSE_TABLE ( TDialogEdit ) ;
    } ;

     


//
//  Classe  TDialogGroupBox -
//	Gère l'affichage pour les dates, numériques, etc.
//

class	TDialogGroupBox 	:  public  TGroupBox
   {
	public :
		TDialogEntryData 	FieldData ;
		int			FirstId, 
					LastId ;
		BOOL			UseStdGetText ;


	public :
		TDialogGroupBox      ( TWindow *		parent,
				       int			resid,
				       TDialogEntryData &	data,
				       DLGITEMTEMPLATE *	first,
				       DLGITEMTEMPLATE *	last ) ;

		int		GetActiveId 	( ) ;
		void		SetActiveId	( int	id ) ;

		char		GetActiveChoice ( ) ;
		void		SetActiveChoice ( char  choice ) ;

		BOOL		IsRadioButton   ( DLGITEMTEMPLATE *	data ) ;

		virtual void	EvSetText	( const char *  title ) ;
		virtual void	EvGetText	( uint		length,
						  char *	title ) ;

		void		EvSetFocus 	( HWND		lost ) ;
		void		EvKillFocus 	( HWND		gained ) ;

		LRESULT 	DefWindowProc   ( uint, WPARAM, LPARAM ) ;
		BOOL		Create		( ) ;
		BOOL		PreProcessMsg	( MSG & ) ;

	DECLARE_RESPONSE_TABLE ( TDialogGroupBox ) ;
    } ;

# endif		/*  __DLGOBJS_H__ */
