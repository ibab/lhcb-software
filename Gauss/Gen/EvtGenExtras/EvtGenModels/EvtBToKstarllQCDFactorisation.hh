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
#include <string>
#include <vector>

typedef qcd::WilsonCoefficients<qcd::WilsonType> WilsonCoefficientsD;

struct EvtBToVllParameters{
public:
	std::auto_ptr<WilsonCoefficientsD> C_mb;
	std::auto_ptr<WilsonCoefficientsD> C_mb3;
};


class QCDFactorisation
{
public:
	
	//TODO: Replace form factors with something better
	QCDFactorisation(const qcd::IPhysicsModel& _model, const bdkszmm::PARAMETERIZATIONS _ffModel, bool _calcAFBZero = false);
	virtual ~QCDFactorisation();
	
	virtual  void getTnAmplitudes(const double q2, const double MB, const double mK, std::vector<EvtComplex>* tensors) const;
	void trasformAntiB(EvtParticle* parent, std::vector<EvtComplex>* tensors) const;
	void getAmp(EvtParticle* parent, EvtAmp& amp) const;
	
	void init();
	
	//physics related utility functions
	void mapAFBDistribution() const;
	double getAFB(const double q2) const;
	double findAFBZero() const;
	double getDstar(const double q2, const std::vector<EvtComplex>& tensors) const;
	double getGammaKstar(const double q2) const;
	
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
	
	enum TensorLabels {A = 0, B, C, D, E, F, G, H, S2, NUMBER_OF_TENSORS};//NUMBER_OF_TENSORS needs to be last
	const static std::string bquark;
	const static std::string antibquark;
	
	static EvtBToVllParameters* parameters;
	
};

#endif /*IQCDFACTO_H_*/
