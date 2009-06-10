#include "EvtGenModels/EvtBToVllConstraints.hh"
#include "EvtGenModels/EvtBToVllEvolveWC.hh"
#include "EvtGenModels/EvtBToVllQCDUtils.hh"

#include <cstdio>
#include "gsl/gsl_deriv.h"
#include "gsl/gsl_errno.h"
#include "gsl/gsl_integration.h"
#include "gsl/gsl_roots.h"

EvtBToVllConstraints::EvtBToVllConstraints(const QCDFactorisation& _fact):
		fact(_fact){
	
	qcd::WCPtrNaked C = fact.parameters->C_mb.get();
	qcd::WCPtrNaked CR = fact.parameters->CR_mb.get();
	qcd::WCPtrNaked CNP = fact.parameters->CNP_mw.get();
	
	Cb1_1 = (*C)(1);
	Cb1_2 = (*C)(2);
	Cb1_3 = (*C)(3);
	Cb1_4 = (*C)(4);
	Cb1_5 = (*C)(5);
	Cb1_6 = (*C)(6);
	//
	Ceff1_7 = (*C)(7);
	Ceff1_8 = (*C)(8);
	Cb1_9 = (*C)(9);
	Cb1_10 = (*C)(10);
	//
	Ceffp1_7 = (*CR)(7);
	Ceffp1_8 = (*CR)(8);
	Cbp1_9 = (*CR)(9);
	Cbp1_10 = (*CR)(10);
	//
	C7NP = qcd::EvtBToVllEvolveWC10D::runC7((*CNP)(7),(*CNP)(8),qcd::MU_MB);
	C8NP = qcd::EvtBToVllEvolveWC10D::runC8((*CNP)(8),qcd::MU_MB);
	C9NP = (*CNP)(9);
	C10NP = (*CNP)(10);
	//
	C7sm = Ceff1_7 - C7NP;
	C8sm = Ceff1_8 - C8NP;
	C9sm = Cb1_9 - C9NP;
	C10sm = Cb1_10 - C10NP;
	//
	CQ1 = (*C)(11);
	CQ2 = (*C)(12);

	//Calculate some CKM factors from unitarity
	Vub = constants::AbsVub/Power(Complex(constants::E),Complex(0,constants::gamma));
	Vts = -((constants::AbsVub*Power(Complex(constants::E),Complex(0,constants::gamma))*
			Sqrt(1 - Power(constants::Vcb,2)/(1 - Power(constants::AbsVub,2)))*constants::Vus)/
				Sqrt(1 - Power(constants::AbsVub,2))) - 
			(constants::Vcb*Sqrt(1 - Power(constants::Vus,2)/(1 - Power(constants::AbsVub,2))))/
			Sqrt(1 - Power(constants::AbsVub,2));
	Vtb = Sqrt(1 - Power(constants::AbsVub,2))*Sqrt(1 - Power(constants::Vcb,2)/(1 - Power(constants::AbsVub,2)));

}

double EvtBToVllConstraints::getBrBsToMuMu() const{
	
	//Calculates the branching fraction of Bs->MuMu
	//using eqn 6.3 of arXiv:0811.1214
	
	EvtComplex S = 0.5*constants::mBs*constants::mBs*CQ1;
	EvtComplex P = (0.5*constants::mBs*constants::mBs*CQ2) + (constants::mmu*(Cb1_10 - Cbp1_10));
	
	EvtComplex result = (Power(constants::alphaEM,2)*Power(constants::fBs,2)*Power(constants::GF,2)*constants::mBs*
		     Sqrt(1 - (4*Power(constants::mmu,2))/Power(constants::mBs,2))*constants::tauBs*
		     (Power(Abs(P),2) + (1 - (4*Power(constants::mmu,2))/Power(constants::mBs,2))*
		        Power(Abs(S),2))*Power(Abs(Vtb*conj(Vts)),2))/(16.*Power(constants::Pi,3));
	DEBUGPRINT("BRBmumu", result);//V
	return real(result);//branching fractions are real!
	
}

