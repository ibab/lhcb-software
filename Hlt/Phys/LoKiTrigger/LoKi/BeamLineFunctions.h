// $Id: $
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
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2011-03-10
 *
 *                    $Revision:$
 *  Last Modification $Date:$ 
 *                 by $Author:$ 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Vertices 
  {
    // ========================================================================
    /** @class BeamSpot LoKi/BeamLineFunctions.h
     *  
     *  functor to evaluate the radial distance ("rho") with respect to 
     *  the middle of Velo as measured by the X and Y resolvers
     *
     *  @see LoKi::Cuts::BEAMSPOT
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @author Victor COCO   Victor.Coco@cern.ch
     *  @date   2011-03-10
     */
    class GAUDI_API BeamSpot
      : virtual public LoKi::AuxFunBase
    {
    public:
      // ======================================================================
      /// Constructor from bound
      BeamSpot( const double bound );
      /// Copy constructor  
      BeamSpot ( const BeamSpot& );
      /// MANDATORY: virtual destructor 
      virtual ~BeamSpot();
      // ======================================================================
    public:
      // ======================================================================
      /// update the condition
      StatusCode     updateCondition ();
      double         x()      const { return m_beamSpotX;  }
      double         y()      const { return m_beamSpotY;  }
      bool           closed() const { return m_veloClosed; }
      // ======================================================================
    protected:
      // ======================================================================
      // resolver bound
      double resolverBound() const { return m_resolverBound; }
    private:
      // ======================================================================
      /// register condition 
      StatusCode   registerCondition ();
      /// unregister condition 
      StatusCode unregisterCondition ();
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor
      BeamSpot();
      // ======================================================================
    private:
      // ======================================================================
      /// resolver bound for closure
      double m_resolverBound; 
      /// velo closed condition
      bool m_veloClosed ;
      /// beam spot
      double m_beamSpotX ;   
      double m_beamSpotY ;   
      /// condition name 
      std::string                m_condName ;
      /// the condition
      LoKi::Interface<Condition> m_condition ;              // the condition 
      // ======================================================================
    };
    // ========================================================================
    // ========================================================================
    /** @class BeamSpotRho LoKi/BeamLineFunctions.h
     *  
     *  functor to evaluate the radial distance ("rho") with respect to 
     *  the middle of Velo as measured by the X and Y resolvers
     *
     *  @see LoKi::Cuts::VX_BEAMSPOTRHO
     *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
     *  @author Victor COCO   Victor.Coco@cern.ch
     *  @date   2011-03-10
     */
    class GAUDI_API BeamSpotRho 
       : public BeamSpot,
         public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
    {
    public:
      // ======================================================================
      /// Constructor from condition name 
      BeamSpotRho ( const double bound ) ;
      /// Copy constructor  
      BeamSpotRho ( const BeamSpotRho& ) ;
      /// MANDATORY: virtual destructor 
      virtual ~BeamSpotRho() ;
      /// MANDATOTY: clone method ("virtual constructor")
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
    /** @typedef  BEAMSPOT
     *  helper functor to obtain the beamspot x and y coordinates from the
     *  Velo resolver.
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch 
     *  @author Victor COCO   Victor.Coco@cern.ch
     *  @date 2011-03-11
     */ 
    typedef LoKi::Vertices::BeamSpot                                 BEAMSPOT ;
  }
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_BEAMLINEFUNCTIONS_H
// ============================================================================
