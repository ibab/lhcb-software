#ifndef EVTTOVLLBASISCOEFFS_H_
#define EVTTOVLLBASISCOEFFS_H_

#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtMathematicaFn.hh"
#include "EvtGenModels/EvtBToVllConstraints.hh"

#include <memory>

class EvtBToVllBasisCoeffs{

public:
	
	enum BasisVectors {I1s, I1c, I1, I2s, I2c, I2, I3, I4, I5, I6s, I6c, I6, I7, I8, I9};
	
	EvtBToVllBasisCoeffs(const EvtBToVllConstraints& _constraints);
	void initParams(const double _q2, const bool isBbar);
	
	EvtComplex getI1s() const;
	EvtComplex getI1c() const;
	EvtComplex getI2s() const;
	EvtComplex getI2c() const;
	EvtComplex getI3() const;
	EvtComplex getI4() const;
	EvtComplex getI5() const;
	EvtComplex getI6s() const;
	EvtComplex getI6c() const;
	EvtComplex getI7() const;
	EvtComplex getI8() const;
	EvtComplex getI9() const;
	
	EvtComplex getVector(const BasisVectors& vec) const;
	

private:
	
	const EvtBToVllConstraints& constraints;

	EvtComplex A0L;
	EvtComplex A0R;
	
	EvtComplex APL;
	EvtComplex APR;
	
	EvtComplex ATL;
	EvtComplex ATR;
	
	EvtComplex AS;
	EvtComplex AT;
	
	double q2;
	double beta;
	
	static const double scaleFactor;
	
	EvtComplex ABConj(const EvtComplex& a, const EvtComplex& b) const{
		return a*Conjugate(b);
	}
};

class EvtBToVllObservable{
public:
	EvtBToVllObservable(const EvtBToVllConstraints& _constraints);
	void initParams(const double _q2);
	
	double getA(EvtBToVllBasisCoeffs::BasisVectors vec) const;
	double getS(EvtBToVllBasisCoeffs::BasisVectors vec) const;
	double getDGamma() const;
	
	
private:
	const std::auto_ptr<EvtBToVllBasisCoeffs> Bbar;
	const std::auto_ptr<EvtBToVllBasisCoeffs> B;
};

#endif /*EVTTOVLLBASISCOEFFS_H_*/
