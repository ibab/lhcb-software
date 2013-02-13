#include "EvtGenModels/EvtBToVllBasisCoeffs.hh"
#include "EvtGenModels/EvtBToVllConstraints.hh"
#include "EvtGenModels/EvtBToVllEvolveWC.hh"
#include "EvtGenModels/EvtBToVllQCDUtils.hh"

#include "EvtGenBase/EvtReport.hh"

#include <cstdio>
#include "gsl/gsl_deriv.h"
#include "gsl/gsl_integration.h"
#include "gsl/gsl_roots.h"

int GSL_ERROR_HANDLER_CALLED = GSL_SUCCESS;
void debug_handler(const char* reason, const char* file, int line, int gsl_errno){
	report(WARNING,"EvtGen") << "GSL Error in " << file << " (line " << line  << ") (code " << gsl_errno << "): " << reason << std::endl;
	GSL_ERROR_HANDLER_CALLED = gsl_errno;
}

EvtBToVllConstraints::EvtBToVllConstraints(const QCDFactorisation& _fact):
		fact(_fact),q2min(1.0),q2max(6.0){
	
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
	C7R = qcd::EvtBToVllEvolveWC10D::runC7((*CR)(7),(*CR)(8),qcd::MU_MB);
	//
	C7sm = Ceff1_7 - C7NP;
	C8sm = Ceff1_8 - C8NP;
	C9sm = Cb1_9 - C9NP;
	C10sm = Cb1_10 - C10NP;
	//
	CQ1 = (*C)(11);
	CQ2 = (*C)(12);

	DEBUGPRINT("Vub: ", constants::Vub);//V
	DEBUGPRINT("Vts: ", constants::Vts);//V
	DEBUGPRINT("Vtb: ", constants::Vtb);//V
	DEBUGPRINT("Vtstb: ", constants::Vtstb);//V
	
}

