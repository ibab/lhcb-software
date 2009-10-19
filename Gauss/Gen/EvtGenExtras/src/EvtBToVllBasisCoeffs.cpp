#include "EvtGenModels/EvtBToVllBasisCoeffs.hh"

#include <cmath>

const double EvtBToVllBasisCoeffs::scaleFactor = 1e12;

EvtBToVllBasisCoeffs::EvtBToVllBasisCoeffs(const EvtBToVllConstraints& _constraints): constraints(_constraints){
}

void EvtBToVllBasisCoeffs::initParams(const double _q2, const bool isBbar){
	
	q2 = _q2;
	beta = sqrt(1 - ((4*constants::mmu*constants::mmu)/q2));
	
	std::vector<EvtComplex> amps(EvtBToVllConstraints::NUMBER_OF_AMPS,scaleFactor);
	constraints.getSpinAmplitudes(q2, &amps, isBbar);
	
	A0L = amps.at(EvtBToVllConstraints::A0L);
	A0R = amps.at(EvtBToVllConstraints::A0R);
	
	APL = amps.at(EvtBToVllConstraints::APL);
	APR = amps.at(EvtBToVllConstraints::APR);
	
	ATL = amps.at(EvtBToVllConstraints::ATL);
	ATR = amps.at(EvtBToVllConstraints::ATR);
	
	AS = amps.at(EvtBToVllConstraints::AS);
	AT = amps.at(EvtBToVllConstraints::AT);
	
}

EvtComplex EvtBToVllBasisCoeffs::getI1s() const{
	return 0.25*(2 + beta*beta)*(ABConj(ATL,ATL) + ABConj(APL,APL) + ABConj(ATR,ATR) + ABConj(APR,APR)) +
		((4*constants::mmu*constants::mmu)/q2)*Re( ABConj(ATL,ATR) + ABConj(APL,APR) );
}

EvtComplex EvtBToVllBasisCoeffs::getI1c() const{
	return ABConj(A0L,A0L) + ABConj(A0R,A0R) +
	((4*constants::mmu*constants::mmu)/q2)*( ABConj(AT,AT) + 2*Re(ABConj(A0L,A0R)) ) +
	(beta*beta*ABConj(AS,AS));
}

EvtComplex EvtBToVllBasisCoeffs::getI2s() const{
	return 0.25*beta*beta*( ABConj(ATL,ATL) + ABConj(APL,APL) + ABConj(ATR,ATR) + ABConj(APR,APR) );
}

EvtComplex EvtBToVllBasisCoeffs::getI2c() const{
	return -beta*beta*( ABConj(A0L,A0L) + ABConj(A0R,A0R) );
}

EvtComplex EvtBToVllBasisCoeffs::getI3() const{
	return 0.5*beta*beta*( ABConj(ATL,ATL) - ABConj(APL,APL) + ABConj(ATR,ATR) - ABConj(APR,APR) );
}

EvtComplex EvtBToVllBasisCoeffs::getI4() const{
	return (1/sqrt(2))*beta*beta*( Re(ABConj(A0L,APL)) + Re(ABConj(A0R,APR)) );
}

EvtComplex EvtBToVllBasisCoeffs::getI5() const{
	return sqrt(2)*beta*( Re(ABConj(A0L,ATL)) - Re(ABConj(A0R,ATR)) - 
			(constants::mmu/sqrt(q2))*Re( ABConj(APL,AS) + ABConj(APR,AS)) );
}

EvtComplex EvtBToVllBasisCoeffs::getI6s() const{
	return 2*beta*( Re(ABConj(APL,ATL)) - Re(ABConj(APR,ATR)) );
}

EvtComplex EvtBToVllBasisCoeffs::getI6c() const{
	return 4*beta*(constants::mmu/sqrt(q2))*( Re( ABConj(A0L,AS) + ABConj(A0R,AS) ) );
}

EvtComplex EvtBToVllBasisCoeffs::getI7() const{
	return sqrt(2)*beta*( Im(ABConj(A0L,APL)) - Im(ABConj(A0R,APR)) + 
			(constants::mmu/sqrt(q2))*Im(ABConj(ATL,AS) + ABConj(ATR,AS)) );
}

EvtComplex EvtBToVllBasisCoeffs::getI8() const{
	return (1/sqrt(2))*beta*beta*( Im(ABConj(A0L,ATL)) + Im(ABConj(A0R,ATR)) );
}

EvtComplex EvtBToVllBasisCoeffs::getI9() const{
	return beta*beta*( Im(Conjugate(APL)*ATL) + Im(Conjugate(APR)*ATR) );
}

EvtComplex EvtBToVllBasisCoeffs::getVector(const BasisVectors& vec) const{
	
	switch (vec){
	case I1s:
		return getI1s();
	case I1c:
		return getI1c();
	case I1:
		return getI1s() + getI1c();
	case I2s:
		return getI2s();
	case I2c:
		return getI2c();
	case I2:
		return getI2s() + getI2c();
	case I3:
		return getI3();
	case I4:
		return getI4();
	case I5:
		return getI5();
	case I6s:
		return getI6s();
	case I6c:
		return getI6c();
	case I6:
		return getI6s() + getI6c();
	case I7:
		return getI7();
	case I8:
		return getI8();
	case I9:
		return getI9();
	default:
		return EvtComplex(0,0);
	}
	
	
}


EvtBToVllObservable::EvtBToVllObservable(const EvtBToVllConstraints& _constraints):
	Bbar(new EvtBToVllBasisCoeffs(_constraints)),
	B(new EvtBToVllBasisCoeffs(_constraints)){
}
void EvtBToVllObservable::initParams(const double _q2){
	Bbar->initParams(_q2, true);
	B->initParams(_q2, false);
}

double EvtBToVllObservable::getA(EvtBToVllBasisCoeffs::BasisVectors vec) const{
	return Re((Bbar->getVector(vec) - B->getVector(vec))/getDGamma());
}

double EvtBToVllObservable::getS(EvtBToVllBasisCoeffs::BasisVectors vec) const{
	return Re((Bbar->getVector(vec) + B->getVector(vec))/getDGamma());
}

double EvtBToVllObservable::getDGamma() const{
	const EvtComplex gamma = 0.75*(2*Bbar->getVector(EvtBToVllBasisCoeffs::I1s) + Bbar->getVector(EvtBToVllBasisCoeffs::I1c)) - 
		0.25*(2*Bbar->getVector(EvtBToVllBasisCoeffs::I2s) + Bbar->getVector(EvtBToVllBasisCoeffs::I2c));
	const EvtComplex gammaBar = 0.75*(2*B->getVector(EvtBToVllBasisCoeffs::I1s) + B->getVector(EvtBToVllBasisCoeffs::I1c)) - 
				0.25*(2*B->getVector(EvtBToVllBasisCoeffs::I2s) + B->getVector(EvtBToVllBasisCoeffs::I2c));
	return Re(gamma + gammaBar);
}


