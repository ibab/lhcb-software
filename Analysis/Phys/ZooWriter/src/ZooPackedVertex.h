#ifndef ZOOPACKEDVERTEX_H
#define ZOOPACKEDVERTEX_H

#include "ZooPacked.h"

/// packed vertex
class ZooPackedVertex : public TObject
{
    public:
	typedef ZooPackedPositionWithError::Vector Vector;
	typedef ZooPackedPositionWithError::Matrix Matrix;

	ZooPackedVertex() :
	    m_chi2(-1.f), m_ndof(-1)
        { }
	virtual ~ZooPackedVertex();

	template<typename P> ZooPackedVertex(
		const P& pos, const Matrix& cov, double chi2, short nDoF) :
	    m_chi2(chi2), m_ndof(nDoF), m_pos(pos, cov)
        { }

	template<typename P>
	void set(const P& pos, Matrix& cov,
		double chi2, short nDoF)
	{
	    m_pos.set(pos, cov);
	    m_ndof = nDoF;
	    m_chi2 = chi2;
	}

	/// return chi^2 of vertex fit
	double chi2() const { return m_chi2; }
	/// return number of degrees of freedom of vertex fit
	short nDoF() const { return m_ndof; }

	/// return vertex position
	const Vector pos() const { return m_pos; }
	/// return vertex position
	operator const ROOT::Math::XYZPoint() const { return m_pos; }
	/// return vertex covariance
	const Matrix cov() const { return m_pos; }
	/// return "raw" packed position with error
	ZooPackedPositionWithError& packedPosWithErr() { return m_pos; }

    private:
	Float_t m_chi2;
	Short_t m_ndof;
	ZooPackedPositionWithError m_pos;
	ClassDef(ZooPackedVertex, 1);
};

#endif // ZOOPACKEDVERTEX_H

// vim:tw=78:sw=4:ft=cpp