double EvtBToVllConstraints::getBrBToXsGamma() const{
	
	/*
	 * Taken from Matias + Lunghi, hep-ph/0612166, 
	 * and rescaled to 3.28e-4 (cf. Hurth et al. 0807.5039)
	 * (3.28 +/- 0.25 )e-4  from Theoretical SM
	 * (3.55 +/- 0.24) e-4 from Experiment
	 */
	
	qcd::WCPtrNaked CR = fact.parameters->getCR_mw();
	qcd::WCPtrNaked CNP = fact.parameters->getCNP_mw();

	EvtComplex dC70 = (*CNP)(7);
	EvtComplex dC7P0 = (*CR)(7);
	EvtComplex dC80 = (*CNP)(8);
	EvtComplex dC8P0 = (*CR)(8);

	const double a(2.98);
	const EvtComplex a7(-7.184,0.612);
	const double a77(4.743);
	const EvtComplex a8(-2.225,-0.557);
	const double a88(0.789);
	const EvtComplex a78(2.454,-0.884);
	//See arXiv:0805.0271 for updated normalisation
	EvtComplex result = (3.28e-4/a)*(a + a77*(Power(Abs(dC70),2) + Power(Abs(dC7P0),2)) + 
		       	a88*(Power(Abs(dC80),2) + Power(Abs(dC8P0),2)) + 
		       	Re(a7*dC70 + a8*dC80 + a78*(dC70*Conjugate(dC80) + dC7P0*Conjugate(dC8P0))));
	
	DEBUGPRINT("BRBXsgamma: ", result);//V
	return real(result);
	
}

