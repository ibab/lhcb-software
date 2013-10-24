// $Id$
// ============================================================================
#ifndef LOKI_HYBRID_ENGINE_ACTOR_H 
#define LOKI_HYBRID_ENGINE_ACTOR_H 1
// ============================================================================
// Include files
#include <stack>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/IHybridTool.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid 
  {
    // ========================================================================
    /** @class EngineActor LoKi/HybridEngineActor.h
     *  
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  By usage of this code one clearly states the disagreement 
     *  with the campain of Dr.O.Callot et al.: 
     *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2004-06-29
     */
    class EngineActor
    { 
      // ======================================================================
    public:
      // ======================================================================
      /// get the static instance 
      static EngineActor& instance() ;               // get the static instance 
      /// connect the hybrid tool for code translation 
      StatusCode connectTool (       LoKi::IHybridTool* tool ) ;
      /// disconnect the tool 
      StatusCode releaseTool ( const LoKi::IHybridTool* tool ) ;
      // ======================================================================
    public:
      // ======================================================================
      // predicates 
      // ======================================================================
      /// propagate the cut to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::Cuts&    cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::VCuts&   cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::ACuts&   cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::PPCuts&  cut  ) const ;
      // ======================================================================
      // functions 
      // ======================================================================
      /// propagate the function to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::Func&    cut  ) const ;    
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::VFunc&   cut  ) const ;
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::AFunc&   cut  ) const ;
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::PPFunc&  cut  ) const ;
      // ======================================================================
      // maps 
      // ======================================================================
      /// propagate the map to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::Maps&    cut  ) const ;
      /// propagate the map to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::VMaps&   cut  ) const ;
      /// propagate the map to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::PPMaps&  cut  ) const ;
      // ======================================================================
      // pipes
      // ======================================================================
      /// propagate the pipe to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::Pipes&   cut  ) const ;
      /// propagate the pipe to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::VPipes&  cut  ) const ;
      /// propagate the pipe to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::PPPipes& cut  ) const ;
      // ======================================================================
      // fun-vals
      // ======================================================================
      /// propagate the fun-val to the tool 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::FunVals&  cut  ) const ;
      /// propagate the fun-val to the tool 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::VFunVals& cut  ) const ;
      /// propagate the fun-val to the tool 
      StatusCode process
      ( const std::string&            name , 
        const LoKi::Types::PPFunVals& cut  ) const ;
      // ======================================================================
      // cut-vals
      // ======================================================================
      /// propagate the cut-val to the tool 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::CutVals&  cut  ) const ;
      /// propagate the cut-val to the tool 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::VCutVals& cut  ) const ;
      /// propagate the cut-val to the tool 
      StatusCode process
      ( const std::string&            name , 
        const LoKi::Types::PPCutVals& cut  ) const ;
      // ======================================================================
      // sources 
      // ======================================================================
      /// propagate the source to the tool 
      StatusCode process
      ( const std::string&            name , 
        const LoKi::Types::Sources&   cut  ) const ;
      /// propagate the source to the tool 
      StatusCode process
      ( const std::string&            name , 
        const LoKi::Types::VSources&  cut  ) const ;
      /// propagate the source to the tool 
      StatusCode process
      ( const std::string&            name , 
        const LoKi::Types::PPSources& cut  ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// Standard constructor
      EngineActor() ;                                   // Standard constructor 
      // Destructor
      virtual ~EngineActor() ;                          //           Destructor
      // ======================================================================
    private:
      // ======================================================================
      /// just to save some lines 
      template <class TYPE>
      inline StatusCode _add 
      ( const std::string& ename ,
        const TYPE&        cut   ) const ; // just to save some lines 
      // ======================================================================
    private:
      // ======================================================================
      // the copy contructor is disabled 
      EngineActor           ( const EngineActor& );
      // ther assignement operator is disabled 
      EngineActor& operator=( const EngineActor& );
      // ======================================================================
    private:
      // ======================================================================
      /// a stack of tools -- since tools might instantiate other tools we need
      /// to keep track of the stack
      std::stack<LoKi::Interface<LoKi::IHybridTool> > m_tool ; // stack of tools
      // ======================================================================
    };
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CUTSHOLDER_H
// ============================================================================
