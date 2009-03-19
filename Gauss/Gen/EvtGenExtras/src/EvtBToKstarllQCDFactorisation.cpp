//from EvtGen
#include "EvtGenBase/EvtDiracSpinor.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtMathematicaFn.hh"
#include "EvtGenBase/EvtIdSet.hh"
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensor4C.hh"

#include "EvtGenModels/EvtBToVllConstants.hh"
#include "EvtGenModels/EvtBToVllEvolveWC.hh"
#include "EvtGenModels/EvtBToVllIntegrals.hh"
#include "EvtGenModels/EvtBToVllParameterisedFFCalc.hh"
#include "EvtGenModels/EvtBToKstarllQCDFactorisation.hh"
#include "EvtGenModels/EvtBToVllQCDUtils.hh"

//from gsl
#include "gsl/gsl_math.h"
#include "gsl/gsl_sf_gegenbauer.h"
#include "gsl/gsl_sf_dilog.h"
#include "gsl/gsl_sf_result.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <utility>

using qcd::alpha_s;

const std::string QCDFactorisation::bquark("anti-B0");//The \bar{B}_d contains the b!
const std::string QCDFactorisation::antibquark("B0");

EvtBToVllParameters* QCDFactorisation::parameters = 0;

EvtBToVllParameters::EvtBToVllParameters(bool _includeRHC,
		qcd::WCPtr _C_mb, qcd::WCPtr _C_mb3,
		qcd::WCPtr _CR_mb, qcd::WCPtr _CR_mb3):
		includeRHC(_includeRHC),
		C_mb(_C_mb),C_mb3(_C_mb3),
		CR_mb(_CR_mb),CR_mb3(_CR_mb3),
		C_mb_conj(qcd::WilsonCoefficients<qcd::WilsonType>::conjugate(*C_mb)),
		C_mb3_conj(qcd::WilsonCoefficients<qcd::WilsonType>::conjugate(*C_mb3)),
		CR_mb_conj(qcd::WilsonCoefficients<qcd::WilsonType>::conjugate(*CR_mb)),
		CR_mb3_conj(qcd::WilsonCoefficients<qcd::WilsonType>::conjugate(*CR_mb3)),
		isBbar(true){
	std::cout << "Constructed the parameters struct" << std::endl;
}

void EvtBToVllParameters::setParentID(const EvtId parentID){
	EvtIdSet b(QCDFactorisation::bquark);//i.e. the Bbar
	isBbar = b.contains(parentID);
}

std::string EvtBToVllParameters::flavourString() const{
	return isBbar ? QCDFactorisation::bquark : QCDFactorisation::antibquark; 
}


QCDFactorisation::QCDFactorisation(const qcd::IPhysicsModel& _model, const bdkszmm::PARAMETERIZATIONS _ffModel, bool _calcAFBZero):
	model(_model),ffModel(bdkszmm::getFFModel(_ffModel)),calcAFBZero(_calcAFBZero)
{
	if(!parameters){
		init();
	}
}

