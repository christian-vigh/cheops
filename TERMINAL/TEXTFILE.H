/**************************************************************************/
/*                                                                        */
/*  TextFile.h  -  Christian Vigh, 95/09/28.                              */
/*                                                                        */
/*  Gestion (en lecture) des lignes d'un fichier texte.                   */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __TEXTFILE_H__ )
#	  define    __TEXTFILE_H__




# define	MAX_OFFSETS		256	
# define	MAX_CACHED_LINES	511
# define	TEXT_BUFFER_SIZE	4096
# define	TEXT_LINE_SIZE		1024

# define	EOP_FLAG		0x80000000L
# define	EOP_MASK		~EOP_FLAG



// Liste chaînée des offsets des lignes
typedef struct  TOffsetTable
   {
		long int		Offsets [ MAX_OFFSETS ] ;
		struct TOffsetTable *   Next ;
    }  TOffsetTable ;



// Dernières lignes lues
typedef struct  
   {
		long int		LineNumber ;
		char *			Line ;
    }  TCachedLines ;




class	TTextFile
   {
	private :
		BOOL			WasOpenedHere ;


	protected :
		TOffsetTable *		OffsetTable,
			     *		OffsetTableEnd ;
		TCachedLines *		Lines ;
		char *			InputBuffer ;
		long int		OffsetCount,
					FilePosition ;


		char			TextFileName [ 256 ] ;
		FILE *			TextFileFP ;


		typedef enum
		   {
				Regular		=  0,
				EOL		=  1,
				Marker		=  2,
				EndOfPage       =  3,
				Discard		=  0x8000
		     }  TCharacterType ;


	public :
		TTextFile  ( ) ;
	       ~TTextFile  ( ) ;


		virtual BOOL	Open 		( char *	Filename ) ;
		virtual BOOL	Open		( FILE *	fp ) ;
		virtual void	Close		( void ) ;

		virtual BOOL	GetLine		( long int  	line_number,
						  char *	buffer ) ;
		void		Update  	( void ) ;

		long int	GetPageFromLine ( long int	line ) ;
		long int	GetLineFromPage ( long int	page ) ;


	// Retourne la taille du fichier en cours de réception
		long int	FileSize  ( void )
		   {
			return ( filelength ( fileno ( TextFileFP ) ) ) ;
		    }


	// Retourne le nombre de lignes
		long int	GetLineCount  ( void )
		   {
			return ( OffsetCount ) ;
		    }



	protected :
		virtual TCharacterType	CheckCharacter  ( int  c ) ;

		virtual BOOL	Read 		( long int	offset,
						  char *	buffer,
						  long int &	new_offset ) ;

		virtual int	GetCharacter	( FILE *	fp )
		   {
			return  ( fgetc ( fp ) ) ;
		    }


		void	Add		( long int	offset ) ;

		void	Reset		( void ) ;
		void	Free		( void ) ;
      } ;



# endif		/*  __TEXTFILE_H__  */