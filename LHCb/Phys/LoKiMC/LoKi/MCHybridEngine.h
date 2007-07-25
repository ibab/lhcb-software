// $Id: MCHybridEngine.h,v 1.1 2007-07-25 15:16:06 ibelyaev Exp $
// ===========================================================================
#ifndef LOKI_MCHYBRIDENGINE_H 
#define LOKI_MCHYBRIDENGINE_H 1
// ===========================================================================
// Include files
// ===========================================================================
#include <string>
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/IMCHybridTool.h"
// ===========================================================================
namespace LoKi 
{
  namespace Hybrid 
  { 
    /** @class MCEngine MCHybridEngine.h LoKi/MCHybridEngine.h
     *  
     *  Helper class for the implementation of Hybrid Tools 
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
    class MCEngine
    {
    public:
      /// constructor
      MCEngine () ;
      /// destructor 
      virtual ~MCEngine( );
      /// add the cut 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::MCCuts&  cut  ) const ;
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::MCVCuts& cut  ) const ;
      /// add the function 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::MCFunc&  func ) const ;
      /// add the function 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::MCVFunc& func ) const ;
    } ;  
  } // end of namespace LoKi::Hybrid 
} // end of namespace LoKi
// ===========================================================================
// The END 
// ===========================================================================
#endif // LOKI_CUTSHOLDERHELPER_H
// ===========================================================================
