// $Id: HltEngine.h,v 1.2 2008-09-23 13:13:35 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HltENGINE_H 
#define LOKI_HltENGINE_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHltAntiFactory.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class HltEngine LoKi/HltEngine.h
     *  
     *  Helper class for implementation of C++/Python "Hybrid" solution
     *
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-09-18
     */
    class HltEngine
    {
    public: 
      // ======================================================================
      /// Standard constructor
      HltEngine () ; 
      // ======================================================================
      // destructor 
      virtual ~HltEngine () ; // Destructor
      // ======================================================================      
    public: // L0
      // ======================================================================
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::L0_Cuts& cut  ) const ;
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::L0_Func& cut  ) const ;
      // ======================================================================
    public: // O1 
      // ======================================================================
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::ODIN_Cuts& cut  ) const ;
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::ODIN_Func& cut  ) const ;
      // ======================================================================
    public: // HLT
      // ======================================================================
      /// add the cut 
      StatusCode process 
      ( const std::string&           name , 
        const LoKi::Types::HLT_Cuts& cut  ) const ;
      /// add the cut 
      StatusCode process 
      ( const std::string&           name , 
        const LoKi::Types::HLT_Func& cut  ) const ;
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Hybrid 
  // ==========================================================================
} //end of namespace LoKi 
// ============================================================================
// The END  
// ============================================================================
#endif // LOKI_HltENGINE_H
// ============================================================================
