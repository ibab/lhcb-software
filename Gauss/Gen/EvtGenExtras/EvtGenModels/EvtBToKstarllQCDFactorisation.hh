#ifndef IQCDFACTO_H_
#define IQCDFACTO_H_

#include "EvtGenBase/EvtAmp.hh"
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtIdSet.hh"
#include "EvtGenBase/EvtParticle.hh"

#include "EvtGenModels/EvtBToVllPhysicsModel.hh"
#include "EvtGenModels/EvtBToVllParameterisedFFCalc.hh"
#include "EvtGenModels/EvtBToVllWC.hh"

#include <cassert>
#include <memory>
#include <list>
#include <string>
#include <utility>
#include <vector>

typedef qcd::WilsonCoefficients<qcd::WilsonType> WilsonCoefficientsD;
typedef std::pair<const EvtComplex, const EvtComplex> ComplexPair;
typedef std::list<ComplexPair> ComplexPairList;

class EvtBToVllParameters{

public:
	
	EvtBToVllParameters(bool _includeRHC,
			qcd::WCPtr _C_mb, qcd::WCPtr _C_mb3,
			qcd::WCPtr _CR_mb, qcd::WCPtr _CR_mb3);
	
	//must be called to get correct WC sets
	void setParentID(const EvtId parentID);
	void setBbar(const bool _isBbar){
		isBbar = _isBbar;
	}
	bool getisBbar() const{
		return isBbar;
	}
	void resetParentID(){
		setBbar(true);
	}
	
	qcd::WCPtrNaked getC_mb() const{
		return isBbar ? C_mb.get() : C_mb_conj.get();
	}
	qcd::WCPtrNaked getCR_mb() const{
		return isBbar ? CR_mb.get() : CR_mb_conj.get();
	}
	qcd::WCPtrNaked getC_mb3() const{
		return isBbar ? C_mb3.get() : C_mb3_conj.get();
	}
	qcd::WCPtrNaked getCR_mb3() const{
		return isBbar ? CR_mb3.get() : CR_mb3_conj.get();
	}
	std::string flavourString() const;

	const bool includeRHC;
	
private:
	
	//Bbar
	const qcd::WCPtr C_mb;
	const qcd::WCPtr C_mb3;
	const qcd::WCPtr CR_mb;
	const qcd::WCPtr CR_mb3;
	//B
	const qcd::WCPtr C_mb_conj;
	const qcd::WCPtr C_mb3_conj;
	const qcd::WCPtr CR_mb_conj;
	const qcd::WCPtr CR_mb3_conj;
	
	bool isBbar;
};


class QCDFactorisation
{
public:
	
	QCDFactorisation(const qcd::IPhysicsModel& _model, const bdkszmm::PARAMETERIZATIONS _ffModel, bool _calcAFBZero = false);
	virtual ~QCDFactorisation();
	
	virtual  void getTnAmplitudes(const double q2, const double MB, const double mK, std::vector<EvtComplex>* tensors) const;
	void trasformAntiB(EvtParticle* parent, std::vector<EvtComplex>* tensors) const;
	void getAmp(EvtParticle* parent, EvtAmp& amp) const;
	
	void init();
	
	//physics related utility functions
	double getAFB(const double q2) const;
	double findAFBZero() const;
	double getDstar(const double q2, const std::vector<EvtComplex>& tensors) const;
	double getGammaKstar(const double q2) const;
	
	enum TensorLabels {A = 0, B, C, D, E, F, G, H, S2, NUMBER_OF_TENSORS};//NUMBER_OF_TENSORS needs to be last
	const static std::string bquark;
	const static std::string antibquark;
	
private:
	
	const qcd::IPhysicsModel& model;
	const std::auto_ptr<bdkszmm::FFCalc> ffModel;
	
	const bool calcAFBZero;

protected:
	
	virtual const qcd::IPhysicsModel& getModel(){
		return model;
	}
	
	//can update form factors by overriding this function
	virtual void getFormFactors(const double q2,
			double* const A0, double* const A1, double* const A2, double* const V,
			double* const T1, double* const T2, double* const T3,
			double* const xiL, double* const xiT) const;
	
	
	static EvtBToVllParameters* parameters;
	
};

#endif /*IQCDFACTO_H_*/
