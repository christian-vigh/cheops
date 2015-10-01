/**************************************************************************/
/*                                                                        */
/*  DLLSupp.h  -  Christian Vigh, 11/03/97.                               */
/*                                                                        */
/*  Fichier include pour le support de DLL supplémentaires.               */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __DLLSUPP_H__ )
#	  define    __DLLSUPP_H__

// Valeurs limites
# define	MAX_DLLS			256	// Nombre max de DLL externes
# define	MAX_TYPE_NAME_SIZE		32	// Longueur max d'un nom de type
# define	MAX_FUNCTION_NAME_SIZE		64	// Longueur max d'un nom de fonction
# define	MAX_PARAMETER_NAME_SIZE		32	// Longueur max d'un nom de paramètre
# define	MAX_FUNCTION_PARAMETERS		16	// Nombre max de paramètres pour une fonction


// Le type DLTTypeDefinition définit tous les types possibles pour les fonctions
// présentes dans la DLL 
# define	BEGIN_DLTTYPE_TABLE(name) 	DLTType 	name = {
# define	DLTTYPE(name)			{ name, #name, sizeof ( name ) },
# define	END_DLTTYPE_TABLE		{ 0 } }

typedef struct
   {
		WORD		TypeId ;				// Identificateur du type
		char 		TypeName [ MAX_TYPE_NAME_SIZE ] ;	// Nom du type
		WORD		TypeSize ;				// Taille du type
     }  DLTTypeDefinition ;


// Le type DLTParameterDefinition décrit un paramètre de fonction
# define	BEGIN_DLTPARAMETER_TABLE(function)			\
			DLTParameterDefinition	  ParameterTableOf_##function [ ] =  {
# define	DLTPARAMETER(name,typeid,indlevel)         		\
			{ name, typeid, indlevel },
# define	END_DLTPARAMETER_TABLE					\
			{ 0xFFFF, 0, 0 } }

typedef struct
   {
		char		ParameterName [ MAX_PARAMETER_NAME_SIZE ] ;
		WORD		ParameterType ;		// Index dans un tableau DLTTypeDefinition
		BYTE		IndirectionLevel ;      // 0 = paramètre simple
							// 1 = Pointeur
							// 2 = Pointeur sur pointeur, etc.
     }  DLTParameterDefinition ;


//  Le type DLTReturnValueDefinition décrit le type d'une valeur de retour de fonction
typedef struct
    {
		WORD		ReturnValueType ;
		BYTE		IndirectionLevel ;
      }  DLTReturnValueDefinition ;



// La structure TFunctionDefinition définit le type des paramètres et de la
// valeur de retour d'une fonction.
typedef struct
   {
		char			 FunctionName [ MAX_FUNCTION_NAME_SIZE ] ;
		WORD			 FunctionId ;		 // Identificateur unique dans la DLL
		void *			 FunctionAddress ;       // Adresse retournée par GetProcAddress
		DLTParameterDefinition   ParameterDefinitions ;	 // Description des paramètres
		BYTE			 ParameterCount ;	 // Nombre de paramètres
		DLTReturnValueDefinition ReturnValueDefinition ; // Description du type de la valeur de retour
     }  TFunctionDefinition ;



// Une DLL CI doit contenir deux fonctions exportées : CIGetFunctionTableSize, pour
// indiquer le nombre de fonctions utilisables dans la DLL, et CIGetFunctionTable
// pour récupérer la table décrivant chacune des fonctions.

# define	DLC_GET_FUNCTION_TABLE_SIZE		"CIGetFunctionTableSize"
# define	DLC_GET_FUNCTION_TABLE			"CIGetFunctionTable"

typedef	 void    ( * pCIGetFunctionTable     )  ( TFunctionDefinition *  entries ) ;
typedef  int	 ( * pCIGetFunctionTableSize )  ( void ) ;

extern pCIGetFunctionTable	CIGetFunctionTable ;
extern pCIGetFunctionTableSize	CIGetFunctionTableSize ;



// Les fonctions de DLLSUP stockent les informations sur les DLL dans un tableau
// de ce type, où chaque entrée représente une DLL

typedef struct
   {
		char *		DLLName ;
		HANDLE		DLLHandle ;
     }  DLTDLLEntry ;

extern DLTDLLEntry		DllEntries [ MAX_DLLS ] ;
extern int			DllEntryCount ;
		

// Fonctions de support
BOOL	CILoadLibrary		( char *	DLLName ) ;
BOOL	CIFreeLibrary		( char * 	DLLName ) ;

BOOL	CICallFunction		( WORD		FunctionId,
				  void *	FunctionParameters,
				  int		ParametersSize,
				  void **	ReturnValue,
				  int *		ReturnValueSize,
				  BOOL		SwapWords ) ;

