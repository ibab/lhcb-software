#ifndef ZOOPACKEDSTATES_H
#define ZOOPACKEDSTATES_H

#include <cassert>

#include <TObject.h>
#include <TClonesArray.h>

#include "ZooPacked.h"
#include "ZooPackedState.h"

//// class to hold a objects at given z
class ZooPackedStates : public TObject
{
private:
    TClonesArray m_array;

public:
    ZooPackedStates() : m_array(ZooPackedState::Class(), 16, true)
    { }
    unsigned size() const { return m_array.GetEntriesFast(); }
    unsigned capacity() const { return m_array.GetSize(); }
    /// set size of states array, clearing it
    void resize(unsigned newsz)
    { m_array.Delete(); m_array.Expand(newsz); }
    void clear() { m_array.Delete(); }
    /// append a state
    void push_back(const ZooPackedState& state)
    {
	// make sure we do not overflow m_array
	assert(size() < capacity());
	new(m_array[size()]) ZooPackedState(state);
    }
    /// get a state
    const ZooPackedState& operator[](unsigned idx) const
    {
	assert(idx < size());
	return *reinterpret_cast<const ZooPackedState*>(m_array[idx]);
    }

    /// get state closest to given z
    const ZooPackedState& closestState(double z) const
    {
	unsigned k = 0;
	double dist = std::numeric_limits<double>::infinity();
	for (unsigned j = 0; j < size(); ++j) {
	    const double d = std::abs(operator[](j).z() - z);
	    if (d < dist) k = j, dist = d;
	}
	return operator[](k);
    }

    ClassDef(ZooPackedStates, 1);
};

#endif