double EvtBToVllConstraints::getBrBToXsll() const{
	
	class utils{
	public:
		
		utils(const qcd::WCPtrNaked _C, const qcd::WCPtrNaked _CR, const EvtComplex _Vtb, const EvtComplex _Vts):
			C(_C),CR(_CR),C100(0.215),C200(-0.487),nfl(5),Vtb(_Vtb),Vts(_Vts){
		}

		static double getLS0(const double sh){
			return log(sh);
		};
		static EvtComplex omega9(const double sh){
			return (-2*Power(constants::Pi,2))/9. + (5 + 9*sh - 6*Power(sh,2))/
		    	(6.*(1 - sh)*(1 + 2*sh)) - ((5 + 4*sh)*Log(1 - sh))/(3.*(1 + 2*sh)) - 
		    	(2*(1 - 2*sh)*sh*(1 + sh)*Log(sh))/(3.*Power(1 - sh,2)*(1 + 2*sh)) - 
		    	(2*Log(1 - sh)*Log(sh))/3. - (4*PolyLog(2,sh))/3.;
		};
		
		static EvtComplex omega7(const double sh, const double mu){
			return (-2*Power(constants::Pi,2))/9. - (16 - 11*sh - 17*Power(sh,2))/
		    (18.*(1 - sh)*(2 + sh)) - (8*Log(mu/constants::mb))/3. - ((8 + sh)*Log(1 - sh))/(3.*(2 + sh)) - 
		   (2*sh*(2 - 2*sh - Power(sh,2))*Log(sh))/(3.*Power(1 - sh,2)*(2 + sh)) - 
		   (2*Log(1 - sh)*Log(sh))/9. - (4*PolyLog(2,sh))/3.;
		};
		
		static EvtComplex get_F_1_9(const double sh, const double mu){
		
			const double mc1 = constants::mc/constants::mb;
			const double Lc = log(mc1);
			const double Ls = getLS0(sh);
			const double Lm = log(mu/constants::mb);
			const EvtComplex IPi(0,constants::Pi);

			return QCDFactorisation::inner::get_F_1_9(Lc,Lm,Ls,mc1,sh);
		};
		
		static EvtComplex get_F_2_9(const double sh, const double mu){
				
			const double mc1 = constants::mc/constants::mb;
			const double Lc = log(mc1);
			const double Ls = getLS0(sh);
			const double Lm = log(mu/constants::mb);
			const EvtComplex IPi(0,constants::Pi);

			return QCDFactorisation::inner::get_F_2_9(Lc,Lm,Ls,mc1,sh);
		};
		
		static EvtComplex get_F_8_9(const double sh, const double){
			return 11.555555555555555 - (32*Power(constants::Pi,2))/27. + 
			   (43.851851851851855 - (40*Power(constants::Pi,2))/9.)*sh + 
			   (105.27407407407408 - (32*Power(constants::Pi,2))/3.)*Power(sh,2) + 
			   (204.7026455026455 - (560*Power(constants::Pi,2))/27.)*Power(sh,3) + 
			   (16*(1 + sh + Power(sh,2) + Power(sh,3))*getLS0(sh))/9.;
		};
		
		static EvtComplex f(const EvtComplex zz){
			return 1 - 8*Power(zz,2) + 8*Power(zz,6) - Power(zz,8) - 
			   24*Power(zz,4)*Log(zz);
		};
		
		EvtComplex kappa(const EvtComplex zz) const{
			return 1 - (2*(1.5 + (-7.75 + Power(constants::Pi,2))*Power(1 - zz,2))*
				      qcd::alpha_s(constants::mb,nfl))/(3.*constants::Pi);
		};
		
		EvtComplex getC7new(const double sh, const double mu) const{
			return (*C)(7)*(1 + (qcd::alpha_s(constants::mb,nfl)*omega7(sh,mu))/constants::Pi);
		};
		
		EvtComplex getC9new(const double sh, const double mu) const{
			return -(qcd::alpha_s(constants::mb,nfl)*(C100*get_F_1_9(sh,mu) + C200*get_F_2_9(sh,mu) + 
			         (*C)(8)*get_F_8_9(sh,mu)))/(4.*constants::Pi) + 
			         (1 + (qcd::alpha_s(constants::mb,nfl)*omega9(sh))/constants::Pi)*
			         ((*C)(9) + qcd::Y(constants::mb*constants::mb*sh,*C));
		};
		
		EvtComplex getC10new(const double sh, const double) const{
			return (*C)(10)*(1 + (qcd::alpha_s(constants::mb,nfl)*omega9(sh))/constants::Pi);
		};
		
		EvtComplex getU(const double sh, const double mu) const{
			
			const double mlh = constants::mmu/constants::mb; 
			
			return (3*Power(constants::mb, 2)*(1 - (4*Power(mlh, 2))/sh)*sh
					* (Power(Abs((*C)(11)), 2) + Power(Abs((*C)(12)), 2)))/2. + 6*Power(
					mlh, 2)*(-Power(Abs(getC10new(sh,mu)), 2) + Power(Abs(getC9new(sh,mu)),
					2) + Power(Abs((*CR)(9)), 2) - Power(Abs((*CR)(10)), 2)) + (1
					+ (2*Power(mlh, 2)*(1 - sh))/sh + 2*sh)* (Power(
					Abs(getC10new(sh,mu)), 2) + Power(Abs(getC9new(sh,mu)), 2) + Power(
					Abs((*CR)(9)), 2) + Power(Abs((*CR)(10)), 2)) + 6
					*constants::mb*mlh*Re((*C)(11)*getC10new(sh,mu)) + (1 + (2*Power(mlh,
					2))/sh)* ((4*(2 + sh)*Power(Abs(getC7new(sh,mu)), 2))/sh + 12
					*Re(getC7new(sh,mu)*Conjugate(getC9new(sh,mu)) + (*CR)(7)
							*Conjugate((*CR)(9))));
		};
		
		double getDiffDecayRate(const double sh) const{
			const double mlh = constants::mmu/constants::mb;
			return real((Power(constants::alphaEM,2)*Sqrt(1 - (4*Power(mlh,2))/sh)*Power(1 - sh,2)*
				     getU(sh,C->getScaleValue())*Power(Abs(Vtb*Vts),2))/(4.*Power(constants::Pi,2)*Power(Abs(constants::Vcb),2)*
				    		 f(constants::mc/constants::mb)*kappa(constants::mc/constants::mb)));
		};
		
		//form dictated by gsl_function
		static double integralFunction(double s, void* p){
			utils* u = (utils*)p;
			return u->getDiffDecayRate(s);
		}
		
	private:
		
		const qcd::WCPtrNaked C;
		const qcd::WCPtrNaked CR;
		
		const double C100;
		const double C200;
		const int nfl;
		
		EvtComplex Vtb;
		EvtComplex Vts;
		
	};
	
	qcd::WCPtrNaked C = fact.parameters->C_mb.get();
	qcd::WCPtrNaked CR = fact.parameters->CR_mb.get();

	utils u(C,CR,Vts,Vtb);
	
	double result = 0;
	double error = 0;
	const int nDivisions = 100;
	const double accuracyGoal = 1e-4;

	gsl_function F;
	F.function = &utils::integralFunction;
	F.params = &u;

	const double sMin = 1/(constants::mb*constants::mb);
	const double sMax = 6/(constants::mb*constants::mb);
	
	gsl_integration_workspace* w = gsl_integration_workspace_alloc(nDivisions);
	const int intCode = gsl_integration_qag(&F, sMin, sMax, 0, accuracyGoal,
			nDivisions, GSL_INTEG_GAUSS61, w, &result, &error);
	if (intCode != 0) {
		report(WARNING,"EvtGen") << "Numerical integration did not return cleanly."
				<< std::endl;
		report(WARNING,"EvtGen") << "Return code of the integration was " << intCode
				<< std::endl;
		report(WARNING,"EvtGen") << "Result: " << result << " Error: " << error << std::endl;
	}
	gsl_integration_workspace_free(w);
	result *= 0.1074;
	DEBUGPRINT("Br(B->X_s mu mu): ", result);//V
	return result;
}