void QCDFactorisation::init(){
	
	report(INFO,"EvtGen") << " Form-factor model is \"" << ffModel->getName() << "\"." << std::endl;

	//find the WCs at mu = mb
	qcd::WCPtr  C_mw = model.getLeftWilsonCoefficientsMW();
	qcd::WCPtr  CR_mw = model.getRightWilsonCoefficientsMW();

	qcd::EvtBToVllEvolveWC10D evolveMb(*C_mw,*CR_mw);
	std::auto_ptr<qcd::WilsonPair> _mb(evolveMb(constants::mu_mb));
	std::auto_ptr<qcd::WilsonPair> _mb3(evolveMb(constants::mu_h));

	parameters = new EvtBToVllParameters(model.hasRightHandedCurrents(),
			qcd::WCPtr(_mb->first),qcd::WCPtr(_mb3->first),
			qcd::WCPtr(_mb->second),qcd::WCPtr(_mb3->second));
	
	assert(C_mw->getOperatorBasis() == parameters->getC_mb()->getOperatorBasis());
	assert(C_mw->getOperatorBasis() == parameters->getC_mb3()->getOperatorBasis());
	
	DEBUGPRINT("C_mw: ", (*C_mw));//V
	DEBUGPRINT("CR_mw: ", (*CR_mw));//V

	const bool saveFlavour = parameters->getisBbar();
	parameters->setBbar(true);
	DEBUGPRINT("Flavour: ", parameters->flavourString());
	DEBUGPRINT("C_mb: ", *(parameters->getC_mb()));//V
	DEBUGPRINT("C_mb3: ", *(parameters->getC_mb3()));//V
	DEBUGPRINT("CR_mb: ", *(parameters->getCR_mb()));//V
	DEBUGPRINT("CR_mb3: ", *(parameters->getCR_mb3()));//V
	
	parameters->setBbar(false);
	DEBUGPRINT("Flavour: ", parameters->flavourString());
	DEBUGPRINT("C_mb: ", *(parameters->getC_mb()));//V
	DEBUGPRINT("C_mb3: ", *(parameters->getC_mb3()));//V
	DEBUGPRINT("CR_mb: ", *(parameters->getCR_mb()));//V
	DEBUGPRINT("CR_mb3: ", *(parameters->getCR_mb3()));//V
	parameters->setBbar(saveFlavour);
	
	report(NOTICE,"EvtGen") << "Using physics model: " << model.getModelName() << std::endl;
	report(NOTICE,"EvtGen") << "Meson: " << parameters->flavourString() << std::endl;
	report(NOTICE,"EvtGen") << "Left-handed Wilson coefficients are: " << std::endl;
	report(NOTICE,"EvtGen") << "\t(m_W): " << (*C_mw) <<std::endl;
	report(NOTICE,"EvtGen") << "\t(m_b): " << *(parameters->getC_mb()) <<std::endl;
	report(NOTICE,"EvtGen") << "\t(m_h): " << *(parameters->getC_mb3()) <<std::endl;
	
	report(NOTICE,"EvtGen") << "Right-handed Wilson coefficients are: " << std::endl;
	report(NOTICE,"EvtGen") << "\t(m_W): " << (*CR_mw) <<std::endl;
	report(NOTICE,"EvtGen") << "\t(m_b): " << *(parameters->getCR_mb()) <<std::endl;
	report(NOTICE,"EvtGen") << "\t(m_h): " << *(parameters->getCR_mb3()) <<std::endl;
	
	if(calcAFBZero){
		double afbZero = findAFBZero();
		report(NOTICE,"EvtGen") << "AFB Zero Crossing point is: " << afbZero << " (GeV^2)" <<std::endl;
	}
	
#if 1
	std::cout << "C(mb)" << std::endl;
	for(unsigned int i = 1; i <= parameters->getC_mb()->getOperatorBasis(); i++){
		const double re = real((*(parameters->getC_mb()))(i));
		const double im = imag((*(parameters->getC_mb()))(i));
		const char op = (im > 0) ? '+' : '-';
		if( (im<1e-5) && (im>-1e-5) ){//real
			std::cout << std::showpoint << std::fixed << std::setprecision(3) << re << " & ";
		}else{//im
			std::cout << std::showpoint << std::fixed << std::setprecision(3) << "$"<<re<<op<<fabs(im)<<"i$"<< " & ";
		}
	}
	std::cout << std::endl;
	std::cout << "C\'(mb)" << std::endl;
	for(unsigned int i = 1; i <= parameters->getCR_mb()->getOperatorBasis(); i++){
		const double re = real((*(parameters->getCR_mb()))(i));
		const double im = imag((*(parameters->getCR_mb()))(i));
		const char op = (im > 0) ? '+' : '-';
		if( (im<1e-5) && (im>-1e-5) ){//real
			std::cout << std::showpoint << std::fixed << std::setprecision(3) << re << " & ";
		}else{//im
			std::cout << std::showpoint << std::fixed << std::setprecision(3) << "$"<<re<<op<<fabs(im)<<"i$"<< " & ";
		}
	}
	std::cout << std::endl;
	
#endif	
}

QCDFactorisation::~QCDFactorisation()
{
}

void QCDFactorisation::trasformAntiB(EvtParticle* parent, std::vector<EvtComplex>* tensors) const{
	/** Transform tensors for the \bar{b} quark case */
		
	const EvtId parentID = parent->getId();
	
	EvtIdSet antib(antibquark);//tmp non-const copy
	
	if(antib.contains(parentID)){
		//we swap terms with a totally anti-symettric tensor
		tensors->at(A) = -tensors->at(A);
		tensors->at(E) = -tensors->at(E);
	}
}

void QCDFactorisation::getFormFactors(const double q2,
			double* const A0, double* const A1, double* const A2, double* const V,
			double* const T1, double* const T2, double* const T3,
			double* const xi1, double* const xi2) const{
	
	//calculate the form factors
	*V  = ffModel->getV(q2);
	*A0 = ffModel->getA0(q2);
	*A1 = ffModel->getA1(q2);
	*A2 = ffModel->getA2(q2);

	*T1 = ffModel->getT1(q2);
	*T2 = ffModel->getT2(q2);
	*T3 = ffModel->getT3(q2);
	
	*xi1 = ffModel->getXiT(q2);
	*xi2 = ffModel->getXiL(q2);
}

