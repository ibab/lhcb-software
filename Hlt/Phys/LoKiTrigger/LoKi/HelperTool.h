// $Id: $
// ============================================================================
#ifndef LOKI_HELPERTOOL_H 
#define LOKI_HELPERTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiKernel/ObjectVector.h"
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
#include "Event/HltCandidate.h"
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
class GaudiAlgorithm ;
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
    class HelperTool : public virtual LoKi::Listener
    {
    public:
      // ======================================================================
      ///
      HelperTool ( const int dummy ) ;
      // ======================================================================
      /// copy constructor 
      HelperTool ( const HelperTool& right ) ;
      // ======================================================================
      /// destructor 
      virtual ~HelperTool() ;
      // ======================================================================
    private:
      // ======================================================================
      /// the edfault constructor is disabled 
      HelperTool() ;                     // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /** handle the incidents:
       *  clear the involved pointers 
       */
      virtual void handle ( const Incident& /* incident */ ) ;
      // ======================================================================
    private:
      // ======================================================================
      template <class TYPE>
      inline const TYPE* _add_ 
      ( TYPE*               object    , 
        ObjectVector<TYPE>* container ) const 
      {
        if ( 0 != object && 0 != container ) { container -> push_back ( object ) ; }
        return object ;
      } ;
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
    protected:
      // ======================================================================      
      /// get the algorithm
      const GaudiAlgorithm* alg() const { return m_alg ; } // get the algorithm
      // ======================================================================
    protected:
      // ======================================================================      
      /// get the stored tracks 
      inline LHCb::Track::Container* storedTracks 
      ( const std::string& location ) const 
      {
        if ( 0 == m_hlt_tracks ) { _createTracks ( location ) ; }
        return m_hlt_tracks ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// get new candidate 
      inline Hlt::Candidate*  newCandidate     () const 
      {
        if ( 0 == m_hlt_candidates  ) { _createCandidates  () ; }
        return _create<Hlt::Candidate> ( m_hlt_candidates   ) ;
      }
      /// get new stage 
      inline Hlt::Stage*      newStage         () const 
      {
        if ( 0 == m_hlt_stages       ) { _createStages      () ; }
        return _create<Hlt::Stage>      ( m_hlt_stages     ) ;
      }
      /// get new multitarck
      inline Hlt::MultiTrack* newMultiTrack    () const 
      {
        if ( 0 == m_hlt_multitracks ) { _createMultiTracks () ; }
        return _create<Hlt::MultiTrack> ( m_hlt_multitracks ) ;
      }
      /// get new vertex 
      inline LHCb::RecVertex* newRecVertex     () const 
      {
        if ( 0 == m_hlt_recvertices ) { _createRecVertices () ; }
        return _create<LHCb::RecVertex> ( m_hlt_recvertices ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// get the storage for tracks 
      LHCb::Track::Container*     _createTracks
      ( const std::string& location ) const ;
      /// get the storage for candidates 
      Hlt::Candidate::Container*  _createCandidates  () const ;
      /// get the storage for stages 
      Hlt::Stage::Container*      _createStages      () const ;
      /// get the storage for multitracks 
      Hlt::MultiTrack::Container* _createMultiTracks () const ;
      /// get the storage for rec-vertices 
      LHCb::RecVertex::Container* _createRecVertices () const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the algorithm 
      mutable const GaudiAlgorithm*       m_alg             ;  // the algorithm 
      // ======================================================================
      /// container of Hlt-candidates 
      mutable Hlt::Candidate::Container*  m_hlt_candidates  ;
      /// container of Hlt-stages 
      mutable Hlt::Stage::Container*      m_hlt_stages      ;      
      /// container of Hlt-multitracks 
      mutable Hlt::MultiTrack::Container* m_hlt_multitracks ;      
      /// container of tracks 
      mutable LHCb::Track::Container*     m_hlt_tracks      ;      
      /// container of vertices  
      mutable LHCb::RecVertex::Container* m_hlt_recvertices ;      
      // ======================================================================
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1 
} // ==========================================================================
//                                                        end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HELPERTOOL_H
// ============================================================================
