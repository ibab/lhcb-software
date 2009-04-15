#include "EvtGenModels/EvtBToVllConstraints.hh"

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
