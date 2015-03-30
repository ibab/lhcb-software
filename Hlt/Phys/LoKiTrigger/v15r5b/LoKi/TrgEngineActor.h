// $Id$
// ===========================================================================
#ifndef LOKI_TRGENGINEACTOR_H 
#define LOKI_TRGENGINEACTOR_H 1
// ===========================================================================
// Include files
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/Interface.h"
#include "LoKi/ITrgFunctorAntiFactory.h"
// ===========================================================================
namespace LoKi
{
  // =========================================================================
  namespace Hybrid
  {
    // =======================================================================
    /** @class TrgEngineActor LoKi/TrgEngineActor.h
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
     *
     *  $Revision$
     *  Last Modification $Date$ 
     *                 by $Author$ 
     */
    class TrgEngineActor
    { 
    public:
      // ======================================================================
      // get the static instance 
      static TrgEngineActor& instance() ;
      // ======================================================================
      /// connect the hybrid tool for code translation 
      StatusCode connectTool (       LoKi::ITrgFunctorAntiFactory* tool ) ;
      // ======================================================================
      /// disconnect the tool 
      StatusCode releaseTool ( const LoKi::ITrgFunctorAntiFactory* tool ) ;
      // ======================================================================
      /// propagate the cut to the tool 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::TC_Cuts&  cut  ) const ;
      // ======================================================================
      /// propagate the function to the tool 
      StatusCode process 
      ( const std::string&           name , 
        const LoKi::Types::TC_Func&  func ) const ;    
      // ======================================================================
    public:
      // ======================================================================
      // functional part for LHCb::TC_ack 
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
    protected:
      // ======================================================================
      /// Standard constructor
      TrgEngineActor() ;
      /// virtual & protected destructor 
      virtual ~TrgEngineActor( ); // virtual & protected destructor 
      // ======================================================================      
    private:
      // ======================================================================
      /// just to save some lines 
      template <class TYPE>
      inline StatusCode _add 
      ( const std::string& name , 
        const TYPE&        cut )  const ; // save some lines 
      // ======================================================================
    private:
      // ======================================================================
      /// the copy contructor is disabled 
      TrgEngineActor           ( const TrgEngineActor& ); // no-copy 
      /// the assignement operator is disabled 
      TrgEngineActor& operator=( const TrgEngineActor& ); // no-assignement 
      // ======================================================================
    private:
      // ======================================================================
      // the tool itself 
      LoKi::Interface<LoKi::ITrgFunctorAntiFactory> m_tool ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CUTSHOLDER_H
// ============================================================================
