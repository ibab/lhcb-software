// $Id$
// ============================================================================
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
// ============================================================================
// Condition
// ============================================================================
#include "DetDesc/Condition.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2011-03-10
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Vertices 
  {
    // ========================================================================
    /** @class BeamSpot
     *  
     *  functor to evaluate the radial distance ("rho") with respect to 
     *  the middle of Velo as measured by the X and Y resolvers
     *
     *  @see LoKi::Cuts::BEAMSPOT
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @author Victor COCO   Victor.Coco@cern.ch
     *  @date   2011-03-10
     */
    class GAUDI_API BeamSpot : virtual public LoKi::AuxFunBase
    {
    public:
      // ======================================================================
      /// Constructor from bound
      BeamSpot ( const double       bound    );
      /// Constructor from bound and condition name 
      BeamSpot ( const double       bound    , 
                 const std::string& condname ) ;
      /// Copy constructor  
      BeamSpot ( const BeamSpot& );
      /// MANDATORY: virtual destructor 
      virtual ~BeamSpot() ;
      // ======================================================================
    public:
      // ======================================================================
      /// update the condition
      virtual  StatusCode  updateCondition ();
      // ======================================================================
    public:
      // ====================================================================== 
      double             resolverBound () const { return m_resolverBound ; }
      double             x             () const { return m_beamSpotX     ; }
      double             y             () const { return m_beamSpotY     ; }
      bool               closed        () const { return m_veloClosed    ; }
      const std::string& condName      () const { return m_condName      ; }
      // ======================================================================
    private:
      // ======================================================================
      /// register condition 
      StatusCode   registerCondition () ;
      /// unregister condition 
      StatusCode unregisterCondition () ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor
      BeamSpot();
      // ======================================================================
    private:
      // ======================================================================
      /// resolver bound for closure
      double      m_resolverBound ;               // resolver bound for closure
      /// velo closed condition
      bool        m_veloClosed    ;               //      velo closed condition
      /// beam spot-X
      double      m_beamSpotX     ;               //                beam spot-X
      /// beam spot-Y
      double      m_beamSpotY     ;               //                beam spot-Y
      /// condition name 
      std::string m_condName      ;               //             condition name 
      /// the condition itself 
      LoKi::Interface<Condition> m_condition ;    //       the condition itself 
      // ======================================================================
    };
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
      : public LoKi::Vertices::BeamSpot
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
  namespace Tracks
  {
    // ========================================================================
    /**
     *  @class FastDOCAToBeamLine
     *  use TTrDOCA and BEAMSPOT to evaluate the closest distance of a track to
     *  the beam line, similar to BEAMSPOTRHO
     *  @author Pieter David pieter.david@cern.ch
     *  @date 2012-02-24
     */
    class GAUDI_API FastDOCAToBeamLine
      : LoKi::Vertices::BeamSpot
      , public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // =====================================================================
      /// Constructor from resolver bound
      FastDOCAToBeamLine ( const double       bound    ) ;
      /// Constructor from resolved bound and condition name
      FastDOCAToBeamLine ( const double       bound    ,
                           const std::string& condname ) ;
      /// MANDATORY: virtual destructor
      virtual ~FastDOCAToBeamLine() {}
      /// update the condition
      StatusCode     updateCondition ();
      /// Copy constructor
      FastDOCAToBeamLine( const FastDOCAToBeamLine& other ) ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  FastDOCAToBeamLine* clone() const
      { return new FastDOCAToBeamLine( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s << "Tr_FASTDOCATOBEAMLINE"; }
    private:
      /// beamline tracks
      LHCb::Track m_beamLine;
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Tracks
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
    /** @typedef  BEAMSPOT
     *  helper functor to obtain the beamspot x and y coordinates from the
     *  Velo resolver.
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch 
     *  @author Victor COCO   Victor.Coco@cern.ch
     *  @date 2011-03-11
     */ 
    typedef LoKi::Vertices::BeamSpot                                 BEAMSPOT ;
    // ========================================================================
    /** @typedef  Tr_FASTDOCATOBEAMLINE
     *  Fast DOCA to beam line using TTrDOCA and BEAMSPOT, similar to the
     *  BEAMSPOTRHO cut
     *  @see LoKi::Cuts::TTrDOCA
     *  @see HltUtils::closestDistanceMod
     *  @see LoKi::Tracks::DistanceOfClosestApproach
     *  @see LoKi::Vertices::BeamSpot
     *  @see LoKi::Cuts::BEAMSPOT
     *  @see LoKi::Vertices::BeamSpotRho
     *  @see LoKi::Cuts::BEAMSPOTRHO
     *  @author Pieter David pieter.david@cern.ch
     *  @date 2012-02-24
     */
    typedef LoKi::Tracks::FastDOCAToBeamLine            Tr_FASTDOCATOBEAMLINE ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_BEAMLINEFUNCTIONS_H
// ============================================================================
