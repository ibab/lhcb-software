// $Id: Particles9.h,v 1.5 2007-07-23 17:35:47 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES9_H 
#define LOKI_PARTICLES9_H 1
// ============================================================================
// Include files
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
namespace LHCb { class Track ; }
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-22 
 */
// ============================================================================
namespace LoKi
{
  namespace Particles 
  {
    // ========================================================================
    /** @class HasTracks 
     *  The simple predicate which checks if the particle 
     *  has a track from the specified list of tracks 
     *  
     *  @see LHCb::Particle
     *  @see LoKi::Extract::Particle2Track
     *  @see LHCb::Track
     * 
     *  @see LoKi::Cuts::HASTRACKS
     *  @see LoKi::Cuts::TRACKS
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-22 
     */
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
      /// constructor from vector of tracks 
      HasTracks ( const SmartRefVector<LHCb::Track>& ts ) ;
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
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      HasTracks();
    private:
      LoKi::Extract::Particle2Track  m_extract ;
    } ;
    // ========================================================================    
    /** @class HasTracksInTree
     *  The simple predicate which checks if the particle 
     *  (or any of the daughter particle) 
     *  has a track from the specified list of tracks 
     *  
     *  @see LHCb::Particle
     *  @see LoKi::Extract::Particle2Track
     *  @see LHCb::Track
     * 
     *  @see LoKi::Cuts::HASTRACKSINTREE
     *  @see LoKi::Cuts::TRACKSINTREE
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-22 
     */
    class HasTracksInTree : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// constructor from one track 
      HasTracksInTree ( const LHCb::Track* track ) ;
      /// constructor from vector of tracks 
      HasTracksInTree ( const LHCb::Track::Vector& ts ) ;
      /// constructor from vector of tracks 
      HasTracksInTree ( const LHCb::Track::ConstVector& ts ) ;
      /// constructor from vector of tracks 
      HasTracksInTree ( const SmartRefVector<LHCb::Track>& ts ) ;
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
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the defualt constructor is private 
      HasTracksInTree() ;
    private: 
      LoKi::Particles::HasTracks     m_cut     ;
    } ;
    // ========================================================================
  } // end of namespace LoKi::Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES8_H
// ============================================================================
