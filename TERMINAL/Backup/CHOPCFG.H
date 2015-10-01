/**************************************************************************/
/*                                                                        */
/*  ChopCfg.h  -  Christian Vigh, 95/10/13.                               */
/*                                                                        */
/*  Gestion de la configuration des fenêtres Cheops.                      */
/*                                                                        */
/**************************************************************************/



# if	! defined ( __CHOPCFG_H__ )
#	  define    __CHOPCFG_H__



/**************************************************************************/
/*                                                                        */
/*  TCheopsConfiguration -                                                */
/*      Accès aux paramètres des fenêtres Cheops.                         */
/*                                                                        */
/**************************************************************************/

typedef enum
   {
		False	=  0,
		True	=  1
     }  boolean ;




class	TCheopsConfiguration 
   {
	private :
		char *		FileName ;



	public :
		int		ApplicationId ;
		int		ObjectId ;
		int		ProcessId ;

	
	public :
		void	SetFileName	( char *	filename  =  0 ) ;

	// Fonctions génériques d'accès aux paramètres
		void	Get      	( const char *	section,
					  const char *	item,
					  char *	output,
					  char *	defval   =  "" ) ;

		void	Get       	( const char *	section,
					  const char *	item,
					  boolean &	output,
					  boolean	defval 	 =  False ) ;

		void	Get       	( const char *	section,
					  const char *	item,
					  int &		output,
					  int		defval	 =  0 ) ;

		void	Get    		( const char *	section, 
					  const char *	item,
					  long int &  	output,
					  long int	defval 	 =  0L ) ;



	// Fonctions génériques d'écriture de paramètres
		void	Set      	( const char *	section,
					  const char *	item,
					  char *	input ) ;

		void	Set       	( const char *	section,
					  const char *	item,
					  boolean	input ) ;

		void	Set       	( const char *	section,
					  const char *	item,
					  int 		input ) ;

		void	Set    		( const char *	section, 
					  const char *	item,
					  long int   	input ) ;


	public :
		TCheopsConfiguration ( char *  filename  =  0 )
		   {
			FileName = 0 ;
			SetFileName ( filename ) ;
		    }


	       ~TCheopsConfiguration ( )
		   {
			if  ( FileName )	
				free ( FileName ) ;
		    }


	// Fonctions d'accès à des données spécifiques à une classe
	// (le nom de la classe fournit le nom de la section)
		void	GetClassItem    ( TWindow *	win,
					  const char *	item,
					  char *	output,
					  char *	defval  =  "" )
		    {
			Get ( typeid ( * win ). name ( ), item, output, defval ) ;
		     }


		void	GetClassItem    ( TWindow *	win,
					  const char *	item,
					  boolean &	output,
					  boolean 	defval  =  False )
		    {
			Get ( typeid ( * win ). name ( ), item, output, defval ) ;
		     }



		void	GetClassItem    ( TWindow *	win,
					  const char *	item,
					  int & 	output,
					  int		defval  =  0 )
		    {
			Get ( typeid ( * win ). name ( ), item, output, defval ) ;
		     }



		void	GetClassItem    ( TWindow *	win,
					  const char *	item,
					  long int &	output,
					  long int 	defval  =  0L )
		    {
			Get ( typeid ( * win ). name ( ), item, output, defval ) ;
		     }



	// Fonctions d'écriture des données spécifiques à une classe
	// (le nom de la classe fournit le nom de la section)
		void	SetClassItem    ( TWindow *	win,
					  const char *	item,
					  char *	output ) 
		    {
			Set ( typeid ( * win ). name ( ), item, output ) ;
		     }


		void	SetClassItem    ( TWindow *	win,
					  const char *	item,
					  boolean &	output )
		    {
			Set ( typeid ( * win ). name ( ), item, output ) ;
		     }



		void	SetClassItem    ( TWindow *	win,
					  const char *	item,
					  int & 	output )
		    {
			Set ( typeid ( * win ). name ( ), item, output ) ;
		     }



		void	SetClassItem    ( TWindow *	win,
					  const char *	item,
					  long int &	output )
		    {
			Set ( typeid ( * win ). name ( ), item, output ) ;
		     }


    } ;


# endif		/*  __CHOPCFG_H__  */
