// $Id: Particles9.h,v 1.1 2006-02-22 20:53:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_PARTICLES9_H 
#define LOKI_PARTICLES9_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Track.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/UniqueKeeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysExtract.h"
// ============================================================================

// ============================================================================
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-22 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  {
    
    class HasTracks
      : public LoKi::Predicate<const LHCb::Particle*>
      , public LoKi::UniqueKeeper<LHCb::Track>
    {
    public:
      /// constructor from one track 
      HasTracks ( const LHCb::Track* track ) ;
      /// constructor from vector of tracks 
      HasTracks ( const LHCb::Track::Vector& ts ) ;
      /// constructor from vector of tracks 
      HasTracks ( const LHCb::Track::ConstVector& ts ) ;
      /// constructor from container of tracks 
      HasTracks ( const LoKi::Keeper<LHCb::Track>& ps ) ;
      /// constructor from container of tracks 
      HasTracks ( const LoKi::UniqueKeeper<LHCb::Track>& ps ) ;
      /** templated constructor from sequence of ptoroparticles 
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       */
      template <class TRACK>
      HasTracks 
      ( TRACK first , 
        TRACK last  ) 
        : LoKi::Predicate<const LHCb::Particle*>()
        , LoKi::UniqueKeeper<LHCb::Track> ( first , last ) 
      {} ;
      /// copy constructor 
      HasTracks ( const HasTracks& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasTracks() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasTracks* clone() const { return new HasTracks(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& filStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      HasTracks();
    private:
      LoKi::Extract::Particle2Track  m_extract ;
    } ;
    
    class HasTracksInTree
      : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// constructor from one track 
      HasTracksInTree ( const LHCb::Track* track ) ;
      /// constructor from vector of tracks 
      HasTracksInTree ( const LHCb::Track::Vector& ts ) ;
      /// constructor from vector of tracks 
      HasTracksInTree ( const LHCb::Track::ConstVector& ts ) ;
      /// constructor from container of tracks 
      HasTracksInTree ( const LoKi::Keeper<LHCb::Track>& ps ) ;
      /// constructor from container of tracks 
      HasTracksInTree ( const LoKi::UniqueKeeper<LHCb::Track>& ps ) ;
      /** templated constructor from sequence of ptoroparticles 
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       */
      template <class TRACK>
      HasTracksInTree 
      ( TRACK first , 
        TRACK last  ) 
        : LoKi::Predicate<const LHCb::Particle*>()
        , m_cut ( first , last )
      {} ;
      /// copy constructor 
      HasTracksInTree 
      ( const HasTracksInTree& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasTracksInTree() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasTracksInTree* clone() const 
      { return new HasTracksInTree(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& filStream( std::ostream& s ) const ;
    private:
      // the defualt constructor is private 
      HasTracksInTree() ;
    private: 
      LoKi::Particles::HasTracks     m_cut     ;
    } ;
      
  } ; // end of namespace LoKi::Particles
} ; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES8_H
// ============================================================================
