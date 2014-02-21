// $Id$
// ===========================================================================
#ifndef LOKI_GENCUTSHOLDER_H
#define LOKI_GENCUTSHOLDER_H 1
// ===========================================================================
// Include files
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/Interface.h"
#include "LoKi/IGenHybridTool.h"
// ===========================================================================
/** @file
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid
  {
    // ========================================================================
    /** @class GenEngineActor LoKi/GenHybridEngineActor.h
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
    class GenEngineActor
    {
    public:
      // ======================================================================
      // get the static instance
      static GenEngineActor& instance() ;
      /// connect the hybrid tool for code translation
      StatusCode connectTool (       LoKi::IGenHybridTool* tool ) ;
      /// disconnect the tool
      StatusCode releaseTool ( const LoKi::IGenHybridTool* tool ) ;
      // ======================================================================
    public:
      // ======================================================================
      // predicates
      // ======================================================================
      /// propagate the cut to the tool
      StatusCode process
      ( const std::string&          name ,
        const LoKi::Types::GCuts&   cut  ) const ;
      /// propagate the cut to the tool
      StatusCode process
      ( const std::string&          name ,
        const LoKi::Types::GVCuts&  cut  ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // functions
      // ======================================================================
      /// propagate the function to the tool
      StatusCode process
      ( const std::string&          name ,
        const LoKi::Types::GFunc&   func ) const ;
      /// propagate the function to the tool
      StatusCode process
      ( const std::string&          name ,
        const LoKi::Types::GVFunc&  func ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // maps
      // ======================================================================
      /// propagate the map to the tool
      StatusCode process
      ( const std::string&          name ,
        const LoKi::Types::GMaps&   func ) const ;
      /// propagate the map to the tool
      StatusCode process
      ( const std::string&          name ,
        const LoKi::Types::GVMaps&  func ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // pipes
      // ======================================================================
      /// propagate the pipe to the tool
      StatusCode process
      ( const std::string&          name ,
        const LoKi::Types::GPipes&  func ) const ;
      /// propagate the pipe to the tool
      StatusCode process
      ( const std::string&          name ,
        const LoKi::Types::GVPipes& func ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // fun-vals
      // ======================================================================
      /// propagate the fun-val to the tool
      StatusCode process
      ( const std::string&            name ,
        const LoKi::Types::GFunVals&  func ) const ;
      /// propagate the fun-val to the tool
      StatusCode process
      ( const std::string&            name ,
        const LoKi::Types::GVFunVals& func ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // cut-vals
      // ======================================================================
      /// propagate the fun-val to the tool
      StatusCode process
      ( const std::string&            name ,
        const LoKi::Types::GCutVals&  func ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // sources
      // ======================================================================
      /// propagate the source to the tool
      StatusCode process
      ( const std::string&            name ,
        const LoKi::Types::GSources&  func ) const ;
      /// propagate the source to the tool
      StatusCode process
      ( const std::string&            name ,
        const LoKi::Types::GVSources& func ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// Standard constructor
      GenEngineActor() ;
      virtual ~GenEngineActor( ); // Destructor
      // ======================================================================
    private:
      // ======================================================================
      /// just to save some lines
      template <class TYPE>
      inline StatusCode _add
      ( const std::string& name , const TYPE& cut ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // the copy contructor is disabled
      GenEngineActor           ( const GenEngineActor& );
      // the assignement operator is disabled
      GenEngineActor& operator=( const GenEngineActor& );
      // ======================================================================
    private:
      // ======================================================================
      // the tool itself
      LoKi::Interface<LoKi::IGenHybridTool> m_tool ;
      // ======================================================================
    };
    // ========================================================================
  } //                                             end of namespace LoKi:Hybrid
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_MCCUTSHOLDER_H
// ============================================================================