double EvtBToVllConstraints::getAFB(const double q2) const{
	
	const double MB = constants::mB;
	const double mK = constants::mKstar;
	const double mk = mK/MB;
	const double mlhat = constants::mmu/MB;

	const double shat = q2/(MB*MB);
	//get the amplitude tensors
	std::vector<EvtComplex> tensors(QCDFactorisation::NUMBER_OF_TENSORS);
	fact.getTnAmplitudes(q2,MB,mK, &tensors);
		
	const EvtComplex tensA = tensors.at(QCDFactorisation::A);
	const EvtComplex tensB = tensors.at(QCDFactorisation::B);
	const EvtComplex tensC = tensors.at(QCDFactorisation::C);
	const EvtComplex tensD = tensors.at(QCDFactorisation::D);
	const EvtComplex tensE = tensors.at(QCDFactorisation::E);
	const EvtComplex tensF = tensors.at(QCDFactorisation::F);
	const EvtComplex tensG = tensors.at(QCDFactorisation::G);
	const EvtComplex tensH =tensors.at(QCDFactorisation::H);
	const EvtComplex tensS2 =tensors.at(QCDFactorisation::S2);
	
	const double lambda = 1 + (mk*mk*mk*mk) + (shat*shat) - (2*shat)*(1 + shat);
	const double uhat = sqrt(lambda*(1 - ((4*mlhat*mlhat)/shat)));
	
	const double dKstar = getDstar(q2,tensors);
	
	const double AFB = (uhat/dKstar)*( shat*(real(tensB*conj(tensE)) +  real(tensA*conj(tensF))) +
										(mlhat/mk)*( real(tensS2*conj(tensB))*(1 - shat - mk*mk) - 
														real(tensS2*conj(tensC))*lambda) );
	return AFB;
}

