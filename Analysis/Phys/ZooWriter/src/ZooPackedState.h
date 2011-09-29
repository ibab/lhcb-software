#ifndef ZOOPACKEDSTATE_H
#define ZOOPACKEDSTATE_H

#include "ZooPacked.h"

/// packed track state at given z
class ZooPackedState : public ZooPackedStateVectorWithError
{
    public:
	ZooPackedState() : m_z(std::numeric_limits<PackedT>::min()) { }

	template<typename V> ZooPackedState(
		UnpackedT z, const V& vector, const Matrix& cov) :
	    ZooPackedStateVectorWithError(vector, cov)
        { setZ(z); }

	virtual ~ZooPackedState();

	/// return z of state
	UnpackedT z() const
	{ return UnitsT::unpack(m_z); }

	void setZ(UnpackedT z)
	{ m_z =  UnitsT::pack(z); }

    private:
	PackedT m_z;
	ClassDef(ZooPackedState, 1);
};

#endif // ZOOPACKEDSTATE_H

// vim:tw=78:sw=4:ft=cpp