void QCDFactorisation::getAmp(EvtParticle* parent, EvtAmp& amp) const{
	
	EvtVector4R q = parent->getDaug(1)->getP4() + parent->getDaug(2)->getP4();
	const double q2 = (q.mass2());
	const double MB = constants::mB;
	const double mK = constants::mKstar;
	//const double q2 = 3.9;
	const double mKhat = mK/MB;

	//set the parent ID so that we get the right WC
	parameters->setParentID(parent->getId());
	//parameters->setBbar(false);
	
	DEBUGPRINT("Flavour: ", parameters->flavourString());
	DEBUGPRINT("C_mb: ", *(parameters->getC_mb()));
	DEBUGPRINT("C_mb3: ", *(parameters->getC_mb3()));
	DEBUGPRINT("CR_mb: ", *(parameters->getCR_mb()));
	DEBUGPRINT("CR_mb3: ", *(parameters->getCR_mb3()));
	DEBUGPRINT("MB: ", MB);
	
	//get the amplitude tensors
	std::vector<EvtComplex> tensors(NUMBER_OF_TENSORS);
	getTnAmplitudes(q2,MB,mK, &tensors);
	//transform to the b-bar if needed
	trasformAntiB(parent, &tensors);
	
	const EvtComplex tensA = tensors.at(A);
	const EvtComplex tensB = tensors.at(B);
	const EvtComplex tensC = tensors.at(C);
	const EvtComplex tensE = tensors.at(E);
	const EvtComplex tensF = tensors.at(F);
	const EvtComplex tensG = tensors.at(G);
	const EvtComplex tensH =tensors.at(H);
	const EvtComplex tensS2 =tensors.at(S2);
	
	const double parentMass = parent->mass();
 	const EvtVector4R p4b(parent->mass(),0.0,0.0,0.0);
	const EvtVector4R p4meson = parent->getDaug(0)->getP4();
	const EvtVector4R pbhat = p4b/parentMass;
	const EvtVector4R qhat = q/parentMass;
	const EvtVector4R pkstarhat = p4meson/parentMass;
	const EvtVector4R phat = pbhat+pkstarhat;
	const EvtComplex I(0.0,1.0);

	const int charge1 = EvtPDL::chg3(parent->getDaug(1)->getId());
	const int charge2 = EvtPDL::chg3(parent->getDaug(2)->getId());
	  
	const EvtParticle* lepPlus = (charge1 > charge2) ? parent->getDaug(1) : parent->getDaug(2);
	const EvtParticle* lepMinus = (charge1 < charge2) ? parent->getDaug(1) : parent->getDaug(2);

	//eqn 4.4
	const EvtTensor4C T1 = tensA*dual(directProd(pbhat, pkstarhat)) - tensB*I*EvtTensor4C::g() + 
		tensC*I*directProd(pbhat, phat);

	//eqn 4.5
	const EvtTensor4C T2 = tensE*dual(directProd(pbhat, pkstarhat)) - tensF*I*EvtTensor4C::g() +
				tensG*I*directProd(pbhat, phat) + tensH*I*directProd(pbhat, qhat);
	
	//now calculate the V, A, and S lepton currents
	EvtVector4C l11,l12;
	EvtVector4C l21,l22;
	
	EvtVector4C a11,a12;
	EvtVector4C a21,a22;
	
	EvtComplex s11,s12;
	EvtComplex s21,s22;

	const EvtId parentID = parent->getId();
	EvtIdSet antib(antibquark);
	EvtIdSet b(bquark);
	if(b.contains(parentID)){
	
		l11 = EvtLeptonVCurrent(lepPlus->spParent(0), lepMinus->spParent(0));
		l21 = EvtLeptonVCurrent(lepPlus->spParent(1), lepMinus->spParent(0));
		l12 = EvtLeptonVCurrent(lepPlus->spParent(0), lepMinus->spParent(1));
		l22 = EvtLeptonVCurrent(lepPlus->spParent(1), lepMinus->spParent(1));
	
		a11 = EvtLeptonACurrent(lepPlus->spParent(0), lepMinus->spParent(0));
		a21 = EvtLeptonACurrent(lepPlus->spParent(1), lepMinus->spParent(0));
		a12 = EvtLeptonACurrent(lepPlus->spParent(0), lepMinus->spParent(1));
		a22 = EvtLeptonACurrent(lepPlus->spParent(1), lepMinus->spParent(1));
		
		s11 = EvtLeptonSCurrent(lepPlus->spParent(0), lepMinus->spParent(0));
		s21 = EvtLeptonSCurrent(lepPlus->spParent(1), lepMinus->spParent(0));
		s12 = EvtLeptonSCurrent(lepPlus->spParent(0), lepMinus->spParent(1));
		s22 = EvtLeptonSCurrent(lepPlus->spParent(1), lepMinus->spParent(1));

	}else if (antib.contains(parentID)){

		l11 = EvtLeptonVCurrent(lepPlus->spParent(1), lepMinus->spParent(1));
		l21 = EvtLeptonVCurrent(lepPlus->spParent(0), lepMinus->spParent(1));
		l12 = EvtLeptonVCurrent(lepPlus->spParent(1), lepMinus->spParent(0));
		l22 = EvtLeptonVCurrent(lepPlus->spParent(0), lepMinus->spParent(0));

		a11 = EvtLeptonACurrent(lepPlus->spParent(1), lepMinus->spParent(1));
		a21 = EvtLeptonACurrent(lepPlus->spParent(0), lepMinus->spParent(1));
		a12 = EvtLeptonACurrent(lepPlus->spParent(1), lepMinus->spParent(0));
		a22 = EvtLeptonACurrent(lepPlus->spParent(0), lepMinus->spParent(0));
		
		s11 = EvtLeptonSCurrent(lepPlus->spParent(1), lepMinus->spParent(1));
		s21 = EvtLeptonSCurrent(lepPlus->spParent(0), lepMinus->spParent(1));
		s12 = EvtLeptonSCurrent(lepPlus->spParent(1), lepMinus->spParent(0));
		s22 = EvtLeptonSCurrent(lepPlus->spParent(0), lepMinus->spParent(0));

	}else{
		report(ERROR,"EvtGen") << __FILE__ << ": Unknown parentID" << std::endl;
	}
	
	for(unsigned int i = 0; i < 3; i++){
		
		//K* polarization vector in B rest frame - \epsilon_\mu^*
		EvtVector4C eps = parent->getDaug(0)->epsParent(i).conj();

	    EvtVector4C E1 = T1.cont1(eps);
	    EvtVector4C E2 = T2.cont1(eps);
	    
	    //eqn 3.7 of hep-ph/000426
	    EvtComplex  S = I*2*mKhat*(eps*pbhat)*tensS2;

	    amp.vertex(i,0,0, l11*E1+a11*E2+S*s11);
	    amp.vertex(i,0,1, l12*E1+a12*E2+S*s12);
	    amp.vertex(i,1,0, l21*E1+a21*E2+S*s21);
	    amp.vertex(i,1,1, l22*E1+a22*E2+S*s22);
	 }
	//return parent ID to default state
	parameters->resetParentID();
}

