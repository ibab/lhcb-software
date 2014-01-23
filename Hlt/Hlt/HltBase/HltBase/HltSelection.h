// $Id: HltSelection.h,v 1.18 2010-08-17 08:49:18 graven Exp $
#ifndef HLTBASE_HLTSELECTION_H
#define HLTBASE_HLTSELECTION_H 1

#include <vector>
#include <boost/utility.hpp>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/ClassID.h"

#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"

namespace Hlt {

template <typename T> class TSelection;

class Selection : private boost::noncopyable {
  public:
    Selection( const Gaudi::StringKey &id )
        : m_id( id ), m_decision( false ), m_processed( false ), m_error( false ) {}
    virtual ~Selection() {}

    const Gaudi::StringKey &id() const { return m_id; }
    const std::vector<Gaudi::StringKey> &inputSelectionsIDs() const {
        return m_inputSelectionsIDs;
    }

    template <typename I> // I is assumed to be iterator over a range of Selection*
    void addInputSelectionIDs( I i, I end ) {
        while ( i != end ) m_inputSelectionsIDs.push_back( ( *i++ )->id() );
    }

    void setDecision( bool value ) {
        m_decision = value;
        m_processed = true;
    }
    void setError( bool value ) { m_error = value; }

    bool decision() const { return m_decision; }
    bool processed() const { return m_processed; }
    bool error() const {
        return m_error;
    } // TODO: is bool really what we want??? do we want more???

    virtual CLID classID() const { return CLID_NULL; }
    virtual void clean() {
        m_decision = false;
        m_processed = false;
        m_error = false;
    }

    template <typename T> TSelection<T> *down_cast() {
        return T::classID() == classID() ? dynamic_cast<TSelection<T> *>( this )
                                         : (TSelection<T> *)0;
    }
    template <typename T> const TSelection<T> *down_cast() const {
        return T::classID() == classID()
                   ? dynamic_cast<const TSelection<T> *>( this )
                   : (const TSelection<T> *)0;
    }

  public:
    virtual size_t size() const { return 0; }
    virtual bool empty() const { return true; }

  private:
    std::vector<Gaudi::StringKey> m_inputSelectionsIDs;
    Gaudi::StringKey m_id;
    bool m_decision;  // accept / reject
    bool m_processed; // did we actually set the decision?
    bool m_error;     // did an error occur during processing?
};

// TODO: can we use Gaudi::Range instead of having an std::vector<T*> ???
//      or do we get in trouble with 'insert' and 'erase'? Can we do
//      that external to a selection?
template <typename T> class TSelection : public Selection {
  public:
    typedef const T candidate_type;
    typedef std::vector<candidate_type *> container_type;

    TSelection( const Gaudi::StringKey &id ) : Selection( id ) {}
    virtual ~TSelection();

    CLID classID() const { return T::classID(); }
    void clean();

    // forward container functionality..
    typedef typename container_type::iterator iterator;
    typedef typename container_type::value_type value_type;
    typedef typename container_type::size_type size_type;
    typedef typename container_type::const_iterator const_iterator;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::reference reference;
    void reserve( size_t s ) { m_candidates.reserve( s ); }
    bool empty() const { return m_candidates.empty(); }
    size_type size() const { return m_candidates.size(); }
    reference front() { return m_candidates.front(); }
    const_reference front() const { return m_candidates.front(); }
    const_iterator begin() const { return m_candidates.begin(); }
    const_iterator end() const { return m_candidates.end(); }
    iterator begin() { return m_candidates.begin(); }
    iterator end() { return m_candidates.end(); }
    void push_back( candidate_type *t ) { m_candidates.push_back( t ); }
    template <typename ITER> void insert( iterator i, ITER begin, ITER end ) {
        m_candidates.insert( i, begin, end );
    }
    template <typename ITER> ITER erase( ITER begin, ITER end ) {
        return m_candidates.erase( begin, end );
    }
    // ========================================================================
  public:
    // ========================================================================
    /// expose the internal container
    const container_type &vct() const { return m_candidates; }
    /// impliict cast to the underlying container
    operator const container_type &() const { return vct(); }
    // ========================================================================
  private:
    container_type m_candidates; // we do NOT own these...
};

typedef TSelection<LHCb::Track> TrackSelection;
typedef TSelection<LHCb::RecVertex> VertexSelection;
typedef TSelection<LHCb::Particle> ParticleSelection;

template <typename R, typename T>
void operator>>( const R &range, TSelection<T> &sel ) {
    sel.insert( sel.end(), range.begin(), range.end() );
}

template <typename T>
void operator>>( typename TSelection<T>::candidate_type *c, TSelection<T> &sel ) {
    sel.push_back( c );
}
}
#endif
