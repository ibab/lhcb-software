// $Id: MCHybridEngineActor.h,v 1.2 2008-02-19 15:04:01 ibelyaev Exp $
// ===========================================================================
#ifndef LOKI_MCCUTSHOLDER_H 
#define LOKI_MCCUTSHOLDER_H 1
// ===========================================================================
// Include files
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/Interface.h"
#include "LoKi/IMCHybridTool.h"
// ===========================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid
  {
    // ========================================================================
    /** @class MCEngineActor MCHybridEngineActor.h LoKi/MCHybridEngineActor.h
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
     *
     *                    $Revision$
     *  Last modification $Date$
     *                 by $Author$
     */
    class GAUDI_API MCEngineActor
    { 
    public:
      // ======================================================================
      // get the static instance 
      static MCEngineActor& instance() ;
      /// connect the hybrid tool for code translation 
      StatusCode connectTool (       LoKi::IMCHybridTool* tool ) ;
      /// disconnect the tool 
      StatusCode releaseTool ( const LoKi::IMCHybridTool* tool ) ;
      // ======================================================================
    public:
      // ======================================================================
      // predicates:
      // ======================================================================     
      /// propagate the cut to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::MCCuts&  cut  ) const ;
      /// propagate the cut to the tool 
      StatusCode process 
      ( const std::string&          name , 
        const LoKi::Types::MCVCuts& cut  ) const ;
      // ======================================================================     
    public:
      // ======================================================================
      // functions: 
      // ======================================================================     
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::MCFunc&  func ) const ;
      /// propagate the function to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::MCVFunc& func ) const ;
      // ======================================================================     
    public:
      // ======================================================================
      // maps:
      // ======================================================================     
      /// propagate the map to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::MCMaps&  func ) const ;
      /// propagate the map to the tool 
      StatusCode process
      ( const std::string&          name , 
        const LoKi::Types::MCVMaps& func ) const ;
      // ======================================================================     
    public:
      // ======================================================================
      // pipes:
      // ======================================================================     
      /// propagate the pipe to the tool 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::MCPipes&  func ) const ;
      /// propagate the pipe to the tool 
      StatusCode process
      ( const std::string&           name , 
        const LoKi::Types::MCVPipes& func ) const ;
      // ======================================================================     
    public:
      // ======================================================================
      // fun-vals:
      // ======================================================================     
      /// propagate the funval to the tool 
      StatusCode process
      ( const std::string&             name , 
        const LoKi::Types::MCFunVals&  func ) const ;
      /// propagate the funval to the tool 
      StatusCode process
      ( const std::string&             name , 
        const LoKi::Types::MCVFunVals& func ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // sources:
      // ======================================================================     
      /// propagate the source to the tool 
      StatusCode process
      ( const std::string&              name , 
        const LoKi::Types::MCSources&   func ) const ;
      /// propagate the source to the tool 
      StatusCode process
      ( const std::string&              name , 
        const LoKi::Types::MCVSources&  func ) const ;
      // ======================================================================
    protected:
      // ======================================================================     
      /// Standard constructor
      MCEngineActor() ;
      /// desctructor 
      virtual ~MCEngineActor( ); // Destructor
      // ======================================================================     
    private:
      // ======================================================================
      // just to save some lines 
      template <class TYPE>
      inline StatusCode _add 
      ( const std::string& name , const TYPE& cut ) const ; 
      // ======================================================================
    private:
      // ======================================================================
      // the copy contructor is disabled 
      MCEngineActor           ( const MCEngineActor& );
      // ther assignement operator is disabled 
      MCEngineActor& operator=( const MCEngineActor& );
      // ======================================================================
    private:
      // ======================================================================
      // the tool itself 
      LoKi::Interface<LoKi::IMCHybridTool> m_tool ;
      // ======================================================================
    };
    // ========================================================================
  } //                                           end of namespace LoKi::Hybrid
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_MCCUTSHOLDER_H
// ============================================================================
