#ifndef LBGENXICC_GENXICC_H 
#define LBGENXICC_GENXICC_H 1

// Include files

//#include <string>
#include "LbGenXicc/Counter.h"
#include "LbGenXicc/Upcom.h"
#include "LbGenXicc/Vegcross.h"
#include "LbGenXicc/Confine.h"
#include "LbGenXicc/Funtrans.h"
#include "LbGenXicc/Loggrade.h"
#include "LbGenXicc/Mixevnt.h"
#include "LbGenXicc/Outpdf.h"
#include "LbGenXicc/Subopen.h"
#include "LbGenXicc/Usertran.h"
#include "LbGenXicc/Vegasinf.h"
#include "LbGenXicc/Vegasbin.h"
#include "LbGenXicc/Valmatrix.h"
#include "LbGenXicc/Mtypeofxi.h"
#include "LbGenXicc/Wbstate.h"


#include "LbPythia/Pythia.h"

class GenXicc {
public:
  static Counter   & counter()   { return s_counter ;  }
  static Upcom     & upcom()     { return s_upcom ;    }
  static Vegcross  & vegcross()  { return s_vegcross ; }
  static Confine   & confine()   { return s_confine ;  }
  static Funtrans  & funtrans()  { return s_funtrans ;}
  static Loggrade  & loggrade()  { return s_loggrade ; }
  static Mixevnt   & mixevnt()   { return s_mixevnt ;  }
  static Outpdf    & outpdf()    { return s_outpdf ;   }
  static Subopen   & subopen()   { return s_subopen ;  }
  static Usertran  & usertran()  { return s_usertran ; }
  static Vegasinf  & vegasinf()  { return s_vegasinf ; }
  static Vegasbin  & vegasbin()  { return s_vegasbin ; }
  static Valmatrix & valmatrix() { return s_valmatrix; }
  static Mtypeofxi & mtypeofxi() { return s_mtypeofxi; }
  static Wbstate   & wbstate()   { return s_wbstate  ; }

  static void SetXiccDefaultParameters( ) ;
  static void SetXiccConsistentParameters( ) ;
  static void EvntInit( ) ;

protected:

private:
	static Counter    s_counter ; 
	static Upcom      s_upcom ; 
	static Vegcross   s_vegcross ; 
	static Confine    s_confine ;  
	static Funtrans   s_funtrans ;
	static Loggrade   s_loggrade ; 
	static Mixevnt    s_mixevnt ;  
	static Outpdf     s_outpdf ;   
	static Subopen    s_subopen ;  
	static Usertran   s_usertran ; 
	static Vegasinf   s_vegasinf ; 
	static Vegasbin   s_vegasbin ; 
	static Valmatrix  s_valmatrix; 
	static Mtypeofxi  s_mtypeofxi; 
	static Wbstate    s_wbstate  ; 
};
#endif // LBGENXICC_GENXICC_H