double EvtBToVllConstraints::findAFBZero() const{
	
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

double EvtBToVllConstraints::getDstar(const double q2, const std::vector<EvtComplex>& tensors) const{
	
	const EvtComplex tensA = tensors.at(QCDFactorisation::A);
	const EvtComplex tensB = tensors.at(QCDFactorisation::B);
	const EvtComplex tensC = tensors.at(QCDFactorisation::C);
	const EvtComplex tensD = tensors.at(QCDFactorisation::D);
	const EvtComplex tensE = tensors.at(QCDFactorisation::E);
	const EvtComplex tensF = tensors.at(QCDFactorisation::F);
	const EvtComplex tensG = tensors.at(QCDFactorisation::G);
	const EvtComplex tensH =tensors.at(QCDFactorisation::H);
	const EvtComplex tensS2 =tensors.at(QCDFactorisation::S2);
	
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
double EvtBToVllConstraints::getGammaKstar(const double q2) const{
	//dGamma^{K*}/ds
	
	const double MB = constants::mB;
	const double mK = constants::mKstar;
	const double mk = mK/MB;
	const double mlhat = constants::mmu/MB;

	const double shat = q2/(MB*MB);
	const double lambda = 1 + (mk*mk*mk*mk) + (shat*shat) - (2*shat)*(1 + shat);
	const double uhat = sqrt(lambda*(1 - ((4*mlhat*mlhat)/shat)));
	
	//get the amplitude tensors
	std::vector<EvtComplex> tensors(QCDFactorisation::NUMBER_OF_TENSORS);
	fact.getTnAmplitudes(q2,MB,mK, &tensors);
	
	//eqn (3.56) with arbitratry normalisation
	const double dStar = getDstar(q2,tensors);
	return uhat*dStar;
}

inline double EvtBToVllConstraints::getBeta(const double q2) const{
	return sqrt(1 - ((4*constants::mmu*constants::mmu)/q2));
	
}
inline double EvtBToVllConstraints::getLambda(const double q2) const{
	const double mB2 = constants::mB * constants::mB;
	const double mk2 = (constants::mKstar/constants::mB)*(constants::mKstar/constants::mB);
	return 1 + (mk2*mk2) + (q2*q2)/(mB2*mB2) - 2*( (q2/mB2) + (mk2)*(1 + q2/mB2) );
}
inline double EvtBToVllConstraints::getN(const double q2, const double beta, const double lambda) const{
	const double constant = (constants::GF*constants::GF*constants::alphaEM*constants::alphaEM)/
		(3*pow(2,10)*pow(constants::Pi,5)*constants::mB);
	return constants::Vtstb * sqrt(constant * q2 * sqrt(lambda) * beta);
}

void EvtBToVllConstraints::getSpinAmplitudes(const double q2, std::vector<EvtComplex>* amps, const bool isBbar) const{

	const bool _isBbar = fact.parameters->getisBbar();
	
	//get the amplitude tensors
	std::vector<EvtComplex> tensors(QCDFactorisation::NUMBER_OF_TENSORS);
	fact.parameters->setBbar(isBbar);
	fact.getTnAmplitudes(q2,constants::mB,constants::mKstar, &tensors);
	
	const double scaleFactor = 1e10;//stop the amplitudes getting too small...
		
	const EvtComplex tensA = tensors.at(QCDFactorisation::A);
	const EvtComplex tensB = tensors.at(QCDFactorisation::B);
	const EvtComplex tensC = tensors.at(QCDFactorisation::C);
	const EvtComplex tensD = tensors.at(QCDFactorisation::D);
	const EvtComplex tensE = tensors.at(QCDFactorisation::E);
	const EvtComplex tensF = tensors.at(QCDFactorisation::F);
	const EvtComplex tensG = tensors.at(QCDFactorisation::G);
	const EvtComplex tensH =tensors.at(QCDFactorisation::H);
	const EvtComplex tensS2 =tensors.at(QCDFactorisation::S2);
	
	DEBUGPRINT("tensA: ", tensA);
	DEBUGPRINT("tensB: ", tensB);
	DEBUGPRINT("tensC: ", tensC);
	DEBUGPRINT("tensE: ", tensE);
	DEBUGPRINT("tensF: ", tensF);
	DEBUGPRINT("tensG: ", tensG);
	DEBUGPRINT("tensH: ", tensH);
	DEBUGPRINT("tensS2: ", tensS2);
	
	//now the auxilleries...
	const EvtComplex aL = tensB - tensF;
	const EvtComplex aR = tensB + tensF;
	const EvtComplex bL = 0.5 * (tensC - tensG);
	const EvtComplex bR = 0.5 * (tensC + tensG);
	const EvtComplex cL = 0.5 * (tensA - tensE);
	const EvtComplex cR = 0.5 * (tensA + tensE);
	
	DEBUGPRINT("q2: ",q2);
	DEBUGPRINT("aLR[L]: ",aL);
	DEBUGPRINT("aLR[R]: ",aR);
	DEBUGPRINT("bLR[L]: ",bL);
	DEBUGPRINT("bLR[R]: ",bR);
	DEBUGPRINT("cLR[L]: ",cL);
	DEBUGPRINT("cLR[R]: ",cR);
	
	DEBUGPRINT("bbl[1.0]: ",getBeta(1.0));//V
	DEBUGPRINT("lambda[1.0]: ",getLambda(1.0));//V
	DEBUGPRINT("Nm[1.0]: ",getN(1.0, getBeta(1.0), getLambda(1.0)));//V
	
	const double beta = getBeta(q2);
	const double lambda = getLambda(q2);
	const double N = getN(q2, beta, lambda);
	const double mB2 = constants::mB * constants::mB;
	const double mk = constants::mKstar/constants::mB;
	
	//calculate the form factors
	double ffA0, ffA1, ffA2, ffV;
	double ffT1, ffT2, ffT3;
	double xi1, xi2;
	
	fact.getFormFactors(q2,
			&ffA0,&ffA1,&ffA2,&ffV,
			&ffT1,&ffT2,&ffT3,
			&xi1,&xi2);
	
	//now calculate the spin amplitudes...
	const EvtComplex _ATL = scaleFactor * sqrt(2*lambda)*N*constants::mB*cL;
	const EvtComplex _ATR = scaleFactor * sqrt(2*lambda)*N*constants::mB*cR;
	
	const EvtComplex _APL = scaleFactor * -sqrt(2)*N*constants::mB*aL;
	const EvtComplex _APR = scaleFactor * -sqrt(2)*N*constants::mB*aR;
	
	const EvtComplex _A0L = scaleFactor * ((constants::mB*mB2)*N*(-((1 - (mk*mk) - q2/mB2)*aL)/2. + lambda*bL))/(constants::mKstar*sqrt(q2));
	const EvtComplex _A0R = scaleFactor * ((constants::mB*mB2)*N*(-((1 - (mk*mk) - q2/mB2)*aR)/2. + lambda*bR))/(constants::mKstar*sqrt(q2));
	
	const EvtComplex _AT = scaleFactor * -(sqrt(2)*aL*constants::mB*N);
	const EvtComplex _AS = scaleFactor * -(CQ1*ffA0*N*sqrt(lambda));
	
	DEBUGPRINT("ATL: ", _ATL);
	DEBUGPRINT("ATR: ", _ATR);
	
	DEBUGPRINT("APL: ", _APL);
	DEBUGPRINT("APR: ", _APR);
	
	DEBUGPRINT("A0L: ", _A0L);
	DEBUGPRINT("A0R: ", _A0R);

	DEBUGPRINT("AT: ", _AT);
	DEBUGPRINT("AS: ", _AS);
	
	amps->at(ATL) = _ATL;
	amps->at(ATR) = _ATR;
	amps->at(APL) = _APL;
	amps->at(APR) = _APR;
	amps->at(A0L) = _A0L;
	amps->at(A0R) = _A0R;
	amps->at(AT) = _AT;
	amps->at(AS) = _AS;
	
	//reset back to the previous value
	fact.parameters->setBbar(_isBbar);
}

const double EvtBToVllConstraints::getJ5(const double q2) const{
	
	std::vector<EvtComplex> amps(NUMBER_OF_AMPS);
	getSpinAmplitudes(q2, &amps, true);//calc for the Bbar
	
	return (3*getBeta(q2)*(-((constants::mmu*(Re(amps.at(APL)*Conjugate(amps.at(AS))) +
			Re(amps.at(APR)*Conjugate(amps.at(AS)))))/sqrt(q2)) + Re(amps.at(A0L)*Conjugate(amps.at(APL))) - 
	       Re(amps.at(A0R)*Conjugate(amps.at(APR)))))/(2.*sqrt(2));
	
}

const double EvtBToVllConstraints::getJ6(const double q2) const{
	
	std::vector<EvtComplex> amps(NUMBER_OF_AMPS);
	getSpinAmplitudes(q2, &amps, true);//calc for the Bbar
	return ( 1.5*getBeta(q2)*( Re(amps.at(ATL) * Conjugate(amps.at(APL))) - Re(amps.at(ATR) * Conjugate(amps.at(APR))) ) );
}

const std::pair<double, double> EvtBToVllConstraints::getS5Zero() const{
	
	//hack to get const correctness 
	class s5utils{
	public:
		s5utils(const EvtBToVllConstraints* _calc):calc(_calc){
		}
		static double getValue(const double q2, void* params){
			const s5utils* c = (s5utils*)params;
			const double result = c->calc->getJ5(q2);//scale up to a more useful range
			std::cout << q2 << "\t" << result << std::endl;
			return result;
		}
	private:
		const EvtBToVllConstraints* calc;
	};
	
	
	s5utils u(this);
	
	
    gsl_function F;
    F.function = &s5utils::getValue;
    F.params = &u;
    return findZero(&F,2.25);

}

const std::pair<double, double> EvtBToVllConstraints::getS6Zero() const{
	
	//hack to get const correctness 
	class s6utils{
	public:
		s6utils(const EvtBToVllConstraints* _calc):calc(_calc){
		}
		static double getValue(const double q2, void* params){
			const s6utils* c = (s6utils*)params;
			const double result = c->calc->getJ6(q2);//scale up to a more useful range
			std::cout << q2 << "\t" << result << std::endl;
			return result;
		}
	private:
		const EvtBToVllConstraints* calc;
	};
	
	
	s6utils u(this);
	
	
    gsl_function F;
    F.function = &s6utils::getValue;
    F.params = &u;
    return findZero(&F,4.0);

}

const std::pair<double, double> EvtBToVllConstraints::findZero(gsl_function* F, const double r_expected) const{

    int status = 0;
	int iter = 0;
	const int max_iter = 100;
    
    const gsl_root_fsolver_type *T = gsl_root_fsolver_brent;
    gsl_root_fsolver *s = gsl_root_fsolver_alloc(T);
    
    double q2Min = 1;
    double q2Max = sqrt(constants::mB*constants::mB - constants::mKstar*constants::mKstar);

    double r = q2Min;

    gsl_root_fsolver_set(s,F,q2Min, q2Max);
  
    do
      {
        iter++;
        status = gsl_root_fsolver_iterate(s);
        r = gsl_root_fsolver_root(s);
        q2Min = gsl_root_fsolver_x_lower(s);
        q2Max = gsl_root_fsolver_x_upper(s);
        status = gsl_root_test_interval(q2Min, q2Max, 0, 0.0001);
  
        if (status == GSL_SUCCESS)
          printf ("Converged:\n");
  
        printf ("%5d [%.7f, %.7f] %.7f %+.7f %.7f\n",
                iter, q2Min, q2Min,
                r, r - r_expected, 
                q2Min - q2Max);
      }
    while (status == GSL_CONTINUE && iter < max_iter);
    
    double gradient = 0.0;
    if( status != GSL_SUCCESS){
    	std::cout << "Zero finding failed" << std::endl;
    	r = -1.0; //outside the kinimatic range
    }else{
    	gradient = findZeroGradient(F,r);
    }
    gsl_root_fsolver_free(s);
    
    
	return std::make_pair(r,gradient);
	
}

const double EvtBToVllConstraints::findZeroGradient(gsl_function* F, const double zero) const{
	
	double result = 0.0;
	double abserr = 1e6;
	
	gsl_deriv_central(F, zero, 1e-6, &result, &abserr);
	printf("f'(x) = %.10f +/- %.10f\n", result, abserr);
	return result;
}
