#ifndef LOKI_BEAMLINEFUNCTIONS_H 
#define LOKI_BEAMLINEFUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
// Event
// ============================================================================
#include "Event/VertexBase.h"
#include "Event/Track.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/BeamSpot.h"
// ============================================================================
/** @file 
 *  Collection of "beam-line"-related functors 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2011-03-10
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Vertices 
  {
    // ========================================================================
    /** @class BeamSpotRho
     *  
     *  functor to evaluate the radial distance ("rho") with respect to 
     *  the middle of Velo as measured by the X and Y resolvers
     *
     *  @attention if the velo is opened return -1.
     *
     *  @see LoKi::Cuts::VX_BEAMSPOTRHO
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @author Victor COCO   Victor.Coco@cern.ch
     *  @date   2011-03-10
     */
    class GAUDI_API BeamSpotRho 
      : public LoKi::BeamSpot
      , public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
    {
    public:
      // ======================================================================
      /// Constructor from resolver bouns 
      BeamSpotRho ( const double       bound    ) ;
      /// Constructor from resolved bound and condition name 
      BeamSpotRho ( const double       bound    , 
                    const std::string& condname ) ;
      /// MANDATORY: virtual destructor 
      virtual ~BeamSpotRho() ;
      // ======================================================================
    public:
      // ======================================================================
      /** MANDATORY: clone method ("virtual constructor")
       *  @attention if the velo is opened return -1
       */
      virtual  BeamSpotRho* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      BeamSpotRho () ;                       // default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
  } //                                          end of namespace LoKi::Vertices 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef  VX_BEAMSPOTRHO 
     *  functor to evaluate the radial distance ("rho") with respect to 
     *  the middle of Velo as measured by the X and Y resolvers
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch 
     *  @author Victor COCO   Victor.Coco@cern.ch
     *  @date 2011-03-11
     */ 
    typedef LoKi::Vertices::BeamSpotRho                        VX_BEAMSPOTRHO ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_BEAMLINEFUNCTIONS_H
// ============================================================================
