// $Id: GenHybridEngine.h,v 1.1 2007-07-25 15:07:57 ibelyaev Exp $
// ===========================================================================
#ifndef LOKI_GENHYBRIDENGINE_H 
#define LOKI_GENHYBRIDENGINE_H 1
// ===========================================================================
// Include files
// ===========================================================================
#include <string>
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/IGenHybridTool.h"
// ===========================================================================
namespace LoKi 
{  
  namespace Hybrid 
  {
    /** @class GenEngine GenHybridEngine.h LoKi/GenHybridEngine.h
     *  
     *  Helper class forn implementation of Hybrid Tools 
     *
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2004-06-30
     */
    class GenEngine
    {
    public:
      /// constructor
      GenEngine () ;
      /// destructor 
      virtual ~GenEngine( );
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::GCuts&   cut  ) const ;
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::GVCuts&  cut  ) const ;
      /// add the function 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::GFunc&   func ) const ;
      /// add the function 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::GVFunc&  func ) const ;
    } ;  
  } // end of namespace LoKi::Hybrid 
} // end of namespace LoKi
// ===========================================================================
// The END 
// ===========================================================================
#endif // LOKI_CUTSHOLDERHELPER_H
// ===========================================================================
