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

class EvtBToVllConstraints;

class EvtBToVllParameters{

	friend class EvtBToVllConstraints;
	
public:
	
	EvtBToVllParameters(
			qcd::WCPtr _C_mb, qcd::WCPtr _C_mb3,
			qcd::WCPtr _CR_mb, qcd::WCPtr _CR_mb3,
			qcd::WCPtr _CNP_mw, qcd::WCPtr _CR_mw);
	
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
	qcd::WCPtrNaked getCNP_mw() const{
			return CNP_mw.get();
	}
	qcd::WCPtrNaked getCR_mw() const{
			return CR_mw.get();
	}
	
	
	std::string flavourString() const;
	
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
	//
	const qcd::WCPtr CNP_mw;
	const qcd::WCPtr CR_mw;
	
	bool isBbar;
};


class QCDFactorisation
{
	
	friend class EvtBToVllConstraints;
	
public:
	
	QCDFactorisation(const qcd::IPhysicsModel& _model, const bdkszmm::PARAMETERIZATIONS _ffModel, bool _calcConstraints = false);
	virtual ~QCDFactorisation();
	
	virtual  void getTnAmplitudes(const double q2, const double MB, const double mK, std::vector<EvtComplex>* tensors) const;
	void trasformAntiB(EvtParticle* parent, std::vector<EvtComplex>* tensors) const;
	void getAmp(EvtParticle* parent, EvtAmp& amp) const;
	
	void init();
	
	enum TensorLabels {A = 0, B, C, D, E, F, G, H, S2, NUMBER_OF_TENSORS};//NUMBER_OF_TENSORS needs to be last
	const static std::string bquark;
	const static std::string antibquark;
	
	class inner{
	public:
		static EvtComplex getC0(const double& s);
		static EvtComplex B10(const double& s, const double& mq, const double& reg);
		static std::auto_ptr<ComplexPairList> getKFactor(const double re[4][2], const double im[4][2]);
		static EvtComplex f_x_y(const ComplexPairList& coeffs, const double& s2, const double& Ls);
		static EvtComplex get_t(const int& a, const std::pair<double,double> xi);
		static EvtComplex get_F_1_9(const double Lc, const double Lm, const double Ls, const double mc1, const double s2);
		static EvtComplex get_F_2_9(const double Lc, const double Lm, const double Ls, const double mc1, const double s2);
		static EvtComplex get_F_1_7(const double Lc, const double Lm, const double Ls, const double mc1, const double s2);
		static EvtComplex get_F_2_7(const double Lc, const double Lm, const double Ls, const double mc1, const double s2);
	};
	
private:
	
	const qcd::IPhysicsModel& model;
	const std::auto_ptr<bdkszmm::FFCalc> ffModel;
	
	const bool calcConstraints;

protected:
	
	virtual const qcd::IPhysicsModel& getModel() const{
		return model;
	}
	
	//can update form factors by overriding this function
	virtual void getFormFactors(const double q2,const double mB, const double mKstar,
			double* const A0, double* const A1, double* const A2, double* const V,
			double* const T1, double* const T2, double* const T3,
			double* const xiL, double* const xiT) const;
	
	
	static EvtBToVllParameters* parameters;
	
	static const double re_f_1_7[4][2];
	static const double re_f_1_9[4][2];
	static const double re_f_2_7[4][2];
	static const double re_f_2_9[4][2];
	
	static const double im_f_1_7[4][2];
	static const double im_f_1_9[4][2];
	static const double im_f_2_7[4][2];
	static const double im_f_2_9[4][2];

};


#endif /*IQCDFACTO_H_*/
