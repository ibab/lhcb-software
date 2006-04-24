// $Id: BcVegPy.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// Include files

// local
#include "LbBcVegPy/BcVegPy.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BcVegPy
//
// 2006-03-15 
//-----------------------------------------------------------------------------

Colflow   BcVegPy::s_colflow   ;
Counter   BcVegPy::s_counter   ;
Grade     BcVegPy::s_grade     ;
Rconst    BcVegPy::s_rconst    ;
Upcom     BcVegPy::s_upcom     ;
Vegcross  BcVegPy::s_vegcross  ;
Coloct    BcVegPy::s_coloct    ;
Confine   BcVegPy::s_confine   ;
Funtrans  BcVegPy::s_funtrans  ;
Genefull  BcVegPy::s_genefull  ;
Intinif   BcVegPy::s_intinif   ;  
Intinip   BcVegPy::s_intinip   ;  
Loggrade  BcVegPy::s_loggrade  ; 
Mixevnt   BcVegPy::s_mixevnt   ;  
Octmatrix BcVegPy::s_octmatrix ;
Outpdf    BcVegPy::s_outpdf    ;   
Qqbar     BcVegPy::s_qqbar     ;    
Subopen   BcVegPy::s_subopen   ;  
Totcross  BcVegPy::s_totcross  ; 
Usertran  BcVegPy::s_usertran  ; 
Vegasinf  BcVegPy::s_vegasinf  ; 
Wavezero  BcVegPy::s_wavezero  ; 

	 
//SetParameter Fortran function for Bc by Z. Yang 2006.03.02
extern "C" {
#ifdef WIN32
  void __stdcall SETPARAMETER( ) ;
#else
  void setparameter_( ) ;
#endif
}

void BcVegPy::SetParameter( ) {
#ifdef WIN32
  SETPARAMETER( );
#else
  setparameter_( );
#endif
}

