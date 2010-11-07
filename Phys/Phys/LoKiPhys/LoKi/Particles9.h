// $Id$
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
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/PhysExtract.h"
// ============================================================================
namespace LHCb 
{
  class Track ; 
  class RecVertex ;
}
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
  // ==========================================================================
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
    class GAUDI_API HasTracks
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
      , public LoKi::UniqueKeeper<LHCb::Track>
    {
    public:
      // ======================================================================
      /// constructor from one track 
      HasTracks ( const LHCb::Track* track ) ;
      /// constructor from vector of tracks 
      HasTracks ( const LHCb::Track::ConstVector& ts ) ;
      /// constructor from vector of tracks 
      HasTracks ( const LHCb::Track::Container*   ts ) ;
      /// constructor from one particle 
      HasTracks ( const LHCb::Particle* p ) ;
      /// constructor from vector of particles 
      HasTracks ( const LHCb::Particle::ConstVector& p ) ;
      /// constructor from vector of particles 
      HasTracks ( const LoKi::Types::Range&          p ) ;
      // ======================================================================
      /** templated constructor from sequence of ptoroparticles 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class TRACK>
      HasTracks 
      ( TRACK first , 
        TRACK last  ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ()
        , LoKi::UniqueKeeper<LHCb::Track> ()
      {
        addTracks ( first , last );
      } 
      // from arbitrary keeper 
      template <class TRACK> 
      HasTracks ( const LoKi::Keeper<TRACK>& tracks ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ()
        , LoKi::UniqueKeeper<LHCb::Track> () 
      {
        addTracks ( tracks.begin() , tracks.end() );
      }
      // from arbitrary keeper 
      template <class TRACK> 
      HasTracks ( const LoKi::UniqueKeeper<TRACK>& tracks ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ()
        , LoKi::UniqueKeeper<LHCb::Track> () 
      {
        addTracks ( tracks.begin() , tracks.end() );
      }
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~HasTracks() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasTracks* clone() const { return new HasTracks(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      template <class TRACK>
      std::size_t addTracks ( TRACK first , 
                              TRACK last  ) 
      {
        std::size_t _size = this->size() ;
        for ( ; first != last ; ++first ) { addTracks ( *first ) ; } 
        return size() - _size ;
      }
      // =====================================================================
    public:
      // ======================================================================
      /// check if particle has a track in list 
      bool inList ( const LHCb::Particle* p ) const ;
      /// check if particle has a track in list 
      bool inTree ( const LHCb::Particle* p ) const ;
      // ======================================================================
    protected:
      // =====================================================================
      std::size_t addTracks ( const LHCb::Track*         track    ) ;
      std::size_t addTracks ( const LHCb::Particle*      particle ) ;
      std::size_t addTracks ( const LHCb::ProtoParticle* particle ) ;
      std::size_t addTracks ( const LHCb::VertexBase*    vertex   ) ;
      std::size_t addTracks ( const LHCb::Vertex*        vertex   ) ;
      std::size_t addTracks ( const LHCb::RecVertex*     vertex   ) ;
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      HasTracks();
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Extract::Particle2Track  m_extract ;
      // ======================================================================
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
    class GAUDI_API HasTracksInTree : public HasTracks 
    {
    public:
      // ======================================================================
      /// constructor from one track 
      HasTracksInTree ( const LHCb::Track* track ) ;
      /// constructor from vector of tracks 
      HasTracksInTree ( const LHCb::Track::ConstVector& ts ) ;
      /// constructor from vector of tracks 
      HasTracksInTree ( const LHCb::Track::Container*   ts ) ;
      /// constructor from one particle 
      HasTracksInTree ( const LHCb::Particle* p ) ;
      /// constructor from vector of particles 
      HasTracksInTree ( const LHCb::Particle::ConstVector& p ) ;
      /// constructor from vector of particles 
      HasTracksInTree ( const LoKi::PhysTypes::Range&      p ) ;
      // ======================================================================
      /** templated constructor from sequence of ptoroparticles 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class TRACK>
      HasTracksInTree 
      ( TRACK first , 
        TRACK last  ) 
        : LoKi::Particles::HasTracks ( first , last ) 
      {} 
      // from arbitrary keeper 
      template <class TRACK> 
      HasTracksInTree ( const LoKi::Keeper<TRACK>& tracks ) 
        : LoKi::Particles::HasTracks ( tracks ) 
      {}
      // from arbitrary keeper 
      template <class TRACK> 
      HasTracksInTree ( const LoKi::UniqueKeeper<TRACK>& tracks ) 
        : LoKi::Particles::HasTracks ( tracks ) 
      {}
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~HasTracksInTree() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasTracksInTree* clone() const 
      { return new HasTracksInTree(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // the defualt constructor is private 
      HasTracksInTree() ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES8_H
// ============================================================================
