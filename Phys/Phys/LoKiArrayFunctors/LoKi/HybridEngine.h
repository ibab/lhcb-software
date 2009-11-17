// $Id: HybridEngine.h,v 1.4 2009-11-17 12:41:41 ibelyaev Exp $
// ===========================================================================
#ifndef LOKI_HYBRID_ENGINE_H 
#define LOKI_HYBRID_ENGINE_H 1
// ===========================================================================
// Include files
// ===========================================================================
#include <string>
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/IHybridTool.h"
// ===========================================================================
namespace LoKi 
{  
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class Engine LoKi/HybridEngine.h
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
     *  @date   2004-06-30
     */
    class Engine
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor
      Engine () ;                                                // constructor
      /// destructor 
      virtual ~Engine( );                                        //  destructor 
      // ======================================================================
    public:
      // ======================================================================
      // predicates:
      // ======================================================================      
      /// add the cut 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::Cuts&    cut  ) const ;
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::VCuts&   cut  ) const ;
      /// add the cut 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::ACuts&   cut  ) const ;
      // ======================================================================
      // functions 
      // ======================================================================      
      /// add the function 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::Func&    cut  ) const ;
      /// add the function 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::VFunc&   cut  ) const ;
      /// add the function 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::AFunc&   cut  ) const ;
      // ======================================================================
      // maps 
      // ======================================================================      
      /// add the "map"
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::Maps&    cut  ) const ;
      /// add the "map"
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::VMaps&   cut  ) const ;
      // ======================================================================      
      // pipes
      // ======================================================================      
      /// add the "pipe"
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::Pipes&   cut  ) const ;
      /// add the "pipe"
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::VPipes&  cut  ) const ;
      // ======================================================================      
      // fun-vals 
      // ======================================================================      
      /// add the "fun-val"
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::FunVals& cut  ) const ;
      /// add the "fun-val"
      StatusCode process 
      ( const std::string&           name , 
        const LoKi::Types::VFunVals& cut  ) const ;
      // ======================================================================      
      // cut-vals 
      // ======================================================================      
      /// add the "fun-val"
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::CutVals& cut  ) const ;
      /// add the "fun-val"
      StatusCode process 
      ( const std::string&           name , 
        const LoKi::Types::VCutVals& cut  ) const ;
      // ======================================================================      
      // elements
      // ======================================================================      
      /// add the "element"
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::Elements& cut  ) const ;
      /// add the "fun-val"
      StatusCode process 
      ( const std::string&            name , 
        const LoKi::Types::VElements& cut  ) const ;
      // ======================================================================      
      // sources
      // ======================================================================      
      /// add the "source"
      StatusCode process
      ( const std::string&            name , 
        const LoKi::Types::Sources&   cut  ) const ;
      /// add the "source"
      StatusCode process 
      ( const std::string&            name , 
        const LoKi::Types::VSources&  cut  ) const ;
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
