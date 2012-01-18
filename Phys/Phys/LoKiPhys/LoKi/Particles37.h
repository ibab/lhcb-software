// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES37_H 
#define LOKI_PARTICLES37_H 1
// ============================================================================
// Incldue files 
// ============================================================================
// Event model
// ============================================================================
#include "Event/RecVertex.h"
#include "Event/Particle.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Vertices1.h"
#include "LoKi/AuxDesktopBase.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/Particles4.h"
#include "LoKi/Particles20.h"
// ============================================================================
/** @file LoKi/Particles37.h
 *  Collection of functors to calculate the decay length
 *  significance of a particle with respect to a vertex.
 *  The decay length significance is defined as the
 *  projection onto the momentum of a particle of the
 *  distance between the particle and a primary vertex,
 *  divided by the error on the distance.
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Roel Aaij roel.aaij@cern.ch
 *  @date 2010-06-18
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Particles 
  { 
    // ========================================================================
    /** @class DecayLengthSignificance 
     *  Functor which calculates the decay length significance of a
     *  particle with respect to a primary vertex.
     *  @code
     *    
     *    LHCb::VertexBase* pv = ...;
     *
     *    // get the DLS functor
     *    const DLS_FUN p1 = DLS( pv );
     *
     *    const LHCb::Particle* B = ...;
     *  
     *    const double val = p1 ( B );
     *
     *  @endcode 
     *
     *  @author Roel Aaij
     *  @date   2010-06-18
     */
    class GAUDI_API DecayLengthSignificance 
      : public LoKi::BasicFunctors< const LHCb::Particle* >::Function
      , public LoKi::Vertices::VertexHolder 
    {
    public:
      // ======================================================================
      /// constructor from a vertex
      DecayLengthSignificance ( const LHCb::VertexBase* vertex );
      /// constructor from a point
      DecayLengthSignificance ( const LoKi::Point3D& vertex );
      /// copy constructor
      DecayLengthSignificance ( const LoKi::Vertices::VertexHolder& base );
      /// MANDATORY: virtual destructor
      virtual ~DecayLengthSignificance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual DecayLengthSignificance* clone() const;
      /// The method where the actual calculation happens.
      virtual result_type dls( argument p ) const;
      /// MANDATORY: the only essential method
      virtual result_type operator()( argument p ) const;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream( std::ostream& stream ) const;
      // ======================================================================
    private:
      // ======================================================================
      DecayLengthSignificance();
      // ======================================================================
    };
    // ========================================================================
    /** @class DecayLengthSignificanceDV
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the decay length significance.
     *  @see DecayLengthSignificance
     *
     *  @code
     *    
     *    // get the DLS functor
     *    const DLS_FUN fun = DLS();
     *
     *    const LHCb::Particle* B = ...;
     *  
     *    const double dls = fun ( B );
     *
     *  @endcode 
     *
     *  @author Roel Aaij
     *  @date   2010-05-07
     */
    class GAUDI_API DecayLengthSignificanceDV
      : public         DecayLengthSignificance
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// basic constructor
      DecayLengthSignificanceDV();
      /// MANDATORY: virtual destructor
      virtual ~DecayLengthSignificanceDV(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DecayLengthSignificanceDV* clone() const
      { return new DecayLengthSignificanceDV( *this ); }
      /// MANDATORY: the only essential method
      virtual result_type operator()( argument p ) const;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s << "BPVDLS"; }
      // ======================================================================
    };
    // ========================================================================
    /** @class PathDistance
     *  get the path distance 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path 
     *  @see LoKi::Cuts::PATHDIST
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class GAUDI_API PathDistance : public LoKi::Particles::ImpPar 
    {
    public:
      // ======================================================================
      /// constructor from the primary vertex & tool 
      PathDistance 
      ( const LHCb::VertexBase*                        pv     , 
        const IDistanceCalculator*                     tool   ) ;
      /// constructor from the primary vertex & tool 
      PathDistance ( const LoKi::Vertices::ImpParBase& tool   ) ;
      /// MANDATORY: virtual destructor 
      virtual ~PathDistance() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PathDistance* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default constructor is disabled 
      PathDistance() ;                   // The default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PathDistanceChi2
     *  get the path distance chi2  
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::PathDistance 
     *  @see LoKi::Cuts::PATHDISTCHI2 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class GAUDI_API PathDistanceChi2 : public LoKi::Particles::PathDistance 
    {
    public:
      // ======================================================================
      /// constructor from the primary vertex & tool 
      PathDistanceChi2 
      ( const LHCb::VertexBase*                        pv     , 
        const IDistanceCalculator*                     tool   ) ;
      /// constructor from the primary vertex & tool 
      PathDistanceChi2 ( const LoKi::Vertices::ImpParBase& tool   ) ;
      /// MANDATORY: virtual destructor 
      virtual ~PathDistanceChi2() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PathDistanceChi2* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default constructor is disabled 
      PathDistanceChi2 () ;              // The default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PathDistanceSignificance
     *  get the path distance significnace  
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::PathDistance 
     *  @see LoKi::Particles::PathDistanceChi2 
     *  @see LoKi::Cuts::PATHDISTSIGNIFICANCE  
     *  @see LoKi::Cuts::PDS 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class GAUDI_API PathDistanceSignificance 
      : public LoKi::Particles::PathDistanceChi2
    {
    public:
      // ======================================================================
      /// constructor from the primary vertex & tool 
      PathDistanceSignificance
      ( const LHCb::VertexBase*                        pv     , 
        const IDistanceCalculator*                     tool   ) ;
      /// constructor from the primary vertex & tool 
      PathDistanceSignificance ( const LoKi::Vertices::ImpParBase& tool   ) ;
      /// MANDATORY: virtual destructor 
      virtual ~PathDistanceSignificance () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PathDistanceSignificance* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default constructor is disabled 
      PathDistanceSignificance () ;      // The default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ProjDistance
     *  get the ''projected-distance''
     *  @see IDistanceCalculator::projectedDistance 
     *  @see LoKi::Cuts::PROJDIST
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class GAUDI_API ProjectedDistance : public LoKi::Particles::PathDistance 
    {
    public:
      // ======================================================================
      /// constructor from the primary vertex & tool 
      ProjectedDistance 
      ( const LHCb::VertexBase*                        pv     , 
        const IDistanceCalculator*                     tool   ) ;
      /// constructor from the primary vertex & tool 
      ProjectedDistance ( const LoKi::Vertices::ImpParBase& tool   ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ProjectedDistance () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProjectedDistance* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default constructor is disabled 
      ProjectedDistance () ;             // The default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ProjectedDistanceSignificance
     *  get the ''projected-distance'' significance 
     *  @see IDistanceCalculator::projectedDistance 
     *  @see LoKi::Cuts::PROJDS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class GAUDI_API ProjectedDistanceSignificance 
      : public LoKi::Particles::ProjectedDistance
    {
    public:
      // ======================================================================
      /// constructor from the primary vertex & tool 
      ProjectedDistanceSignificance 
      ( const LHCb::VertexBase*                        pv     , 
        const IDistanceCalculator*                     tool   ) ;
      /// constructor from the primary vertex & tool 
      ProjectedDistanceSignificance ( const LoKi::Vertices::ImpParBase& tool   ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ProjectedDistanceSignificance () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProjectedDistanceSignificance* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default constructor is disabled 
      ProjectedDistanceSignificance () ; // The default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PathDistanceWithBestPV
     *  get the path distance 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path 
     *  @see LoKi::Cuts::BPVPATHDIST
     *  @see LoKi::Cuts::BPVPATHDIST_
     *  @see LoKi::Cuts::PATHDIST
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class GAUDI_API PathDistanceWithBestPV
      : public LoKi::Particles::ImpParWithTheBestPV  
    {
    public:
      // ======================================================================
      /// constructor from tool nickname 
      PathDistanceWithBestPV ( const std::string& geo = "" )  ;
      /// MANDATORY: virtual destructor 
      virtual ~PathDistanceWithBestPV() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PathDistanceWithBestPV* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PathDistanceChi2WithBestPV
     *  get the path distance 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path 
     *  @see LoKi::Cuts::BPVPATHDISTCHI2 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class PathDistanceChi2WithBestPV 
      : public LoKi::Particles::PathDistanceWithBestPV 
    {
    public:
      // ======================================================================
      /// constructor from tool nickname 
      PathDistanceChi2WithBestPV ( const std::string& geo = "" )  ;
      /// MANDATORY: virtual destructor 
      virtual ~PathDistanceChi2WithBestPV() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PathDistanceChi2WithBestPV* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PathDistanceSignificanceWithBestPV
     *  get the path distance 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path 
     *  @see LoKi::Cuts::BPVPDS 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class GAUDI_API PathDistanceSignificanceWithBestPV 
      : public LoKi::Particles::PathDistanceChi2WithBestPV 
    {
    public:
      // ======================================================================
      /// constructor from tool nickname 
      PathDistanceSignificanceWithBestPV ( const std::string& geo = "" )  ;
      /// MANDATORY: virtual destructor 
      virtual ~PathDistanceSignificanceWithBestPV() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PathDistanceSignificanceWithBestPV* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ProjectedDistanceWithBestPV
     *  get the path distance 
     *  @see IDistanceCalculator::projectedDistance 
     *  @see LoKi::Cuts::BPVPROJDIST
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class GAUDI_API ProjectedDistanceWithBestPV 
      : public LoKi::Particles::PathDistanceWithBestPV
    {
    public:
      // ======================================================================
      /// constructor from tool nickname 
      ProjectedDistanceWithBestPV ( const std::string& geo = "" )  ;
      /// MANDATORY: virtual destructor 
      virtual ~ProjectedDistanceWithBestPV() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProjectedDistanceWithBestPV* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ProjectedDistanceSignificanceWithBestPV
     *  get the path distance 
     *  @see IDistanceCalculator::projectedDistance 
     *  @see LoKi::Particles::ImpPar::path 
     *  @see LoKi::Cuts::BPVPROJDS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-23
     */
    class GAUDI_API ProjectedDistanceSignificanceWithBestPV 
      : public LoKi::Particles::ProjectedDistanceWithBestPV
    {
    public:
      // ======================================================================
      /// constructor from tool nickname 
      ProjectedDistanceSignificanceWithBestPV ( const std::string& geo = "" )  ;
      /// MANDATORY: virtual destructor 
      virtual ~ProjectedDistanceSignificanceWithBestPV() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProjectedDistanceSignificanceWithBestPV* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                        end of namespace LoKi:: Particles
  // ==========================================================================
  namespace Cuts 
  {  
    // ========================================================================
    /** @typedef DLS
     *  Functor which calculates the decay length significance of a
     *  particle with respect to a primary vertex.
     *  @see LoKi::Particles::DecayLengthSignificance
     *
     *  @code
     *    
     *    const LHCb::VertexBase* pv = ...;
     *
     *    // get the DLS functor
     *    const DLS p1 = DLS( pv );
     *
     *    const LHCb::Particle* B = ...;
     *  
     *    const double val = p1 ( B );
     *
     *  @endcode 
     *
     *  @author Roel Aaij
     *  @date   2010-06-18
     */
    typedef LoKi::Particles::DecayLengthSignificance                      DLS ;
    // ========================================================================
    /** @typedef BPVDLS
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the decay length significance.
     *  @see LoKi::Particles::DecayLengthSignificance
     *  @see LoKi::Particles::DecayLengthSignificanceDV
     *
     *  @author Roel Aaij
     *  @date   2010-05-07
     */
    typedef LoKi::Particles::DecayLengthSignificanceDV                 BPVDLS ;
    // ========================================================================
    /** @typedef PATHDIST 
     *  Functor which calculates the 'path-distance'
     *  particle with respect to a primary vertex.
     *
     *  @code
     *    
     *    const LHCb::VertexBase* pv = ...;
     *
     *    // get the PATHDIST functor
     *    const PATHDIST  fun = PATHDIST( pv );
     *
     *    const LHCb::Particle* B = ...;
     *  
     *    const double distance = fun ( B );
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::PathDistance 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::PathDistance                            PATHDIST ;
    // ========================================================================
    /** @typedef PATHDISTCHI2 
     *  Functor which calculates the chi2 for 'path-distance'
     *  particle with respect to a primary vertex.
     *
     *  @code
     *    
     *    const LHCb::VertexBase* pv = ...;
     *
     *    // get the PATHDISTCHI2 functor
     *    const PATHDISTCHI2  fun = PATHDISTCHI2 ( pv );
     *
     *    const LHCb::Particle* B = ...;
     *  
     *    const double chi2 = fun ( B );
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::PathDistanceChi2
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @see LoKi::Cuts::PATHDIST 
     *  @see LoKi::Cuts::PATHDISTSIGNIFICANCE  
     *  @see LoKi::Cuts::PDS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::PathDistanceChi2                    PATHDISTCHI2 ;
    // ========================================================================
    /** @typedef PATHDISTSIGNIFICANCE
     *  Functor which calculates the significance for 'path-distance'
     *  particle with respect to a primary vertex.
     *
     *  @code
     *    
     *    const LHCb::VertexBase* pv = ...;
     *
     *    // get the functor
     *    const PATHDISTSIGNIFICANCE  fun = PATHDISTSIGNIFICANCE ( pv );
     *
     *    const LHCb::Particle* B = ...;
     *  
     *    const double chi2 = fun ( B );
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::PathDistanceSignificance
     *  @see LoKi::Particles::PathDistanceChi2
     *  @see LoKi::Particles::PathDistance
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @see LoKi::Cuts::PATHDIST 
     *  @see LoKi::Cuts::PATHDISTCHI2 
     *  @see LoKi::Cuts::PDS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::PathDistanceSignificance    PATHDISTSIGNIFICANCE ;
    // ========================================================================
    /** @typedef PDS
     *  Functor which calculates the significance for 'path-distance'
     *  particle with respect to a primary vertex.
     *
     *  @code
     *    
     *    const LHCb::VertexBase* pv = ...;
     *
     *    // get the functor
     *    const PDS  fun = PDS ( pv );
     *
     *    const LHCb::Particle* B = ...;
     *  
     *    const double chi2 = fun ( B );
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::PathDistanceSignificance
     *  @see LoKi::Particles::PathDistanceChi2
     *  @see LoKi::Particles::PathDistance
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @see LoKi::Cuts::PATHDIST 
     *  @see LoKi::Cuts::PATHDISTCHI2 
     *  @see LoKi::Cuts::PATHDISTANCESIGNIFICANCE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::PathDistanceSignificance                     PDS ;
    // ========================================================================
    /** @typedef PROJDIST 
     *  Functor which calculates the 'projected-distance'
     *  particle with respect to a primary vertex.
     *
     *  @code
     *    
     *    const LHCb::VertexBase* pv = ...;
     *
     *    // get the functor
     *    const PROJDIST  fun = PROJDIST( pv );
     *
     *    const LHCb::Particle* B = ...;
     *  
     *    const double distance = fun ( B );
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::ProjectedDistance 
     *  @see IDistanceCalculator::projectedDistance 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::ProjectedDistance                       PROJDIST ;
    // ========================================================================
    /** @typedef PROJDS 
     *  Functor which calculates the 'projected-distance' significance 
     *  particle with respect to a primary vertex.
     *
     *  @code
     *    
     *    const LHCb::VertexBase* pv = ...;
     *
     *    // get the functor
     *    const PROJDS  fun = PROJDS ( pv );
     *
     *    const LHCb::Particle* B = ...;
     *  
     *    const double distance = fun ( B );
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::ProjectedDistanceSignificance 
     *  @see IDistanceCalculator::projectedDistance 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::ProjectedDistanceSignificance             PROJDS ;
    // ========================================================================
    /** @var BPVPATHDIST
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the decay length significance.
     *
     *  @see LoKi::Particles::PathDistanceWithBestPV 
     *  @see LoKi::Particles::PathDistance 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @see LoKi::Cuts::PATHDIST
     *  @see LoKi::Cuts::BPVPATHDIST_
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    const LoKi::Particles::PathDistanceWithBestPV                 BPVPATHDIST ;
    // ========================================================================
    /** @typedef BPVPATHDIST_
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the decay length significance.
     
     *
     *  @see LoKi::Particles::PathDistanceWithBestPV 
     *  @see LoKi::Particles::PathDistance 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @see LoKi::Cuts::PATHDIST
     *  @see LoKi::Cuts::BPVPATHDIST
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::PathDistanceWithBestPV              BPVPATHDIST_ ;
    // ========================================================================
    /** @var BPVPATHDISTCHI2 
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the decay length significance.
     *
     *  @see LoKi::Particles::PathDistanceChi2WithBestPV 
     *  @see LoKi::Particles::PathDistanceChi2 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    const LoKi::Particles::PathDistanceChi2WithBestPV         BPVPATHDISTCHI2 ;
    // ========================================================================
    /** @typedef BPVPATHDISTCHI2_
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the decay length significance.
     *
     *  @see LoKi::Particles::PathDistanceChi2WithBestPV 
     *  @see LoKi::Particles::PathDistanceChi2 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @see LoKi::Cuts::PATHDIST
     *  @see LoKi::Cuts::BPVPATHDIST
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::PathDistanceChi2WithBestPV      BPVPATHDISTCHI2_ ;
    // ========================================================================
    /** @var BPVPDS 
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the decay length significance.
     *
     *  @see LoKi::Particles::PathDistanceSiginficanceWithBestPV 
     *  @see LoKi::Particles::PathDistanceSignificance 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    const LoKi::Particles::PathDistanceSignificanceWithBestPV          BPVPDS ;
    // ========================================================================
    /** @typedef BPVPDS_
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the decay length significance.
     *
     *  @see LoKi::Particles::PathDistanceSignificanceWithBestPV 
     *  @see LoKi::Particles::PathDistanceSignificance 
     *  @see IDistanceCalculator::pathDistance 
     *  @see LoKi::Particles::ImpPar::path
     *  @see LoKi::Cuts::PATHDIST
     *  @see LoKi::Cuts::BPVPATHDIST
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::PathDistanceSignificanceWithBestPV       BPVPDS_ ;
    // ========================================================================
    /** @var BPVPROJDIST
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the projected disatnce 
     *
     *  @see LoKi::Particles::ProjectedDistanceWithBestPV
     *  @see IDistanceCalculator::projectedDistance 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    const LoKi::Particles::ProjectedDistanceWithBestPV            BPVPROJDIST ;
    // ========================================================================
    /** @typedef BPVPROJDIST_ 
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the projected disatnce 
     *
     *  @see LoKi::Particles::ProjectedDistanceWithBestPV
     *  @see IDistanceCalculator::projectedDistance 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::ProjectedDistanceWithBestPV         BPVPROJDIST_ ;
    // ========================================================================
    /** @var BPVPROJDS
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the sigibnficance of projected distance 
     *
     *  @see LoKi::Particles::ProjectedDistanceSignificanceWithBestPV
     *  @see IDistanceCalculator::projectedDistance 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    const LoKi::Particles::ProjectedDistanceSignificanceWithBestPV BPVPROJDS ;
    // ========================================================================
    /** @typedef BPVPROJDS_ 
     *  Functor which uses the best primary vertex from the
     *  PhysDesktop to calculate the significance of projected distance 
     *
     *  @see LoKi::Particles::ProjectedDistanceWithBestPV
     *  @see IDistanceCalculator::projectedDistance 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-23
     */
    typedef LoKi::Particles::ProjectedDistanceSignificanceWithBestPV BPVPROJDS_ ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // FUNCTORS_H
// ============================================================================