double EvtBToVllConstraints::getBrBsToMuMu() const{
	
	//Calculates the branching fraction of Bs->MuMu
	//using eqn 6.3 of arXiv:0811.1214
	
	EvtComplex S = 0.5*constants::mBs*constants::mBs*CQ1;
	EvtComplex P = (0.5*constants::mBs*constants::mBs*CQ2) + (constants::mmu*(Cb1_10 - Cbp1_10));
	
	EvtComplex result = (Power(constants::alphaEM,2)*Power(constants::fBs,2)*Power(constants::GF,2)*constants::mBs*
		     Sqrt(1 - (4*Power(constants::mmu,2))/Power(constants::mBs,2))*constants::tauBs*
		     (Power(Abs(P),2) + (1 - (4*Power(constants::mmu,2))/Power(constants::mBs,2))*
		        Power(Abs(S),2))*Power(Abs(constants::Vtb*conj(constants::Vts)),2))/(16.*Power(constants::Pi,3));
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

/** Calculation of S(B\to K*\gamma) from arXiv:0805.2525 */
double EvtBToVllConstraints::getSBToKStarGamma() const{
	
	const double beta = (21.1/180)*constants::Pi;//Value of beta from HFAG Winter 08/09
	const EvtComplex tbyxi(-0.08912655225010323,-0.033310267087406725);
	const double tpbyxi = 0.0;//an approximation - we neglect the NP contribution to C8

	const EvtComplex common = ((Ceffp1_7 + tpbyxi)/(Ceff1_7 + tbyxi));
	const EvtComplex top = 2*Im(common*Exp(EvtComplex(0.0,-2*beta)));
	const EvtComplex bottom = Abs(common);

	const EvtComplex SBtoKStarGamma =  top/(1 + bottom*bottom);
	DEBUGPRINT("SBtoKStarGamma: ", SBtoKStarGamma);//V

	return Re(SBtoKStarGamma);
}

const double EvtBdToXsllParameters::mbp = qcd::mb_pole(constants::mb, qcd::mb_pole(constants::mb));//value at scale mbp
const double EvtBdToXsllParameters::muX = qcd::mb_pole(constants::mb);//normal value

double EvtBToVllConstraints::getBrBToXsll() const{
	
	class utils{
	public:
		
		utils(const qcd::WCPtrNaked _C, const qcd::WCPtrNaked _CR):
			C(_C),CR(_CR),C100(0.215),C200(-0.487),nfl(5){
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
		    (18.*(1 - sh)*(2 + sh)) - (8*Log(mu/EvtBdToXsllParameters::mbp))/3. - ((8 + sh)*Log(1 - sh))/(3.*(2 + sh)) - 
		   (2*sh*(2 - 2*sh - Power(sh,2))*Log(sh))/(3.*Power(1 - sh,2)*(2 + sh)) - 
		   (2*Log(1 - sh)*Log(sh))/9. - (4*PolyLog(2,sh))/3.;
		};
		
		static EvtComplex get_F_1_9(const double sh, const double mu){
			
			const double mc1 = constants::mc/EvtBdToXsllParameters::mbp;
			const double Lc = log(mc1);
			const double Ls = getLS0(sh);
			const double Lm = log(mu/EvtBdToXsllParameters::mbp);
			return QCDFactorisation::inner::get_F_1_9(Lc,Lm,Ls,mc1,sh);
		};
		
		static EvtComplex get_F_2_9(const double sh, const double mu){
			
			const double mc1 = constants::mc/EvtBdToXsllParameters::mbp;
			const double Lc = log(mc1);
			const double Ls = getLS0(sh);
			const double Lm = log(mu/EvtBdToXsllParameters::mbp);
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
				      qcd::alpha_s(EvtBdToXsllParameters::mbp,nfl))/(3.*constants::Pi);
		};
		
		EvtComplex getC7new(const double sh, const double mu) const{
			return (*C)(7)*(1 + (qcd::alpha_s(mu,nfl)*omega7(sh,mu))/constants::Pi);
		};
		
		EvtComplex getC9new(const double sh, const double mu) const{
			return -(qcd::alpha_s(mu,nfl)*(C100*get_F_1_9(sh,mu) + C200*get_F_2_9(sh,mu) + 
			         (*C)(8)*get_F_8_9(sh,mu)))/(4.*constants::Pi) + 
			         (1 + ((qcd::alpha_s(mu,nfl)*omega9(sh))/constants::Pi))*
			         ((*C)(9) + qcd::Y(EvtBdToXsllParameters::mbp*EvtBdToXsllParameters::mbp*sh,*C));
		};
		
		EvtComplex getC10new(const double sh, const double mu) const{
			return (*C)(10)*(1 + (qcd::alpha_s(mu,nfl)*omega9(sh))/constants::Pi);
		};
		
		EvtComplex getU(const double sh, const double mu, const double mlh) const{
			const EvtComplex C7new = getC7new(sh,mu);
			const EvtComplex C9new = getC9new(sh,mu);
			const EvtComplex C10new = getC10new(sh,mu);
			return (3*Power(EvtBdToXsllParameters::mbp,2)*(1 - (4*Power(mlh,2))/sh)*sh*
				      (Power(Abs((*C)(11)),2) + Power(Abs((*C)(12)),2)))/2. + 
				   6*Power(mlh,2)*(-Power(Abs(C10new),2) + 
				      Power(Abs(C9new),2) + Power(Abs((*CR)(9)),2) - 
				      Power(Abs((*CR)(10)),2)) + 
				   (1 + (2*Power(mlh,2)*(1 - sh))/sh + 2*sh)*
				    (Power(Abs(C10new),2) + Power(Abs(C9new),2) + 
				      Power(Abs((*CR)(9)),2) + Power(Abs((*CR)(10)),2)) + 
				   6*EvtBdToXsllParameters::mbp*mlh*Re((*C)(11)*C10new) + 
				   (1 + (2*Power(mlh,2))/sh)*
				    ((4*(2 + sh)*Power(Abs(C7new),2))/sh + 
				      12*Re(C7new*Conjugate(C9new) + 
				    		  (*C)(7)*Conjugate((*CR)(9))));
		};
		
		double getDiffDecayRate(const double sh, const double lepton_mass) const{
			const double mlh = lepton_mass/EvtBdToXsllParameters::mbp;
			return real((Power(constants::alphaEM,2)*Sqrt(1 - (4*Power(mlh,2))/sh)*Power(1 - sh,2)*
				     getU(sh,EvtBdToXsllParameters::muX,mlh)*Power(Abs(constants::Vtb*constants::Vts),2))/(4.*Power(constants::Pi,2)*Power(Abs(constants::Vcb),2)*
				    		 f(constants::mc/EvtBdToXsllParameters::mbp)*kappa(constants::mc/EvtBdToXsllParameters::mbp)));
		};
		
		//form dictated by gsl_function
		static double integralFunction(double s, void* p){
			utils* u = (utils*)p;
			//do the mu e average. Should make very little difference
			return 0.5*(u->getDiffDecayRate(s,constants::mmu) + u->getDiffDecayRate(s,constants::mme));
		}
		
	private:
		
		const qcd::WCPtrNaked C;
		const qcd::WCPtrNaked CR;
		
		const double C100;
		const double C200;
		const int nfl;
	};
	
	qcd::WCPtr  C_mw = fact.getModel().getLeftWilsonCoefficientsMW();
	qcd::WCPtr  CNP_mw = fact.getModel().getLeftNewPhysicsDeltasMW();
	qcd::WCPtr  CR_mw = fact.getModel().getRightWilsonCoefficientsMW();
	
	qcd::EvtBToVllEvolveWC10D evolveMb(*C_mw,*CNP_mw,*CR_mw);
	std::auto_ptr<qcd::WilsonPair> _mbp(evolveMb(qcd::MU_MBP));
	
	qcd::WCPtr C(_mbp->first);
	qcd::WCPtr CR(_mbp->second);

	utils u(C.get(),CR.get());
	
#if 0
	std::cout << "C[mbp]: " << *C << std::endl;//V
	std::cout << "CR[mbp]: " << *CR << std::endl;//V
	std::cout << "mbp (constants): " << constants::mbp << std::endl;//V
	std::cout << "mbp: " << EvtBdToXsllParameters::mbp << std::endl;//V
	std::cout << "muX: " << EvtBdToXsllParameters::muX << std::endl;//V
	std::cout << "C7new[3.0,muX]: " << u.getC7new(3.0,EvtBdToXsllParameters::muX) << std::endl;//V
	std::cout << "C9new[3.0,muX]: " << u.getC9new(3.0,EvtBdToXsllParameters::muX) << std::endl;//V
	std::cout << "C10new[3.0,muX]: " << u.getC10new(3.0,EvtBdToXsllParameters::muX) << std::endl;//V
	std::cout << "omega9[3.0]: " << utils::omega9(3.0) << std::endl;//V
	std::cout << "omega7[3.0]: " << utils::omega7(3.0,EvtBdToXsllParameters::muX) << std::endl;//V
	std::cout << "DiffDecayRate[3.0,mmu]: " << u.getDiffDecayRate(3.0,constants::mmu) << std::endl;//V
	std::cout << "DiffDecayRate[3.0,mme]: " << u.getDiffDecayRate(3.0,constants::mme) << std::endl;//V
#endif	
	
	double result = 0;
	double error = 0;
	const int nDivisions = 200;
	const double accuracyGoal = 1e-8;

	gsl_function F;
	F.function = &utils::integralFunction;
	F.params = &u;

	const double sMin = q2min/(EvtBdToXsllParameters::mbp*EvtBdToXsllParameters::mbp);
	const double sMax = q2max/(EvtBdToXsllParameters::mbp*EvtBdToXsllParameters::mbp);
	
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

double EvtBToVllConstraints::getDstar(const double q2, const std::vector<EvtComplex>& tensors) const{
	
	const EvtComplex tensA = tensors.at(QCDFactorisation::A);
	const EvtComplex tensB = tensors.at(QCDFactorisation::B);
	const EvtComplex tensC = tensors.at(QCDFactorisation::C);
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
inline EvtComplex EvtBToVllConstraints::getN(const double q2, const double beta, const double lambda) const{
	const double constant = (constants::GF*constants::GF*constants::alphaEM*constants::alphaEM)/
		(3*pow(2,10)*pow(constants::Pi,5)*constants::mB);
	return -constants::Vtstb * sqrt(constant * q2 * sqrt(lambda) * beta);
}

void EvtBToVllConstraints::getSpinAmplitudes(const double q2, std::vector<EvtComplex>* amps,\
		const bool isBbar, const double _scaleFactor) const{

	const bool _isBbar = fact.parameters->getisBbar();
	
	//get the amplitude tensors
	std::vector<EvtComplex> tensors(QCDFactorisation::NUMBER_OF_TENSORS);
	fact.parameters->setBbar(isBbar);
	fact.getTnAmplitudes(q2,constants::mB,constants::mKstar, &tensors);
	
	const EvtComplex tensA = tensors.at(QCDFactorisation::A);
	const EvtComplex tensB = tensors.at(QCDFactorisation::B);
	const EvtComplex tensC = tensors.at(QCDFactorisation::C);
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
	const EvtComplex N = getN(q2, beta, lambda);
	const double mB2 = constants::mB * constants::mB;
	const double mk = constants::mKstar/constants::mB;
	
	//calculate the form factors
	double ffA0, ffA1, ffA2, ffV;
	double ffT1, ffT2, ffT3;
	double xi1, xi2;
	
	fact.getFormFactors(q2,constants::mB,constants::mKstar,
			&ffA0,&ffA1,&ffA2,&ffV,
			&ffT1,&ffT2,&ffT3,
			&xi1,&xi2);
	
	//now calculate the spin amplitudes...
	const EvtComplex _ATL = sqrt(2*lambda)*N*constants::mB*cL;
	const EvtComplex _ATR = sqrt(2*lambda)*N*constants::mB*cR;
	
	const EvtComplex _APL = -sqrt(2)*N*constants::mB*aL;
	const EvtComplex _APR = -sqrt(2)*N*constants::mB*aR;
	
    const EvtComplex _A0L = ((constants::mB*mB2)*N*(-((1 - (mk*mk) - q2/mB2)*aL)/2. + lambda*bL))/(constants::mKstar*sqrt(q2));
    const EvtComplex _A0R = ((constants::mB*mB2)*N*(-((1 - (mk*mk) - q2/mB2)*aR)/2. + lambda*bR))/(constants::mKstar*sqrt(q2)); 
		
	const EvtComplex _AT = ((N*sqrt(lambda)*mB2)/(mk*sqrt(q2)))*
		(tensF + (mk*mk*tensG) - tensG - (q2/mB2)*tensH );
	const EvtComplex _AS = (N*sqrt(lambda)*tensS2);
	
	DEBUGPRINT("ATL: ", _ATL);
	DEBUGPRINT("ATR: ", _ATR);
	
	DEBUGPRINT("APL: ", _APL);
	DEBUGPRINT("APR: ", _APR);
	
	DEBUGPRINT("A0L: ", _A0L);
	DEBUGPRINT("A0R: ", _A0R);

	DEBUGPRINT("AT: ", _AT);
	DEBUGPRINT("AS: ", _AS);
	
	amps->at(ATL) = _scaleFactor*_ATL;
	amps->at(ATR) = _scaleFactor*_ATR;
	amps->at(APL) = _scaleFactor*_APL;
	amps->at(APR) = _scaleFactor*_APR;
	amps->at(A0L) = _scaleFactor*_A0L;
	amps->at(A0R) = _scaleFactor*_A0R;
	amps->at(AT) = _scaleFactor*_AT;
	amps->at(AS) = _scaleFactor*_AS;
	
	//reset back to the previous value
	fact.parameters->setBbar(_isBbar);
}

double EvtBToVllConstraints::getJ4(const double q2) const{
	EvtBToVllObservable obs(*this);
	obs.initParams(q2);
	return obs.getS(EvtBToVllBasisCoeffs::I4); 
}

double EvtBToVllConstraints::getJ5(const double q2) const{
	EvtBToVllObservable obs(*this);
	obs.initParams(q2);
	return obs.getS(EvtBToVllBasisCoeffs::I5); 
}

double EvtBToVllConstraints::getJ6(const double q2) const{
	EvtBToVllObservable obs(*this);
	obs.initParams(q2);
	return obs.getS(EvtBToVllBasisCoeffs::I6);
}

double EvtBToVllConstraints::getAFB(const double q2) const{
	EvtBToVllObservable obs(*this);
	obs.initParams(q2);
	//eqn 5.7 of Altmannhofer et al (2008)
	return (3/8.)*((2*obs.getS(EvtBToVllBasisCoeffs::I6s) + obs.getS(EvtBToVllBasisCoeffs::I6c)));
}

double EvtBToVllConstraints::getFL(const double q2) const{
	EvtBToVllObservable obs(*this);
	obs.initParams(q2);
	//eqn 5.9 of Altmannhofer et al (2008)
	return -obs.getS(EvtBToVllBasisCoeffs::I2c);
}

double EvtBToVllConstraints::getGamma(const double q2) const{
	EvtBToVllObservable obs(*this);
	obs.initParams(q2);
	//eqn 5.3 of Altmannhofer et al (2008)
	return obs.getDGamma();
}

const std::pair<double, double> EvtBToVllConstraints::getS4Zero() const{
	
	//hack to get const correctness 
	class s4utils{
	public:
		s4utils(const EvtBToVllConstraints* _calc):calc(_calc){
		}
		static double getValue(const double q2, void* params){
			const s4utils* c = (s4utils*)params;
			const double result = c->calc->getJ4(q2);
			return result;
		}
	private:
		const EvtBToVllConstraints* calc;
	};
	s4utils u(this);
	
    gsl_function F;
    F.function = &s4utils::getValue;
    F.params = &u;
    return findZero(&F);
}

const std::pair<double, double> EvtBToVllConstraints::getS5Zero() const{
	
	//hack to get const correctness 
	class s5utils{
	public:
		s5utils(const EvtBToVllConstraints* _calc):calc(_calc){
		}
		static double getValue(const double q2, void* params){
			const s5utils* c = (s5utils*)params;
			const double result = c->calc->getJ5(q2);
			return result;
		}
	private:
		const EvtBToVllConstraints* calc;
	};
	s5utils u(this);
	
    gsl_function F;
    F.function = &s5utils::getValue;
    F.params = &u;
    return findZero(&F);
}

const std::pair<double, double> EvtBToVllConstraints::getS6Zero() const{
	
	//hack to get const correctness 
	class s6utils{
	public:
		s6utils(const EvtBToVllConstraints* _calc):calc(_calc){
		}
		static double getValue(const double q2, void* params){
			const s6utils* c = (s6utils*)params;
			const double result = c->calc->getJ6(q2);
			return result;
		}
	private:
		const EvtBToVllConstraints* calc;
	};
	s6utils u(this);
	
    gsl_function F;
    F.function = &s6utils::getValue;
    F.params = &u;
    return findZero(&F);
}

double EvtBToVllConstraints::getAFBIntegral() const{
	
	//hack to get const correctness 
	class afb_utils{
	public:
		afb_utils(const EvtBToVllConstraints* _calc):calc(_calc){
		}
		static double getValue(const double q2, void* params){
			const afb_utils* c = (afb_utils*)params;
			const double result = c->calc->getAFB(q2);
			return result;
		}
	private:
		const EvtBToVllConstraints* calc;
	};
	afb_utils u(this);
	
    gsl_function F;
    F.function = &afb_utils::getValue;
    F.params = &u;
    //compare to Belle result
    return findRateIntegral(&F,q2min,q2max);
	
	
}

double EvtBToVllConstraints::getFLIntegral() const{
	
	//hack to get const correctness 
	class fl_utils{
	public:
		fl_utils(const EvtBToVllConstraints* _calc):calc(_calc){
		}
		static double getValue(const double q2, void* params){
			const fl_utils* c = (fl_utils*)params;
			const double result = c->calc->getFL(q2);
			return result;
		}
	private:
		const EvtBToVllConstraints* calc;
	};
	fl_utils u(this);
	
    gsl_function F;
    F.function = &fl_utils::getValue;
    F.params = &u;
    //compare to Belle result
    return findRateIntegral(&F,q2min,q2max);
	
}

double EvtBToVllConstraints::getS5Integral() const{

	//hack to get const correctness
	class s5_utils{
	public:
		s5_utils(const EvtBToVllConstraints* _calc):calc(_calc){
		}
		static double getValue(const double q2, void* params){
			const s5_utils* c = (s5_utils*)params;
			const double result = c->calc->getJ5(q2);
			return result;
		}
	private:
		const EvtBToVllConstraints* calc;
	};
	s5_utils u(this);

    gsl_function F;
    F.function = &s5_utils::getValue;
    F.params = &u;
    return findRateIntegral(&F,q2min,q2max);

}

const std::pair<double, double> EvtBToVllConstraints::findZero(gsl_function* F) const{
	
	//slight hack: recover from errors by setting result to an error value
	GSL_ERROR_HANDLER_CALLED = GSL_SUCCESS;
	gsl_error_handler_t* default_handler = gsl_set_error_handler(debug_handler);
	
    int status = GSL_FAILURE;
	int iter = 0;
	const int max_iter = 100;
    
    const gsl_root_fsolver_type *T = gsl_root_fsolver_brent;
    gsl_root_fsolver *s = gsl_root_fsolver_alloc(T);
    
    //the kinimatic range (almost!)
    double q2Min = 0.5;
    double q2Max = 15;

    double r = -1;//the error state
    gsl_root_fsolver_set(s,F,q2Min, q2Max);
    do{
        iter++;
        status = gsl_root_fsolver_iterate(s);
        r = gsl_root_fsolver_root(s);
        q2Min = gsl_root_fsolver_x_lower(s);
        q2Max = gsl_root_fsolver_x_upper(s);
        status = GSL_FAILURE;//reset the flag each time
        status = gsl_root_test_interval(q2Min, q2Max, 0, 0.0001);
    }while(status == GSL_CONTINUE && iter < max_iter && GSL_ERROR_HANDLER_CALLED == GSL_SUCCESS);
    
    double gradient = 0.0;
    if( status != GSL_SUCCESS || GSL_ERROR_HANDLER_CALLED != GSL_SUCCESS ){
    	r = -1.0; //outside the kinimatic range
    }else{
    	gradient = findZeroGradient(F,r);
    }
    gsl_root_fsolver_free(s);
    gsl_set_error_handler(default_handler);
	return std::make_pair(r,gradient);
}

double EvtBToVllConstraints::findZeroGradient(gsl_function* F, const double zero) const{
	
	double result = 0.0;
	double abserr = 1e6;
	
	gsl_deriv_central(F, zero, 0.2, &result, &abserr);
	return result;
}

double EvtBToVllConstraints::findRateIntegral(gsl_function* F, const double q2min, const double q2max) const{
	
	//hack to get const correctness 
	class rate_utils{
	public:
		rate_utils(const EvtBToVllConstraints* _calc, gsl_function* _F):
			calc(_calc),F(_F){
		}
		static double getGammaValue(const double q2, void* params){
			const rate_utils* c = (rate_utils*)params;
			const double result = c->calc->getGamma(q2);
			return result;
		}
		static double getRateValue(const double q2, void* params){
			const rate_utils* c = (rate_utils*)params;
			const double value = c->F->function(q2,c->F->params);
			const double result = c->calc->getGamma(q2)*value;
			return result;
		}
	private:
		const EvtBToVllConstraints* calc;
		const gsl_function* F;
	};
	rate_utils u(this,F);
	
	//first find the total width
    gsl_function F_rate;
    F_rate.function = &rate_utils::getGammaValue;
    F_rate.params = &u;
	
	double result = 0;
	double error = 0;
	const int nDivisions = 100;
	const double accuracyGoal = 1e-2;

	//the rate integral is the same every time, so only count once
	gsl_integration_workspace* w = 0;
		
	w = gsl_integration_workspace_alloc(nDivisions);
	const int intCode_rate = gsl_integration_qag(&F_rate, q2min, q2max, accuracyGoal, accuracyGoal,
			nDivisions, GSL_INTEG_GAUSS61, w, &result, &error);
	if (intCode_rate != 0) {
		report(WARNING,"EvtGen") << "Total Width: Numerical integration did not return cleanly."
		<< std::endl;
		report(WARNING,"EvtGen") << "Return code of the integration was " << intCode_rate
		<< std::endl;
		report(WARNING,"EvtGen") << "Result: " << result << " Error: " << error << std::endl;
	}
	const double rateIntegral_1_6 = result;
	gsl_integration_workspace_free(w);
	
	//now the observable
    gsl_function F_obs;
    F_obs.function = &rate_utils::getRateValue;
    F_obs.params = &u;
	
	w = gsl_integration_workspace_alloc(nDivisions);
	const int intCode_obs = gsl_integration_qag(&F_obs, q2min, q2max, accuracyGoal, accuracyGoal,
			nDivisions, GSL_INTEG_GAUSS61, w, &result, &error);
	if (intCode_obs != 0) {
		report(WARNING,"EvtGen") << "Numerical integration did not return cleanly."
				<< std::endl;
		report(WARNING,"EvtGen") << "Return code of the integration was " << intCode_obs
				<< std::endl;
		report(WARNING,"EvtGen") << "Result: " << result << " Error: " << error << std::endl;
	}
	gsl_integration_workspace_free(w);
	return result/rateIntegral_1_6;
	
}
