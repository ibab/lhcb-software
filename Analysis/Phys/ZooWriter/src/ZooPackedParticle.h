#ifndef ZOOPACKEDPARTICLE_H
#define ZOOPACKEDPARTICLE_H

#include "ZooPacked.h"

/// packed particle
class ZooPackedParticle : public TObject
{
    private:
	ZooPackedParticleVectorWithError m_particle;
    public:
	typedef ZooPackedParticleVectorWithError::Vector Vector;
	typedef ZooPackedParticleVectorWithError::Matrix Matrix;
	/// default constructor
	ZooPackedParticle() { Class()->IgnoreTObjectStreamer(); }
	/// constructor taking position/momentum-vector and its covariance
	template<typename V> ZooPackedParticle(
		const V& posmomentum, const Matrix& cov) :
	    m_particle(posmomentum, cov)
	{ Class()->IgnoreTObjectStreamer(); }
	/// constructor taking position/momentum-vector and its covariance
	template<typename VP, typename VM> ZooPackedParticle(
		const VP& pos, const VM& mom, const Matrix& cov) :
	    m_particle(Vector(pos.x(), pos.y(), pos.z(),
			mom.x(), mom.y(), mom.z(), mom.t()), cov)
	{ Class()->IgnoreTObjectStreamer(); }

	virtual ~ZooPackedParticle();

	/// return position
	const ROOT::Math::XYZPoint pos() const
	{
	    return ROOT::Math::XYZPoint(
		    m_particle[0], m_particle[1], m_particle[2]);
	}
	/// return momentum
	const ROOT::Math::PxPyPzEVector momentum() const
	{
	    return ROOT::Math::PxPyPzEVector(m_particle[3],
		    m_particle[4], m_particle[5], m_particle[6]);
	}
	/// return covariance matrix for vector (position, momentum)
	const Matrix cov() const { return m_particle; }

	/// return underlying ZooPackedParticleVectorWithError
	const ZooPackedParticleVectorWithError& particleVector() const
	{ return m_particle; }
	/// return underlying ZooPackedParticleVectorWithError
	ZooPackedParticleVectorWithError& particleVector()
	{ return m_particle; }

	ClassDef(ZooPackedParticle, 1);
};

#endif // ZOOPACKEDPARTICLE_H

// vim:tw=78:sw=4:ft=cpp