double QCDFactorisation::getAFB(const double q2) const{
	
	const double MB = constants::mB;
	const double mK = constants::mKstar;
	const double mk = mK/MB;
	const double mlhat = constants::mmu/MB;

	const double shat = q2/(MB*MB);
	//get the amplitude tensors
	std::vector<EvtComplex> tensors(NUMBER_OF_TENSORS);
	getTnAmplitudes(q2,MB,mK, &tensors);
		
	const EvtComplex tensA = tensors.at(A);
	const EvtComplex tensB = tensors.at(B);
	const EvtComplex tensC = tensors.at(C);
	const EvtComplex tensD = tensors.at(D);
	const EvtComplex tensE = tensors.at(E);
	const EvtComplex tensF = tensors.at(F);
	const EvtComplex tensG = tensors.at(G);
	const EvtComplex tensH =tensors.at(H);
	const EvtComplex tensS2 =tensors.at(S2);
	
	const double lambda = 1 + (mk*mk*mk*mk) + (shat*shat) - (2*shat)*(1 + shat);
	const double uhat = sqrt(lambda*(1 - ((4*mlhat*mlhat)/shat)));
	
	const double dKstar = getDstar(q2,tensors);
	
	const double AFB = (uhat/dKstar)*( shat*(real(tensB*conj(tensE)) +  real(tensA*conj(tensF))) +
										(mlhat/mk)*( real(tensS2*conj(tensB))*(1 - shat - mk*mk) - 
														real(tensS2*conj(tensC))*lambda) );
	return AFB;
}

double QCDFactorisation::findAFBZero() const{
	
	double q2min = 0.5;
	double q2max = 8.0;
	
	double afb1 = getAFB(q2min);
	double afb2 = 0.0;
	double afb3 = getAFB(q2max);
	
	double q2diff = fabs(q2min - q2max);
	unsigned int chops = 0;
	while(q2diff > 0.01){
	
		q2diff = fabs(q2min - q2max);
		double q2middle = q2min + (0.5*q2diff);
		
		afb1 = getAFB(q2min);
		afb2 = getAFB(q2middle);
		afb3 = getAFB(q2max);
		
		if(afb1*afb3 > 0){//No AFB zero at all
			report(WARNING,"EvtGen") << __FILE__ << ": No AFB Zero Found in range " << q2min << " - " << q2max << std::endl;
			if(chops == 0){
				break;
			}
		}
		
		if(afb1*afb2 < 0){//afb zero in lower half
			q2max = q2middle;
		}else{//afb in upper half
			q2min = q2middle;
		}
		
		if(chops > 50){
			report(WARNING,"EvtGen") << __FILE__ << ": Zero not found to required accuracy." << std::endl;
			break;
		}
		
	}
	double afbZero = 0.5*(fabs(q2min)+fabs(q2max));
	return afbZero;
	
}

double QCDFactorisation::getDstar(const double q2, const std::vector<EvtComplex>& tensors) const{
	
	const EvtComplex tensA = tensors.at(A);
	const EvtComplex tensB = tensors.at(B);
	const EvtComplex tensC = tensors.at(C);
	const EvtComplex tensD = tensors.at(D);
	const EvtComplex tensE = tensors.at(E);
	const EvtComplex tensF = tensors.at(F);
	const EvtComplex tensG = tensors.at(G);
	const EvtComplex tensH =tensors.at(H);
	const EvtComplex tensS2 =tensors.at(S2);
	
	const double MB = constants::mB;
	const double mK = constants::mKstar;
	const double mk = mK/MB;
	const double mlhat = constants::mmu/MB;

	const double shat = q2/(MB*MB);
	
	const double lambda = 1 + (mk*mk*mk*mk) + (shat*shat) - (2*shat)*(1 + shat);
	const double uhat = sqrt(lambda*(1 - ((4*mlhat*mlhat)/shat)));

	const double mk2 = mk*mk;
	const double ml2 = mlhat*mlhat;
	const double u2 = uhat*uhat;
	
	//eqn (3.57) of Phys Rev D62 094023
	EvtComplex result = ((conj(tensA)*tensA)/3.0)*shat*lambda*( 1 + 2*ml2/shat) + 
		(conj(tensE)*tensE)*shat*uhat/3.0 +
		(conj(tensS2)*tensS2)*(shat - 4*ml2)*lambda +
		(1/(1*mk2))*(  (conj(tensB)*tensB)*( lambda - (u2/3.0) + 8*mk2*(shat + 2*ml2) )  +
				(conj(tensF)*tensF)*(lambda - u2/3.0 + 8*mk2*(shat - 4*ml2)) ) +
		(lambda/(4*mk2))*( (conj(tensC)*tensC)*(lambda - u2/3.0) + 
				(conj(tensG)*tensG)*(lambda - u2/3.0 + 4*ml2*(2 + 2*mk2 -shat)) ) - 
		(1/(2*mk2))*( real(tensB*conj(tensC))*(lambda - u2/3.0)*(1 - mk2 - shat) + 
				real(tensF*conj(tensG))*((lambda - u2/3)*(1 - mk2 - shat) + 4*ml2*lambda) ) -
		2*(ml2/mk2)*lambda*( real(tensF*conj(tensH)) - real(tensG*conj(tensH)) )*(1 - mk2) + 
		(ml2/mk2)*shat*(conj(tensH)*tensH);
	
	return real(result);
	
}
double QCDFactorisation::getGammaKstar(const double q2) const{
	//dGamma^{K*}/ds
	
	const double MB = constants::mB;
	const double mK = constants::mKstar;
	const double mk = mK/MB;
	const double mlhat = constants::mmu/MB;

	const double shat = q2/(MB*MB);
	const double lambda = 1 + (mk*mk*mk*mk) + (shat*shat) - (2*shat)*(1 + shat);
	const double uhat = sqrt(lambda*(1 - ((4*mlhat*mlhat)/shat)));
	
	//get the amplitude tensors
	std::vector<EvtComplex> tensors(NUMBER_OF_TENSORS);
	getTnAmplitudes(q2,MB,mK, &tensors);
	
	//eqn (3.56) with arbitratry normalisation
	const double dStar = getDstar(q2,tensors);
	return uhat*dStar;
}

