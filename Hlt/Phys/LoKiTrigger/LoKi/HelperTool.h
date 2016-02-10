// $Id$
// ============================================================================
#ifndef LOKI_HELPERTOOL_H
#define LOKI_HELPERTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <unordered_map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiKernel/ObjectVector.h"
#include "Relations/Relation1D.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
#include "Event/HltCandidate.h"
#include "Event/HltProtoParticle.h"
#include "Event/HltParticle.h"
// ============================================================================
// TrackInterfaces
// ============================================================================
#include "TrackInterfaces/ITracksFromTrack.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/AuxFunBase.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/Listener.h"
// ============================================================================
/** @file
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
 *  @date   2010-10-30
 *
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class HelperTool
     *  Helper class for implementation of upgrade & match tools
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-30
     */
    class GAUDI_API HelperTool : public virtual LoKi::Listener
    {
    public:
      // ======================================================================
      /// constructor with the fake argument
      HelperTool ( const int dummy ) ;
      // ======================================================================
      /// (very) non-trivial copy constructor
      HelperTool ( const HelperTool& right ) ;
      // ======================================================================
      /// virtual destructor
      virtual ~HelperTool() ;                             // virtual destructor
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      HelperTool () ;                    // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /** handle the incidents:
       *  clear the involved pointers
       */
      virtual void handle ( const Incident& /* incident */ ) ;
      // ======================================================================
    protected:
      // ======================================================================
      template <class TYPE>
      inline const TYPE* _add_
      ( TYPE*               object    ,
        ObjectVector<TYPE>* container ) const
      {
        if ( 0 != object && 0 != container ) { container -> push_back ( object ) ; }
        return object ;
      }
      // ========================================================================
      template <class TYPE, class MAPPING>
      inline const TYPE* _add_
      ( TYPE*                         object    ,
        KeyedContainer<TYPE,MAPPING>* container ) const
      {
        if ( 0 != object && 0 != container ) { container -> insert ( object ) ; }
        return object ;
      }
      // ======================================================================
      template <class TYPE>
      inline TYPE* _create ( typename TYPE::Container* cnt ) const
      {
        TYPE* _new = new TYPE() ;
        _add_ ( _new , cnt ) ;
        return _new ;
      }
      // ======================================================================
      template<class OBJECT>
      LHCb::Relation1D<OBJECT, OBJECT>*
      _createCache( const std::string& location, bool create ) const
      {
         using OBJECTS = LHCb::Relation1D<OBJECT, OBJECT>;
         if ( create ) {
         return alg()->getOrCreate<OBJECTS, OBJECTS>( location );
         } else {
            return alg()->getIfExists<OBJECTS>( location );
      }
      }
    protected:
      // ======================================================================
      /// get the own name
      const std::string myName() const
      {
        if ( m_myname.empty() ){ m_myname = this->printOut() ; }
        return m_myname ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// get the algorithm
      const GaudiAlgorithm* alg   () const { return m_alg   ; } // get the algorithm
      // ======================================================================
    protected:
      // ======================================================================
      /// get the stored tracks
      inline LHCb::Track::Container* storedTracks
        ( const std::string& location ) const
      {
        if ( ! m_hlt_tracks ) { m_hlt_tracks = _createTracks ( location ) ; }
        return m_hlt_tracks ;
      }
      // ======================================================================
      /// get the cache
      template<class Object>
        inline LHCb::Relation1D<Object, Object>* cache
        ( std::string location, bool create = true ) const
      {
        using Cache = LHCb::Relation1D<Object, Object>*;
        if (location.empty()) 
        {
          alg()->error() << "Got empty cache location for "
                         << alg()->name() << endmsg;
          return nullptr;
        }
        auto found = m_cached.find(location);
        if (found == end(m_cached)) 
        {
          IRelationBase* relation = _createCache<Object>(location, create);
          if ( !relation ) return nullptr;
          m_cached.emplace(std::make_pair(std::move(location), relation));
          return static_cast<Cache>(relation);
        } 
        else { return static_cast<Cache>(found->second); }
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// get new candidate
      inline Hlt::Candidate*  newCandidate     () const
      {
        if ( !m_hlt_candidates  ) { m_hlt_candidates = _createCandidates  () ; }
        return _create<Hlt::Candidate> ( m_hlt_candidates   ) ;
      }
      /// get new stage
      inline Hlt::Stage*      newStage         () const
      {
        if ( !m_hlt_stages      ) { m_hlt_stages = _createStages      () ; }
        return _create<Hlt::Stage>      ( m_hlt_stages     ) ;
      }
      /// get new multitrack
      inline Hlt::MultiTrack* newMultiTrack    () const
      {
        if ( ! m_hlt_multitracks ) { m_hlt_multitracks = _createMultiTracks () ; }
        return _create<Hlt::MultiTrack> ( m_hlt_multitracks ) ;
      }
      /// get new vertex
      inline LHCb::RecVertex* newRecVertex     () const
      {
        if ( !m_hlt_recvertices ) { m_hlt_recvertices = _createRecVertices () ; }
        return _create<LHCb::RecVertex> ( m_hlt_recvertices ) ;
      }
      /// get new proto-particle
      inline LHCb::ProtoParticle* newProtoParticle () const
      {
        if ( !m_hlt_protoparticles ) { m_hlt_protoparticles = _createProtoParticles () ; }
        return _create<LHCb::ProtoParticle> ( m_hlt_protoparticles ) ;
      }
      /// get new particle
      inline LHCb::Particle* newParticle () const
      {
        if ( !m_hlt_particles ) { m_hlt_particles = _createParticles () ; }
        return _create<LHCb::Particle> ( m_hlt_particles ) ;
      }
      // ======================================================================
    protected: // store the stuff 
      // ======================================================================
      inline bool _store ( const LHCb::RecVertex* v ) const
      {
        if ( !v                     ) { return false ; }
        if ( v->parent()            ) { return true  ; }
        if ( !m_hlt_recvertices     ) { m_hlt_recvertices = _createRecVertices () ; }
        if ( !m_hlt_recvertices     ) { return false ; }
        //
        LHCb::RecVertex* _v = const_cast<LHCb::RecVertex*> ( v ) ;
        _add_ ( _v , m_hlt_recvertices ) ;
        //
        return 0 != v->parent() ;
      }
      // ======================================================================
      inline bool _storeProtoParticle ( const LHCb::ProtoParticle* p ) const
      {
        if ( !p               ) { return false ; }
        if ( !m_hlt_protoparticles ) { m_hlt_protoparticles = _createProtoParticles () ; }
        if ( !m_hlt_protoparticles ) { return false ; }
        LHCb::ProtoParticle* _p = const_cast<LHCb::ProtoParticle*> ( p ) ;
        _add_ ( _p , m_hlt_protoparticles ) ;
        return 1;
      }
      // ======================================================================
      inline bool _storeParticle ( const LHCb::Particle* p ) const
      {
        if ( !p               ) { return false ; }
        if ( !m_hlt_particles ) { m_hlt_particles = _createParticles () ; }
        if ( !m_hlt_particles ) { return false ; }
        LHCb::Particle* _p = const_cast<LHCb::Particle*> ( p ) ;
        _add_ ( _p , m_hlt_particles ) ;
        return 1;
      }
      // ======================================================================
      inline bool _storeVertex ( const LHCb::Vertex* v ) const
      {
        if ( !v               ) { return false ; }
        if ( !m_hlt_vertices  ) { m_hlt_vertices = _createVertices () ; }
        if ( !m_hlt_vertices  ) { return false ; }
        LHCb::Vertex* _v = const_cast<LHCb::Vertex*> ( v ) ;
        _add_ ( _v , m_hlt_vertices ) ;
        return 1;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// get the storage for tracks
      LHCb::Track::Container*         _createTracks 
        ( const std::string& location ) const ;
      /// get the storage for candidates
      Hlt::Candidate::Container*      _createCandidates     () const ;
      /// get the storage for stages
      Hlt::Stage::Container*          _createStages         () const ;
      /// get the storage for multitracks
      Hlt::MultiTrack::Container*     _createMultiTracks    () const ;
      /// get the storage for rec-vertices
      LHCb::RecVertex::Container*     _createRecVertices    () const ;
      /// get the storage for proto-particles
      LHCb::ProtoParticle::Container* _createProtoParticles () const ;
      /// get the storage for particles
      LHCb::Particle::Container*      _createParticles      () const ;
      /// get the storage for particles
      LHCb::Vertex::Container*        _createVertices       () const ;
      // ======================================================================
    private:
      // ======================================================================
      /// container of Hlt-candidates
      mutable Hlt::Candidate::Container*       m_hlt_candidates  ;
      /// container of Hlt-stages
      mutable Hlt::Stage::Container*           m_hlt_stages      ;
      /// container of Hlt-multitracks
      mutable Hlt::MultiTrack::Container*      m_hlt_multitracks ;
      /// container of tracks
      mutable LHCb::Track::Container*          m_hlt_tracks      ;
      /// container of vertices
      mutable LHCb::RecVertex::Container*      m_hlt_recvertices ;
      /// container of ProtoParticles
      mutable LHCb::ProtoParticle::Container*  m_hlt_protoparticles    ;
      /// container of Particles
      mutable LHCb::Particle::Container*       m_hlt_particles         ;
      /// container of Vertices
      mutable LHCb::Vertex::Container*         m_hlt_vertices          ;
      // ======================================================================
      mutable std::string                      m_myname                ;
      // ======================================================================
      mutable std::unordered_map<std::string, IRelationBase*> m_cached ;
      // ======================================================================
    private: // algorith
      // ======================================================================      
      /// the algorithm
      mutable const GaudiAlgorithm*       m_alg   ;  // the algorithm
      // ======================================================================
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
} // ==========================================================================
//                                                        end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_HELPERTOOL_H
// ============================================================================
