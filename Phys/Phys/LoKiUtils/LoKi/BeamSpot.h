// $Id: BeamLineFunctions.h 136843 2012-03-14 16:52:03Z piedavid $
// ============================================================================
#ifndef LOKI_BEAMSPOT_H 
#define LOKI_BEAMSPOT_H 1
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
 *                    $Revision: 136843 $
 *  Last Modification $Date: 2012-03-14 17:52:03 +0100 (Wed, 14 Mar 2012) $ 
 *                 by $Author: piedavid $ 
 */
// ============================================================================
namespace LoKi 
{
  // ========================================================================
  /** @class BeamSpot
   *  
   *  functor to pick up the beamspot position
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
  // =========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_BEAMLINEFUNCTIONS_H
// ============================================================================