void QCDFactorisation::getTnAmplitudes(const double q2, const double MB, const double mK, std::vector<EvtComplex>* tensors) const{
	
	/**
	 * Code to calculate the tensors. The factorisation calculations
	 * follow Beneke, but the final structure is that of Ali et al
	 * (PRD 61 074024). The calculations are to be done for the b
	 * quark. These will be transformed for the b-bar quark.
	 * 
	 * @param std::vector<EvtComplex> tensors - An empty vector to be filled by the routine.
	 * 
	 * There is an enum to give the correct indexes. These should be:
	 * 
	 * index:	0 1 2 3 4 5 6 7 8	
	 * tensor:	A B C D E F G H S2
	 * 
	 * These tensors will be used in the eqns 2.2, 4.4 and 4.5 of Ali et al.
	 * 
	 */

	//energy of final state meson
	const double e = ((MB*MB) - q2)/(2*MB);
	DEBUGPRINT("e: ", e);//V

	//calculate the form factors
	double ffA0, ffA1, ffA2, ffV;
	double ffT1, ffT2, ffT3;
	double xi1, xi2;
	
	getFormFactors(q2,
			&ffA0,&ffA1,&ffA2,&ffV,
			&ffT1,&ffT2,&ffT3,
			&xi1,&xi2);
	
	DEBUGPRINT("q2: ", q2);//V
	DEBUGPRINT("ffA0: ", ffA0);//V
	DEBUGPRINT("ffA1: ", ffA1);//V
	DEBUGPRINT("ffA2: ", ffA2);//V
	DEBUGPRINT("ffV: ", ffV);//V

	DEBUGPRINT("ffT1: ", ffT1);//V
	DEBUGPRINT("ffT2: ", ffT2);//V
	DEBUGPRINT("ffT3: ", ffT3);//V

	DEBUGPRINT("xi[1]: ", xi1);//V
	DEBUGPRINT("xi[2]: ", xi2);//V	

		
	class inner{
	public:
		
		static EvtComplex getC0(const double& s){
			//Eqn 84 - Analytic solution only valid in range 0 <= s <= 2.
			assert( (s >= 0) && (s <= 2) );
			
			/*
			 * the following is basically exported from Mathematica, where the recommendation
			 * in the paper to express in terms of dilogarithms has been implemented.
			 */
			
			//calculate the 4 PolyLog[2,x]
			gsl_sf_result re;
			gsl_sf_result im;

			EvtComplex tmp = -1 + s;
			gsl_sf_complex_dilog_e(abs(tmp),arg(tmp),&re,&im);
			EvtComplex diLog0(re.val,im.val);
			
			tmp = ((sqrt(4 - s) - EvtComplex(0,1)*sqrt(s))*(-1 + s))/(-sqrt(4 - s) + EvtComplex(0,3)*sqrt(s) + sqrt(4 - s)*s - EvtComplex(0,1)*pow(s,1.5));
			gsl_sf_complex_dilog_e(abs(tmp),arg(tmp),&re,&im);
			EvtComplex diLog1(re.val,im.val);
			
			tmp = ((sqrt(4 - s) + EvtComplex(0,1)*sqrt(s))*(-1 + s))/(-sqrt(4 - s) + EvtComplex(0,3)*sqrt(s) + sqrt(4 - s)*s - EvtComplex(0,1)*pow(s,1.5));
			gsl_sf_complex_dilog_e(abs(tmp),arg(tmp),&re,&im);
			EvtComplex diLog2(re.val,im.val);
			
			tmp = ((sqrt(4 - s) - EvtComplex(0,1)*sqrt(s))*(-1 + s))/(-sqrt(4 - s) - EvtComplex(0,3)*sqrt(s) + sqrt(4 - s)*s + EvtComplex(0,1)*pow(s,1.5));
			gsl_sf_complex_dilog_e(abs(tmp),arg(tmp),&re,&im);
			EvtComplex diLog3(re.val,im.val);
			
			tmp = ((sqrt(4 - s) + EvtComplex(0,1)*sqrt(s))*(-1 + s))/(-sqrt(4 - s) - EvtComplex(0,3)*sqrt(s) + sqrt(4 - s)*s + EvtComplex(0,1)*pow(s,1.5));
			gsl_sf_complex_dilog_e(abs(tmp),arg(tmp),&re,&im);
			EvtComplex diLog4(re.val,im.val);

			using constants::Pi;
			const EvtComplex I(0,1);
			
			const double s1 = s;
			return chop(((-log(-1 + I*sqrt(-1 + 4/s1)) + log(1 + I*sqrt(-1 + 4/s1)))*
				      (EvtComplex(0,-2)*Pi + log(4) + log(s1) + 2*log(1/(EvtComplex(0,-1)*sqrt(4 - s1) + 3*sqrt(s1) + I*sqrt(4 - s1)*s1 - pow(s1,1.5))))
				       + log(-1 - I*sqrt(-1 + 4/s1))*(EvtComplex(0,2)*Pi - log(4) - log(s1) - 
				        2*log(EvtComplex(0,-1)/(sqrt(4 - s1) + I*sqrt(s1)*(-3 + s1 + I*sqrt(-((-4 + s1)*s1)))))) + 
				     log(1 - I*sqrt(-1 + 4/s1))*(EvtComplex(0,-2)*Pi + log(4) + log(s1) + 
				        2*log(EvtComplex(0,-1)/(sqrt(4 - s1) + I*sqrt(s1)*(-3 + s1 + I*sqrt(-((-4 + s1)*s1)))))) - 4*diLog0 - 
				     2*diLog1 + 2*diLog2 + 2*diLog3 - 2*diLog4)/(2.*(-1 + s1)));
		};
		
		static EvtComplex B10(const double& s, const double& mq, const double& reg){
			EvtComplex regmb2(mq*mq,reg);
			//Eqn 29
			const EvtComplex coeff = sqrt(((4*regmb2)/s) - 1);
			return -2*coeff*atan(1/coeff); 
		};
		
		//makes a list of Ks from the static data
		static std::auto_ptr<ComplexPairList> getKFactor(const double re[4][2], const double im[4][2]){
			ComplexPairList* result = new ComplexPairList();
			for(unsigned int i = 0; i < 4; ++i){
				result->push_back(std::make_pair(EvtComplex(re[i][0],im[i][0]),EvtComplex(re[i][1],im[i][1])));
			}
			return std::auto_ptr<ComplexPairList>(result);
		};
		
		static EvtComplex f_x_y(const ComplexPairList& coeffs, const double& s2, const double& Ls){
			assert(coeffs.size() == 4);//looking for k factors
			EvtComplex sum = 0;
			unsigned int count = 1;
			for(ComplexPairList::const_iterator it = coeffs.begin(); it != coeffs.end(); ++it){
				const double s2_fac = pow(s2,count-1);
				sum += (s2_fac*it->first) + (Ls*s2_fac*it->second);
				count++;
			}
			return sum;
		};
		
		
		static EvtComplex get_t(const int& a, const std::pair<double,double> xi){
			const double xi_data[2] = {xi.first, xi.second};
			const int a_index = a - 1;
			assert( (a_index < 2) && (a_index <= 0));
			return xi_data[a_index];
		}
		
	};
	
	static const double re_f_1_7[4][2] = {{-0.68192, 0}, {-0.23935, 0.0027424}, {-0.0018555, 0.022864}, {0.28248, 0.029027}};
	static const double re_f_1_9[4][2] = {{-11.973, -0.081271}, {-28.432, -0.040243}, {-57.114, -0.035191}, {-128.8, -0.017587}};
	static const double re_f_2_7[4][2] = {{4.095, 0}, {1.4361, -0.016454}, {0.011133, -0.13718}, {-1.6949, -0.17416}};
	static const double re_f_2_9[4][2] = {{6.6338, 0.48763}, {3.3585, 0.24146}, {-1.1906, 0.21115}, {-17.12, 0.10552}};
	
	static const double im_f_1_7[4][2] = {{-0.074998, 0}, {-0.12289, 0.019676}, {-0.175, 0.011456}, {-0.12783, -0.0082265}};
	static const double im_f_1_9[4][2] = {{0.16371, -0.059691}, {-0.25044, 0.016442}, {-0.86486, 0.027909}, {-2.5243, 0.050639}};
	static const double im_f_2_7[4][2] = {{0.44999, 0}, {0.73732, -0.11806}, {1.05, -0.068733}, {0.76698, 0.049359}};
	static const double im_f_2_9[4][2] = {{-0.98225, 0.35815}, {1.5026, -0.098649}, {5.1892, -0.16745}, {15.146, -0.30383}};
	
	const std::auto_ptr<ComplexPairList> k_1_7 = inner::getKFactor(re_f_1_7, im_f_1_7);
	const std::auto_ptr<ComplexPairList> k_1_9 = inner::getKFactor(re_f_1_9, im_f_1_9);
	const std::auto_ptr<ComplexPairList> k_2_7 = inner::getKFactor(re_f_2_7, im_f_2_7);
	const std::auto_ptr<ComplexPairList> k_2_9 = inner::getKFactor(re_f_2_9, im_f_2_9);
		
	const double mc1 = constants::mc/constants::mb;
	const double s2 = q2/(constants::mb*constants::mb);
	const double Lc = log(mc1);
	const double Ls = log(s2);
	const double Lm = log(parameters->getC_mb()->getScale()/constants::mb);
	
	DEBUGPRINT("mc1: ", mc1);//V
	DEBUGPRINT("s2: ", s2);//V
	DEBUGPRINT("Lc: ", Lc);//V
	DEBUGPRINT("Ls: ", Ls);//V
	DEBUGPRINT("Lm: ", Lm);//V
	
	const EvtComplex f_1_7 = inner::f_x_y(*k_1_7, s2, Ls);
	const EvtComplex f_1_9 = inner::f_x_y(*k_1_9, s2, Ls);
	const EvtComplex f_2_7 = inner::f_x_y(*k_2_7, s2, Ls);
	const EvtComplex f_2_9 = inner::f_x_y(*k_2_9, s2, Ls);
	
	DEBUGPRINT("f[1,7]: ", f_1_7);//V
	DEBUGPRINT("f[1,9]: ", f_1_9);//V
	DEBUGPRINT("f[2,7]: ", f_2_7);//V
	DEBUGPRINT("f[2,9]: ", f_2_9);//V
	
	//see Appendix B
	const EvtComplex IPi(0,constants::Pi);
	const EvtComplex F_1_9 = ((-1424/729.) + ((16/243.)*IPi) + ((64/27.)*Lc))*Lm -
		(16/243.)*Lm*Ls + ((16/1215.) - (32/(135.*mc1*mc1)))*Lm*s2 + 
		((4/2835.) - (8/(315.*pow(mc1,4))))*Lm*s2*s2 + ((16/76545.) - (32/(8505.*pow(mc1,6))))*Lm*pow(s2,3) -
		(256/243.)*Lm*Lm + f_1_9;
	const EvtComplex F_2_9 = ( (256/243.) - ((32/81.)*IPi) - (128/9.)*Lc)*Lm +
		(32/81.)*Lm*Ls + ((-32/405.) + (64/(45.*mc1*mc1)))*Lm*s2 + ( (-8/945.) + (16/(105.*pow(mc1,4))))*Lm*s2*s2 +
		( (-32/25515.) + (64/(2835.*pow(mc1,6))))*Lm*pow(s2,3) + (512/81.)*Lm*Lm + f_2_9;
	const EvtComplex F_1_7 = (-208/243.)*Lm + f_1_7;
	const EvtComplex F_2_7 = ((416/81.)*Lm) + f_2_7;
	
	DEBUGPRINT("F[1,9]: ", F_1_9);//V
	DEBUGPRINT("F[2,9]: ", F_2_9);//V
	DEBUGPRINT("F[1,7]: ", F_1_7);//V
	DEBUGPRINT("F[2,7]: ", F_2_7);//V
	
	const double reg = 0;
	//do the integration
	const EvtComplex c00 = inner::getC0(s2);
	const EvtComplex b10 = inner::B10(q2, constants::mb*constants::mb, reg);
	
	DEBUGPRINT("C00[s2]: ", c00);//V
	DEBUGPRINT("B10[q2,m]: ", b10);//V

	const EvtComplex F_8_7 = ((-32/9.)*Lm) - ((8/9.)*(s2/(1-s2))*log(s2)) - ((8/9.)*IPi) - 
		((4/9.)*((11 - (16*s2) + (8*s2*s2))/pow(1-s2,2))) + 
		(4/(9*pow(1-s2,3)))*(((9*s2) - (5*s2*s2) + (2*s2*s2*s2))*b10 - ((4 + 2*s2)*c00));
	const EvtComplex F_8_9 = ((16/(9*(1-s2)))*log(s2)) + ((8*(5 - 2*s2))/(9*pow(1-s2,3))) - 
			((8*(4-s2))/(9*pow(1-s2,3)))*(((1+s2)*b10) - (2*c00));
	
	DEBUGPRINT("F[8,7]: ", F_8_7);//V
	DEBUGPRINT("F[8,9]: ", F_8_9);//V
	
	DEBUGPRINT("mbp: ", qcd::mb_pole(constants::mb));//V
	DEBUGPRINT("h3(q^2,0): ", qcd::h(q2,0,parameters->getC_mb3()->getScale()));//V
	DEBUGPRINT("h3(q^2,mc): ", qcd::h(q2,constants::mc,parameters->getC_mb3()->getScale()));//V
	DEBUGPRINT("h3(q^2,mb): ", qcd::h(q2,constants::mb,parameters->getC_mb3()->getScale()));//V
	DEBUGPRINT("h3(q^2,mbp): ", qcd::h(q2,qcd::mb_pole(constants::mb),parameters->getC_mb3()->getScale()));//V
	DEBUGPRINT("Y3(q^2): ", qcd::Y(q2,*(parameters->getC_mb3())));//V
	
	DEBUGPRINT("PolyLog[2, 10102.3465 - 78798.918 I]",PolyLog(2,EvtComplex(10102.3465,78798.918)));
	DEBUGPRINT("PolyLog[2, -0.003465 - 0.00918 I]",PolyLog(2,EvtComplex(-0.003465,0.00918)));
	
	const Integrate t1_int_left(e,MB,mK,q2,parameters->getC_mb(),parameters->getC_mb3());
	const SignedPair<double> xi(xi1,xi2);
	const SignedPair<EvtComplex> C1 = t1_int_left.getC1(F_2_7,F_8_7,F_1_9,F_2_9,F_8_9);
	DEBUGPRINT("C1[1]: ", C1[1]);//V
	DEBUGPRINT("C1[2]: ", C1[2]);//V
	
	const EvtComplex t_1 = t1_int_left.get_t(1, xi, C1);
	DEBUGPRINT("t_1: ", t_1);
	const EvtComplex t_2 = t1_int_left.get_t(2, xi, C1);
	DEBUGPRINT("t_2: ", t_2);
	
	//add penguin corrections
	const EvtComplex C9_1 = (2*constants::mb*MB*t_1)/(q2*xi[1]);
	const EvtComplex C9_2 = (-2*constants::mb*t_2)/(MB*xi[2]);
	const SignedPair<EvtComplex> C9(C9_1,C9_2);
	const EvtComplex Ceff1_9 = (*(parameters->getC_mb()))(9) + qcd::Y(q2,*(parameters->getC_mb()));
	
	DEBUGPRINT("C9[1]: ", C9[1]);//V
	DEBUGPRINT("C9[2]: ", C9[2]);//V
	DEBUGPRINT("Ceff1[9]: ", Ceff1_9);//V
	
	//add contributions from primed operators
	SignedPair<EvtComplex> C9p(0.0,0.0);
	EvtComplex Ceff1_9p = 0.0;
	if(parameters->includeRHC){
		const IntegrateRight t1_int_right(e,MB,mK,q2,parameters->getCR_mb(),parameters->getCR_mb3());
		const SignedPair<EvtComplex> CR1 = t1_int_right.getC1(F_2_7,F_8_7,F_1_9,F_2_9,F_8_9);
		const EvtComplex tp_1 = t1_int_right.get_t(1, xi, CR1);
		DEBUGPRINT("tp[1]: ", tp_1);
		const EvtComplex tp_2 = t1_int_right.get_t(2, xi, CR1);
		DEBUGPRINT("tp[2]: ", tp_2);
		
		C9p.set(1,(2*constants::mb*MB*tp_1)/(q2*xi[1]));
		C9p.set(2,(-2*constants::mb*tp_2)/(MB*xi[2]));
		Ceff1_9p = (*(parameters->getCR_mb()))(9);
		
		DEBUGPRINT("C9p[1]: ", C9p[1]);
		DEBUGPRINT("C9p[2]: ", C9p[2]);
		DEBUGPRINT("Ceff1[9]p: ", Ceff1_9p);

	}
	
	
	//These terms are taken from eqns 4.10-4.17 of PR D61 074024 (2000),
	//with a few alterations to fit in with Beneke.
	
	const double mk = mK/MB;
	DEBUGPRINT("mk: ", mk);
	DEBUGPRINT("C7:",(*(parameters->getC_mb()))(7));
	DEBUGPRINT("C7\':",(*(parameters->getCR_mb()))(7));
	
	//eqn 4.10
	const EvtComplex tensA = ((2*(Ceff1_9 + Ceff1_9p)*ffV)/(1 + mk)) +
		((4*constants::mb*MB*((*(parameters->getC_mb()))(7)+(*(parameters->getCR_mb()))(7))*ffT1)/(q2)) +
		((2*(C9[1]+C9p[1])*xi[1]));
	DEBUGPRINT("tensA: ", tensA);//V
	tensors->at(A) = tensA;//will throw runtime exception if the vector is not correct
	
	//eqn 4.11
	const EvtComplex tensB = (1 + mk)*(((Ceff1_9 - Ceff1_9p)*ffA1) +
			(2*constants::mb*MB*(1 - mk)*((*(parameters->getC_mb()))(7)-(*(parameters->getCR_mb()))(7))*ffT2/q2)) +
		(2*e*(C9[1]-C9p[1])*xi[1]/MB);
	DEBUGPRINT("tensB: ", tensB);//V
	tensors->at(B) = tensB;//will throw runtime exception if the vector is not correct
	
	//eqn 4.12
	const EvtComplex tensC = (1./(1 - (mk*mk)))*(((1 - mk)*(Ceff1_9 + Ceff1_9p)*ffA2) + 
			(2*(constants::mb/MB)*((*(parameters->getC_mb()))(7)+(*(parameters->getCR_mb()))(7))*(ffT3 + (((MB*MB) - (mK*mK))/q2)*ffT2)) + 
			((C9[1]+C9p[1])*xi[1]) - ((C9[2]+C9p[2])*xi[2]));
	DEBUGPRINT("tensC: ", tensC);//V
	tensors->at(C) = tensC;//will throw runtime exception if the vector is not correct
		
	//eqn 4.14
	const EvtComplex tensE = (2*((*(parameters->getC_mb()))(10)+(*(parameters->getCR_mb()))(10))*ffV)/(1 + mk);
	DEBUGPRINT("tensE: ", tensE);//V
	tensors->at(E) = tensE;//will throw runtime exception if the vector is not correct
	
	//eqn 4.15
	const EvtComplex tensF = (1 + mk)*((*(parameters->getC_mb()))(10)-(*(parameters->getCR_mb()))(10))*ffA1;
	DEBUGPRINT("tensF: ", tensF);//V
	tensors->at(F) = tensF;//will throw runtime exception if the vector is not correct
	
	//eqn 4.16
	const EvtComplex tensG = ((*(parameters->getC_mb()))(10)+(*(parameters->getCR_mb()))(10))*(ffA2/(1 + mk));
	DEBUGPRINT("tensG: ", tensG);//V
	tensors->at(G) = tensG;//will throw runtime exception if the vector is not correct
	
	//eqn 4.17 of Ali and Ball, and also 3.18 of hep-ph/0004262
	const EvtComplex tensH = ((*(parameters->getC_mb()))(10)+(*(parameters->getCR_mb()))(10))*
		( ((MB*MB/q2)*( (((1 + mk)*ffA1)) - ((1 - mk)*ffA2) - (2*mk*ffA0))) ) +
		((mK*MB*ffA0*(*(parameters->getC_mb()))(12))/(constants::mmu*(constants::mb + constants::ms)));//scalar correction
	tensors->at(H) = tensH;//will throw runtime exception if the vector is not correct
	DEBUGPRINT("tensH: ", tensH);
	
	const EvtComplex tensS2 = (-MB*ffA0*(*(parameters->getC_mb()))(11))/(constants::mb + constants::ms);
	tensors->at(S2) = tensS2;//will throw runtime exception if the vector is not correct
	DEBUGPRINT("tensS2: ", tensS2);//V
}
