// $Id$
// ===========================================================================
#ifndef LOKI_TRACKENGINEACTOR_H 
#define LOKI_TRACKENGINEACTOR_H 1
// ===========================================================================
// $URL$
// ===========================================================================
// Include files
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/Interface.h"
#include "LoKi/ITrackFunctorAntiFactory.h"
// ===========================================================================
namespace LoKi
{
  // =========================================================================
  namespace Hybrid
  {
    // =======================================================================
    /** @class TrEngineActor LoKi/TrackEngineActor.h
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
    class TrackEngineActor
    { 
    public:
      // ======================================================================
      // get the static instance 
      static TrackEngineActor& instance() ;
      // ======================================================================
      /// connect the hybrid tool for code translation 
      StatusCode connectTool (       LoKi::ITrackFunctorAntiFactory* tool ) ;
      // ======================================================================
      /// disconnect the tool 
      StatusCode releaseTool ( const LoKi::ITrackFunctorAntiFactory* tool ) ;
      // ======================================================================
      /// propagate the cut to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::TrCuts&  cut  ) const ;
      // ======================================================================
      /// propagate the function to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::TrFunc&  func ) const ;    
      // ======================================================================
    public:
      // ======================================================================
      // functional part for LHCb::Track 
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrMaps&     fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrPipes&    fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrFunVals&  fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrElements& fun  ) const ;
      // ======================================================================
      /// add the function 
      StatusCode process 
      ( const std::string&             name , 
        const LoKi::Types::TrSources&  fun  ) const ;
      // ======================================================================      
    protected:
      // ======================================================================
      /// Standard constructor
      TrackEngineActor() ;
      /// virtual & protected destructor 
      virtual ~TrackEngineActor( ); // virtual & protected destructor 
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
      TrackEngineActor           ( const TrackEngineActor& ); // no-copy 
      /// the assignement operator is disabled 
      TrackEngineActor& operator=( const TrackEngineActor& ); // no-assignement 
      // ======================================================================
    private:
      // ======================================================================
      // the tool itself 
      LoKi::Interface<LoKi::ITrackFunctorAntiFactory> m_tool ;
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
