/**************************************************************************/
/*                                                                        */
/*  Print.h  -  Christian Vigh, 95/08/17.                                 */
/*                                                                        */
/*  Gestion des recopies �cran.                                           */
/*                                                                        */
/**************************************************************************/


# if	! defined  ( __PRINT_H__ )
# 	  define     __PRINT_H__





/**************************************************************************/
/*                                                                        */
/*   Classe  THardcopy -                                                  */
/*      G�re les recopies d'�cran sur imprimante.                         */
/*                                                                        */
/**************************************************************************/

class	THardcopy 
   {
	private :
		TPrintDC *			PrintDC ;
		TPoint				PagePosition ;
		int				PageWidth, 
						PageHeight ;
		


	public :
		THardcopy  ( )
		   {
			PrintDC 	= NULL ;
			PagePosition    = TPoint ( 0, 0 ) ;
		    }

	       ~THardcopy ( void )
		   {
			if  ( PrintDC  !=  NULL )
				EndJob ( ) ;
		    }



	// StartJob : d�marre un nouveau job 
		void	StartJob  ( char *  title, TPoint  size, BOOL  reallystart  =  TRUE )
		   {
			if  ( PrintDC  !=  NULL  &&  reallystart )
				return ;
			
		// Cr�er l'objet imprimante
			if  ( reallystart )
			   {
				TPrintDialog :: TData &  PrintData = Printer -> GetSetup ( ) ;


				PrintDC = new  TPrintDC ( PrintData. GetDriverName ( ),
							  PrintData. GetDeviceName ( ),
							  PrintData. GetOutputName ( ),
							  PrintData. GetDevMode ( ) ) ;
				PagePosition    = TPoint ( 0, 0 ) ;
			     }

		// Sauvegarder le DC
			PrintDC -> SaveDC ( ) ;

		// R�cup�rer la r�solution de l'imprimante
			PageWidth  = PrintDC -> GetDeviceCaps ( HORZRES ) ;
			PageHeight = PrintDC -> GetDeviceCaps ( VERTRES ) ;

		//  Syst�me de coordonn�es 
			PrintDC -> SetBkMode ( TRANSPARENT ) ;
			PrintDC -> SetMapMode ( MM_ISOTROPIC ) ;
			PrintDC -> SetWindowExt ( TSize ( size ) ) ;
			PrintDC -> SetViewportExt ( TSize ( PageWidth, PageHeight ) ) ;

		// D�marrer un nouveau document
			if  ( reallystart )
			   {
				PrintDC -> StartDoc ( title, 0 ) ;
				PrintDC -> StartPage ( ) ;
			    }
		     }



	// EndJob : termine un job
		void	EndJob  ( BOOL  reallyend  =  TRUE )
		   {
			if  ( PrintDC  !=  NULL  &&  reallyend )
			   {
				PrintDC -> EndPage ( ) ;
				PrintDC -> EndDoc ( ) ;

				PrintDC -> RestoreDC ( ) ;

				delete  PrintDC ;
				PrintDC = NULL ;
			     }
		     }


	// GetDC ( ) : retourne une r�f�rence sur le print DC
		TPrintDC &  GetDC ( void )
		   {
			return ( * PrintDC ) ;
		    }



	// MustRemain : Arr�te le job et en d�marre un nouveau si la place
	// demand�e est sup�rieure � la place libre sur la page en cours
	// Retourne TRUE si un nouveau job a �t� commenc�
		BOOL	MustRemain ( char *  title, TPoint  size )
		   {
		// Commencer le job si ce n'est d�j� fait
			if  ( PrintDC  ==  NULL )
				StartJob ( title, size ) ;


		// Convertir la taille demand�e en unit�s du p�riph�rique
			TPoint		DeviceSize = size ;
			PrintDC -> LPtoDP ( & DeviceSize ) ;

		// Ignorer les demandes sup�rieures � la taille d'une page
			if  ( DeviceSize. y >= PageHeight ) 
				return ( FALSE ) ;

		// Si la taille demand�e ne tient pas dans la page, on �jecte
		// (en d�marrant un nouveau job)
			if  ( DeviceSize. y + PagePosition. y  >=  PageHeight )
			   {
				EndJob ( ) ;
				StartJob ( title, size ) ;

			// Refaire un LPtoDP avec une bonne origine
				PrintDC -> SetViewportOrg ( PagePosition ) ;
				DeviceSize = size ;
				PrintDC -> LPtoDP ( & DeviceSize ) ;

				PagePosition. y  += DeviceSize. y ;
				return ( TRUE ) ;
			    }
		// sinon on conserve la nouvelle position
			else
			   {
				PrintDC -> SetViewportOrg ( PagePosition ) ;
				PagePosition. y  += DeviceSize. y ;
				return ( FALSE ) ;
			    }
		     }
     } ;



# endif		/*  __PRINT_H__  */







			