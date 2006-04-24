// $Id: BcVegPy.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
#ifndef LBBCVEGPY_BCVEGPY_H 
#define LBBCVEGPY_BCVEGPY_H 1

// Include files

//#include <string>
#include "LbBcVegPy/Colflow.h"
#include "LbBcVegPy/Counter.h"
#include "LbBcVegPy/Grade.h"
#include "LbBcVegPy/Rconst.h"
#include "LbBcVegPy/Upcom.h"
#include "LbBcVegPy/Vegcross.h"
#include "LbBcVegPy/Coloct.h"
#include "LbBcVegPy/Confine.h"
#include "LbBcVegPy/Funtrans.h"
#include "LbBcVegPy/Genefull.h"
#include "LbBcVegPy/Intinif.h"
#include "LbBcVegPy/Intinip.h"
#include "LbBcVegPy/Loggrade.h"
#include "LbBcVegPy/Mixevnt.h"
#include "LbBcVegPy/Octmatrix.h"
#include "LbBcVegPy/Outpdf.h"
#include "LbBcVegPy/Qqbar.h"
#include "LbBcVegPy/Subopen.h"
#include "LbBcVegPy/Totcross.h"
#include "LbBcVegPy/Usertran.h"
#include "LbBcVegPy/Vegasinf.h"
#include "LbBcVegPy/Wavezero.h"

#include "LbPythia/Pythia.h"

class BcVegPy {
public:
  static Colflow   & colflow()   { return s_colflow ;  }
  static Counter   & counter()   { return s_counter ;  }
  static Grade     & grade()     { return s_grade ;    }
  static Rconst    & rconst()    { return s_rconst ;   }
  static Upcom     & upcom()     { return s_upcom ;    }
  static Vegcross  & vegcross()  { return s_vegcross ; }
  static Coloct    & coloct()    { return s_coloct ;   }
  static Confine   & confine()   { return s_confine ;  }
  static Funtrans  & funtrans()  { return s_funtrans ;}
  static Genefull  & geneful()   { return s_genefull;  }
  static Intinif   & intinif()   { return s_intinif ;  }
  static Intinip   & intinip()   { return s_intinip ;  }
  static Loggrade  & loggrade()  { return s_loggrade ; }
  static Mixevnt   & mixevnt()   { return s_mixevnt ;  }
  static Octmatrix & octmatrix() { return s_octmatrix ;}
  static Outpdf    & outpdf()    { return s_outpdf ;   }
  static Qqbar     & qqbar()     { return s_qqbar ;    }
  static Subopen   & subopen()   { return s_subopen ;  }
  static Totcross  & totcross()  { return s_totcross ; }
  static Usertran  & usertran()  { return s_usertran ; }
  static Vegasinf  & vegasinf()  { return s_vegasinf ; }
  static Wavezero  & wavezero()  { return s_wavezero ; }

  static void SetParameter( ) ;

protected:

private:
  	static Colflow    s_colflow ; 
	static Counter    s_counter ; 
	static Grade      s_grade ; 
	static Rconst     s_rconst ; 
	static Upcom      s_upcom ; 
	static Vegcross   s_vegcross ; 
	static Coloct     s_coloct ;   
	static Confine    s_confine ;  
	static Funtrans   s_funtrans ;
	static Genefull   s_genefull;  
	static Intinif    s_intinif ;  
	static Intinip    s_intinip ;  
	static Loggrade   s_loggrade ; 
	static Mixevnt    s_mixevnt ;  
	static Octmatrix  s_octmatrix ;
	static Outpdf     s_outpdf ;   
	static Qqbar      s_qqbar ;    
	static Subopen    s_subopen ;  
	static Totcross   s_totcross ; 
	static Usertran   s_usertran ; 
	static Vegasinf   s_vegasinf ; 
	static Wavezero   s_wavezero ; 
};
#endif // LBBCVEGPY_BCVEGPY_H
