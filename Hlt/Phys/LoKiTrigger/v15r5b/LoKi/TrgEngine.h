// $Id$
// ===========================================================================
#ifndef LOKI_TRGENGINE_H 
#define LOKI_TRGENGINE_H 1
// ===========================================================================
// Include files
// ===========================================================================
#include <string>
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/TriggerTypes.h"
// ===========================================================================
namespace LoKi 
{  
  // =========================================================================
  namespace Hybrid 
  {
    // =======================================================================
    /** @class TrgEngine LoKi/TrgEngine.h
     *  
     *  Helper class for implementation of Hybrid Tools 
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
     *  @date   2004-06-30e
     *
     *  $Revision$
     *  Last Modification $Date$ 
     *                 by $Author$ 
     */
    class TrgEngine
    {
    public:
      // ======================================================================
      /// constructor
      TrgEngine () ;
      /// destructor 
      virtual ~TrgEngine( );
      // ======================================================================
    public:
      // ======================================================================
      /// add the cut 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::TC_Cuts&  cut  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::TC_Func&  func ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // functional part
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&              name , 
        const LoKi::Types::TC_Maps&     fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&              name , 
        const LoKi::Types::TC_Pipes&    fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&              name , 
        const LoKi::Types::TC_FunVals&  fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&              name , 
        const LoKi::Types::TC_Sources&  fun  ) const ;
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CUTSHOLDERHELPER_H
// ============================================================================
