// $Id: HybridEngineActor.h,v 1.3 2008-02-19 16:13:07 ibelyaev Exp $
// ===========================================================================
#ifndef LOKI_HYBRID_ENGINE_ACTOR_H 
#define LOKI_HYBRID_ENGINE_ACTOR_H 1
// ===========================================================================
// Include files
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/Interface.h"
#include "LoKi/IHybridTool.h"
// ===========================================================================
namespace LoKi
{
  namespace Hybrid 
  {
    /** @class EngineActor HybridEngineActor.h LoKi/HybridEngineActor.h
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
     *  @date   2004-06-29
     */
    class EngineActor
    { 
    public:
      // get the static instance 
      static EngineActor& instance() ;
      /// connect the hybrid tool for code translation 
      StatusCode connectTool (       LoKi::IHybridTool* tool ) ;
      /// disconnect the tool 
      StatusCode releaseTool ( const LoKi::IHybridTool* tool ) ;
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
      /// propagate the function to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::Func&    cut  ) const ;    
      // ======================================================================
      // functions 
      // ======================================================================
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::VFunc&   cut  ) const ;
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::AFunc&   cut  ) const ;
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
      // ======================================================================
      // elements:
      // ======================================================================
      /// propagate the element to the tool 
      StatusCode process
      ( const std::string&            name , 
        const LoKi::Types::Elements&  cut  ) const ;
      /// propagate the element to the tool 
      StatusCode process
      ( const std::string&            name , 
        const LoKi::Types::VElements& cut  ) const ;
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
      // ======================================================================
    protected:
      /// Standard constructor
      EngineActor() ;
      virtual ~EngineActor( ); ///< Destructor
    private:
      // just to save some lines 
      template <class TYPE>
      inline StatusCode _add 
      ( const std::string& name , const TYPE& cut ) const ; ///< just to save some lines 
    private:
      // the copy contructor is disabled 
      EngineActor           ( const EngineActor& );
      // ther assignement operator is disabled 
      EngineActor& operator=( const EngineActor& );
    private:
      // the tool itself 
      LoKi::Interface<LoKi::IHybridTool> m_tool ;
    };
  } // end of namespace LoKi::Hybrid
} // end of namespace LoKi
// ===========================================================================
// The END 
// ===========================================================================
#endif // LOKI_CUTSHOLDER_H
// ===========================================================================
