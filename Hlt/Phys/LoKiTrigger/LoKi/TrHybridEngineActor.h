// $Id: TrHybridEngineActor.h,v 1.1.1.1 2007-07-27 15:56:42 ibelyaev Exp $
// ===========================================================================
#ifndef LOKI_TRHYBRIDENGINEACTOR_H 
#define LOKI_TRHYBRIDENGINEACTOR_H 1
// ===========================================================================
// Include files
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/Interface.h"
#include "LoKi/ITrHybridTool.h"
// ===========================================================================
namespace LoKi
{
  namespace Hybrid
  {
    /** @class TrEngineActor TrHybridEngineActor.h LoKi/TrHybridEngineActor.h
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
    class TrEngineActor
    { 
    public:
      // get the static instance 
      static TrEngineActor& instance() ;
      /// connect the hybrid tool for code translation 
      StatusCode connectTool (       LoKi::ITrHybridTool* tool ) ;
      /// disconnect the tool 
      StatusCode releaseTool ( const LoKi::ITrHybridTool* tool ) ;
      /// propagate the cut to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::TrCuts&  cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::RVCuts&  cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::TTrCuts& cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::TrVCuts& cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::RVVCuts& cut  ) const ;
      /// propagate the function to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::TrFunc&  func ) const ;    
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::RVFunc&  func ) const ;
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::TTrFunc& func ) const ;
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::TrVFunc& func ) const ;
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::RVVFunc& func ) const ;
    public:
      /// get the reference tracks   ( for Trigger/Hlt)
      std::vector<LHCb::Track*>*     tracks   () const ;
      /// get the reference vertces  ( for Trigger/Hlt)
      std::vector<LHCb::RecVertex*>* vertices () const ;
    protected:
      /// Standard constructor
      TrEngineActor() ;
      virtual ~TrEngineActor( ); ///< Destructor
    private:
      // just to save some lines 
      template <class TYPE>
      inline StatusCode _add 
      ( const std::string& name , const TYPE& cut ) const ; ///< just to save some lines 
    private:
      // the copy contructor is disabled 
      TrEngineActor           ( const TrEngineActor& );
      // the assignement operator is disabled 
      TrEngineActor& operator=( const TrEngineActor& );
    private:
      // the tool itself 
    LoKi::Interface<LoKi::ITrHybridTool> m_tool ;
    } ;
  } // end of namespace LoKi::Hybrid 
} // end of namespace LoKi
// ===========================================================================
// The END 
// ===========================================================================
#endif // LOKI_CUTSHOLDER_H
// ===========================================================================
