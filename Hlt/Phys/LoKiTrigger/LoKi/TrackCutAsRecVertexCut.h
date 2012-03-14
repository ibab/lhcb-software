// ============================================================================
#ifndef LOKI_TRACKCUTASRECVERTEXCUT_H
#define LOKI_TRACKCUTASRECVERTEXCUT_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
#include "Event/RecVertex.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace RecVertices
  {
    // ========================================================================
    /**
     * @class Hlt_TrackCutAsRecVertexCut_Any
     *
     * Adapter from track functor to vertex functor, where the vertex passes if
     * any of its tracks passes.
     *
     * @author Pieter David pieter.david@cern.ch
     * @date   2012-02-27
     */
    class GAUDI_API Hlt_TrackCutAsRecVertexCut_Any
      : public LoKi::BasicFunctors<const LHCb::RecVertex*>::Predicate
    {
      // ======================================================================
    public:
      // ======================================================================
      /** Constructor from track cut
       * @param cut  track cut
       */
      Hlt_TrackCutAsRecVertexCut_Any
      ( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut ,
        bool useExtraInfo = false                                     )
      : LoKi::BasicFunctors<const LHCb::RecVertex*>::Predicate()
      , m_cut          ( cut )
      , m_useExtraInfo ( useExtraInfo )
      { if ( useExtraInfo ) { retrieveFailKey() ; } }
      /// MANDATORY: virtual destructor
      virtual ~Hlt_TrackCutAsRecVertexCut_Any() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Hlt_TrackCutAsRecVertexCut_Any* clone() const
      { return new Hlt_TrackCutAsRecVertexCut_Any( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << "TCASRVC_ANY(" << m_cut << ")" ; }
      // ======================================================================
    private:
      // ======================================================================
      Hlt_TrackCutAsRecVertexCut_Any();  // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Assignable<LoKi::BasicFunctors<const LHCb::Track*>::Predicate>::Type m_cut;
      // ======================================================================
      void retrieveFailKey();
      // ======================================================================
      bool m_useExtraInfo;
      int  m_extraInfoKey;
      // ======================================================================
    };
    // ========================================================================
    /**
     * @class Hlt_TrackCutAsRecVertexCut_All
     *
     * Adapter from track functor to vertex functor, where all tracks of the
     * vertex are required to pass for the vertex to pass.
     *
     * @author Pieter David pieter.david@cern.ch
     * @date   2012-01-02
     */
    class GAUDI_API Hlt_TrackCutAsRecVertexCut_All
      : public LoKi::BasicFunctors<const LHCb::RecVertex*>::Predicate
    {
      // ======================================================================
    public:
      // ======================================================================
      /** Constructor from track cut
       * @param cut  track cut
       */
      Hlt_TrackCutAsRecVertexCut_All
      ( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut )
      : m_cut( cut )
      {}
      /// MANDATORY: virtual destructor
      virtual ~Hlt_TrackCutAsRecVertexCut_All () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Hlt_TrackCutAsRecVertexCut_All* clone () const
      { return new Hlt_TrackCutAsRecVertexCut_All( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << "TCASRVC_ALL(" << m_cut << ")" ; }
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Assignable<LoKi::BasicFunctors<const LHCb::Track*>::Predicate>::Type m_cut;
      // ======================================================================
    };
    // ========================================================================
  } //                                                    end of namespace Hlt1
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    typedef LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_Any  HLT_TCASRVC_ANY;
    // ========================================================================
    typedef LoKi::RecVertices::Hlt_TrackCutAsRecVertexCut_All  HLT_TCASRVC_ALL;
    // ========================================================================
  } //                                                    end of namespace Cuts
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_TRACKCUTASRECVERTEXCUT_H
// ============================================================================
