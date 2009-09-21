#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "EvtGenModels/EvtBToVllEvolveWC.hh"
#include "EvtGenModels/EvtBToVllConstants.hh"
#include "EvtGenBase/EvtMathematicaFn.hh"
#include "EvtGenModels/EvtBToVllQCDUtils.hh"

/**
 * Evolve the Wilson coefficients using
 * Gorbahn, Haisch, Nucl. Phys. B 713 (2005) 291-332
 */

qcd::EvtBToVllEvolveWC10D::EvtBToVllEvolveWC10D(const WilsonCoefficients<WilsonType>& _c,
		const WilsonCoefficients<WilsonType>& _cnp,
		const WilsonCoefficients<WilsonType>& _cr):
	C(_c),CNP(_cnp),CR(_cr){
}

/**
 * Wilson Coefficient format is
 * 
 * Cm 1-8 are first eight numbers in vector
 * C9 is the 9th Number
 * C10 is the 10th Number
 * CQ1 is the 11th Number
 * CQ2 is the 12th Number
 * 
 * The same goes for the right-handed versions, although
 * currently the code only deals with C'_{7-10}.
 */

qcd::WilsonType qcd::EvtBToVllEvolveWC10D::runC7(const qcd::WilsonType C7h, const qcd::WilsonType C8h, const argument_type& scale){
	switch(scale){
	case MU_MW:
	{
		DEBUGPRINT("RunC7_MW: ", C7h);
		return C7h;
	}
	case MU_MB:
	case MU_MBP:
	{
		const qcd::WilsonType result = 0.6277390019256378*C7h + 0.10032280455435938*C8h;
		DEBUGPRINT("RunC71: ", result);
		DEBUGPRINT("C7h: ", C7h);
		DEBUGPRINT("C8h: ", C8h);
		return result;
	}
	case MU_H:
	{
		const qcd::WilsonType result = 0.5074850407598744*C7h + 0.11974496886416888*C8h;
		DEBUGPRINT("RunC73: ", result);
		DEBUGPRINT("C7h: ", C7h);
		DEBUGPRINT("C8h: ", C8h);
		return result;
	}
	default:
	{
		DEBUGPRINT("RunC7_DEF: ", C7h);
		return C7h;//no running
	}
	}
}
qcd::WilsonType qcd::EvtBToVllEvolveWC10D::runC8(const qcd::WilsonType C8h, const argument_type& scale){
	switch(scale){
	case MU_MW:
	{
		DEBUGPRINT("RunC8_MW: ", C8h);
		return C8h;
	}
	case MU_MB:
	case MU_MBP:
	{
		const qcd::WilsonType result = 0.6653600536335226*C8h;
		DEBUGPRINT("RunC81: ", result);
		DEBUGPRINT("C8h: ", C8h);
		return result;
	}
	case MU_H:
	{
		const qcd::WilsonType result = 0.5523894040839378*C8h;
		DEBUGPRINT("RunC83: ", result);
		DEBUGPRINT("C8h: ", C8h);
		return result;
	}
	default:
	{
		DEBUGPRINT("RunC8_DEF: ", C8h);
		return C8h;//no running
	}
	}
}

/**
 * Method that actually does the scaling.
 */
qcd::EvtBToVllEvolveWC10D::result_type qcd::EvtBToVllEvolveWC10D::operator()(const qcd::EvtBToVllEvolveWC10D::argument_type& scale){
	
	//these large arrays are just extracted from mathematica. They are calculated from the
	//three loop anomalous dimension matrix
	const double gam0_data[dimension] = {
				-15.333333333333334, -15.333333329999999,
				-13.790720905057974, -7.999999999999996,
				-6.485825798068867, 6.265491004149409,
				-6., -4.666666666666667,
				3.9999999999999996, 2.2332779211996603
				};
	
	const double V_data[dimension][dimension] = {
			{0,0,0,-0.9409878113450303,0,0,0,0,-0.8282780998098401,0},
			{0,0,0,0.3136626037816766,0,0,0,0,-0.5521853998732267,0},
			{0,0,0.15365781326844563,-0.03485140042018831,0.809648459481775,
					-0.9116406586088899,0,0,-0.02629454285111652,-0.7258718754819949},
			{0,0,0.9416338022084993,0.10455420126055011,
			    -0.2351691786072425,0.32762703694096873,0,0,
			    -0.03944181427664873,0.6838800872511364},
			{0,0,-0.006938972071662472,0.00871285010504693,
			    -0.13557230718505744,0.129371172860537,0,0,
			    0.00657363571277672,0.03686183998099939},
			{0,0,-0.07263235909898481,-0.026138550315139515,
			    0.22135905053575092,0.14961642825054225,0,0,
			    0.009860453569163968,-0.0498653937181326},
			{0,0,0,0,0,0,0.9363291775690445,1.,0,0},
			{0,0,0,0,0,0,0.35112344158839165,0,0,0},
			{1.,0,-0.2905020654629312,0.05702956432394024,
			    0.47094070726835113,0.14983276766672252,0,0,
			    0.08160375367584675,0.039657204997072995},
			{0,1.,0,0,0,0,0,0,0,0}
	};
	
	const double VInv_data[dimension][dimension] = {
			{0.11836177258011768,-0.046881121481155634,
			    0.47289788548832057,0.14054361047889016,3.5745546556130554,
			    -1.5186169423711888,0,0,1.,0},
			{0,0,0,0,0,0,0,0,0,1.},
			{0.05972704970547585,-0.14183442739855268,
			    1.142913945621245,0.9294558048873438,5.49818694160623,
			    0.1744800210763717,0,0,0,0},
			{-0.7084753475326593,1.062713021298989,0,0,0,0,0,0,0,0},
			{-0.061366169238878915,0.07345372526831809,
			    -0.24772495569926223,0.194989473417782,-5.1228762196565825,
			    2.493259223870645,0,0,0,0},
			{0.014192364080404515,0.07230371312052847,
			    0.3177353025183256,0.21203242136881953,6.155388277025298,
			    2.8329981583443993,0,0,0,0},
			{0,0,0,0,0,0,0,2.8480012484391772,0,0},
			{0,0,0,0,0,0,1.,-2.666666666666667,0,0},
			{-0.4024413218336467,-1.207323965500941,0,0,0,0,0,0,0,0},
			{-0.02503536048498868,-0.04619035883068274,
			    -1.811081178337563,0.14795131295327013,-12.280943760100726,
			    -0.7400790922790951,0,0,0,0}
	};
	
	const double G1_data[dimension][dimension] = {
			{-77.33333333333333,0,-56.12434155904813,
					-6.055174076131168,20.15790483801497,-105.22949665865576,0,
					0,-6.906678093595069,-0.49413577430935707},
		    {0,-77.33333333333333,0,0,0,0,0,0,0,0},
		    {0,0,-269.6822736628382,-26.512751253127565,
		    		167.18126020927266,-186.62224349047335,0,0,
		    		-3.4516826464479378,-9.603584705187998},
		    {0,0,0,-8.222222222222221,0,0,0,0,-5.868146146164667,0},
		    {0,0,-4.297238597230889,-4.725500050571828,
		    		96.35022935966593,-24.25112544373212,0,0,-2.049989608508575,
		    -14.889012794034533},
		    {0,0,18.244235294235885,
		    		22.929304615450338,-215.24141232524852,74.82389304304905,0,
		    		0,0.4628736483369096,43.60549134188355},
		    {0,0,-19.916055653379374,10.322710518281209,
		    		-170.7206034676889,120.19423524108075,73.14814814814815,0,
		    		1.1981902538731044,10.43695108943377},
		    {0,0,21.957967708787287,-6.552063278995629,
		    		161.11093109645194,-130.87943696859023,12.172279308397577,
		    		96.29629629629629,-4.470072284687747,-11.509474533263523},
		    {0,0,0,-31.052775047417633,0,0,0,0,-40.55555555555556,0},
		    {0,0,-35.12317860079664,4.69860654684328,
		    		-190.08277565032589,289.3916019371436,0,0,8.653421566435089,
		    		77.43201957288045}
		    };
	
	const double G2_data[dimension][dimension] = {
			{-361.81481481481484,0,-719.7673998684838,
			    -403.38660194092404,3313.624438341266,-2035.9254962274683,0,
			    0,126.32993989721271,-558.726907252501},
			{0,-361.81481481481484,0,0,0,0,0,0,0,0},
			{0,0,-2266.8507438950032,-1160.5852033295387,
			    13557.371563272314,-7754.451343333125,0,0,
			    -227.3847407091016,-1926.8950508027642},
			{0,0,0,864.2496243120679,0,0,0,0,-438.611323613886,0},
			{0,0,191.4145261202803,-44.2032814070324,
			    2070.4055258459953,-1218.5986879219972,0,0,
			    -151.7479069849946,-193.2893005903536},
			{0,0,548.6511729010886,349.30071383766744,
			    -2744.377547041093,-1997.8555502385295,0,0,
			    -20.132444383718536,418.3606001848423},
			{0,0,76.52075002208085,525.0105877928984,
			    -6718.548213771075,1960.8162841380129,-140.27740679831027,0,
			    91.88303702370379,883.7268536021514},
			{0,0,-86.93101486684395,-450.20393666493874,
			    5939.340528784292,-1822.6955221998744,1010.9738224443854,
			    920.1169079162315,-90.24335800097738,-856.5567086422225},
			{0,0,0,-364.8490683416783,0,0,0,0,-947.312227913954,0},
			{0,0,562.3482851864076,812.818751995618,
			    -10713.586058257375,1806.5682135475108,0,0,
			    122.99599496165209,1594.2222410300963}
	};
	
	/**
	 * The following is based mostly on Appendix C of hep-ph/0106067v2,
	 * using extra calculations based on hep-ph/9612313v3.
	 */
	assert(C.getOperatorBasis() == 12); //all WCs should be included

	//most values stay the same, so copy
	qcd::WilsonCoefficients<WilsonType> CmwL(C);
		CmwL.setOperatorBasis(dimension);
		for(unsigned int i = 1; i <= CmwL.getDimension(); ++i){
			CmwL(i) = C(i);
	}
	assert(CmwL.getOperatorBasis() == dimension);
	qcd::WilsonCoefficients<WilsonType> CmwR(C);
			CmwR.setOperatorBasis(dimension);
			for(unsigned int i = 1; i <= CmwR.getDimension(); ++i){
				CmwR(i) = CR(i);
	}
	assert(CmwR.getOperatorBasis() == dimension);

	const double mu = WilsonCoefficients<WilsonType>::getScaleValue(scale);
	const unsigned int nfl = 5;
	const unsigned int nf = nfl;
	const unsigned int nfl_MW = 5;
	
	const double as1MW = qcd::as1(constants::MW,nf);
	const double _B0 = qcd::B0(nf);
	const double _B1 = qcd::B1(nf);
	const double _B2 = qcd::B2(nf);
	const double et = qcd::alpha_s(mu,nfl_MW)/qcd::alpha_s(constants::MW,nfl_MW);
	
	DEBUGPRINT("as1MW: ", as1MW);//V
	DEBUGPRINT("B0[nf]: ", _B0);//V
	DEBUGPRINT("B1[nf]: ", _B1);//V
	DEBUGPRINT("B2[nf]: ", _B2);//V
	DEBUGPRINT("et: ", et);//V
	DEBUGPRINT("scale: ", scale);
	DEBUGPRINT("mu: ", mu);
	
	class inner{
	
	public:

		/** Simple implementation of the Mathematica function Chop
		 * with a delta of 1E-10. Returns 0.0 for numbers smaller 
		 * than this.
		 */
		static double matrixChop(double val, int, int){
			return (fabs(val) > 1E-10) ? val : 0.0; 
		}
	};

	/**
	 * The following is taken from 
	 * Gorbahn, M. and Haisch, U., Nuc. Phys. B 713 (2005) 291-332
	 * I'll refer to this as Ref A in this file.
	 */
		
	//extracted from \gamma^{(0)} - see page 295 of Ref. A
	StdMatrixPtr V = qcd::makeMatrix<dimension,dimension>(V_data);
	StdMatrixPtr VInv = qcd::makeMatrix<dimension,dimension>(VInv_data);
	
	//make the matrix a11 - from eqna (8) and (9) in Ref. A
	DiagMatrixPtr a11(new DiagMatrix(dimension));
	for(unsigned int i = 0; i < dimension; ++i){
		(*a11)[i][i] = pow(et,(-(1/(2*_B0))*gam0_data[i]));
	}
	DEBUGPRINT("a11: ", *a11);//V
	
	//eqn (8) of Ref. A
	StdMatrix U0 = (*V)*(*a11)*(*VInv);
	U0.apply(inner::matrixChop);//filter out very small numbers
	DEBUGPRINT("U0: ", U0);//V
	
	//G{n} here is the order of \gamma^{(n)}
	StdMatrixPtr G1 = qcd::makeMatrix<dimension,dimension>(G1_data);
	StdMatrixPtr G2 = qcd::makeMatrix<dimension,dimension>(G2_data);
	
	//now use eqn (11) from Ref. A. Vars are S(1) and S(2) there
	StdMatrixPtr H1(new StdMatrix(dimension,dimension,0));
	StdMatrixPtr H2(new StdMatrix(dimension,dimension,0));
	
	//we need to calculate H1 completely before H2 can be done
	for(unsigned int i = 0; i < dimension; i++){
		for(unsigned int j = 0; j < dimension; j++){
			(*H1)[i][j] = (KroneckerDelta(i,j)*(gam0_data[i])*(_B1/(2*_B0*_B0))) -
				(*G1)[i][j]/(2*_B0 + gam0_data[i] - gam0_data[j]);
		}
	}
	//now H2
	for(unsigned int i = 0; i < dimension; i++){
		for(unsigned int j = 0; j < dimension; j++){
			double secondTerm = 0;
			for(unsigned int k = 0; k < dimension; k++){
				secondTerm += ((2*_B0 + gam0_data[i] - gam0_data[k])/(4*_B0 + gam0_data[i] - gam0_data[j]))*
					( (*H1)[i][k]*(*H1)[k][j] - ((_B1*(*H1)[i][j]*KroneckerDelta(j,k))/_B0) );
			}
			(*H2)[i][j] = (KroneckerDelta(i,j)*gam0_data[i]*(_B2/(4*_B0*_B0))) + secondTerm -
				(*G2)[i][j]/(4*_B0 + gam0_data[i] - gam0_data[j]);
			
		}
	}
	H1->apply(inner::matrixChop);
	H2->apply(inner::matrixChop);
	
	StdMatrix J1 = (*V)*(*H1)*(*VInv);
	StdMatrix J2 = (*V)*(*H2)*(*VInv);
	
	DEBUGPRINT("H1: ", *H1);//V
	DEBUGPRINT("H2: ", *H2);//V
	DEBUGPRINT("J1: ", J1);//V
	DEBUGPRINT("J2: ", J2);//V
	
	const double as1 = qcd::as1(mu,nfl);
	const StdMatrixPtr I10(new StdMatrix(dimension,dimension,1));//Identity matrix (10x10)
	
	//Eqn (7) of Ref. A
	const StdMatrix K = *I10 + as1*J1 + as1*as1*J2;
	const StdMatrix KInv = *I10 - as1MW*J1 - (as1MW*as1MW)*(J2 - J1*J1);
	DEBUGPRINT("K: ", K);//V
	DEBUGPRINT("KInv: ", KInv);//V
	
	//eqn (6) of Ref. A
	const StdMatrix U = K*U0*KInv;
	DEBUGPRINT("U: ", U);//V
	DEBUGPRINT("CmwL: ", CmwL);//V

	const qcd::WilsonCoefficients<qcd::WilsonType> CmL = U * CmwL;
	DEBUGPRINT("CmL: ", CmL);//V
		
	WilsonCoefficients<WilsonType>* C_barred = new WilsonCoefficients<WilsonType>(scale,12,dimension);
	WilsonCoefficients<WilsonType>* CR_barred = new WilsonCoefficients<WilsonType>(scale,12,dimension);
	DEBUGPRINT("RunC7[c7,c8]: ", runC7(CNP(7),CNP(8),scale));//V
	DEBUGPRINT("RunC8[c7]: ", runC8(CNP(8),scale));//V
	
	const EvtComplex Ceff_7 = (CmL(7)/as1 - CmL(3)/3. - 4*CmL(4)/9. - 20*CmL(5)/3. - 80*CmL(6)/9.);
	const EvtComplex Ceff_8 = (CmL(8)/as1 + CmL(3) - CmL(4)/6. + 20*CmL(5) - 10*CmL(6)/3.);
	
	//this is the final result
	//see Appendix A of hep-ph/0106067
	(*C_barred)(1) = 0.5*CmL(1);
	(*C_barred)(2) = CmL(2) - (1/6.)*CmL(1);
	(*C_barred)(3) = CmL(3) - (1/6.)*CmL(4) + 16*CmL(5) - (8/3.)*CmL(6);
	(*C_barred)(4) = 0.5*CmL(4) + 8*CmL(6);
	(*C_barred)(5) = CmL(3) - (1/6.)*CmL(4) + 4*CmL(5) - (2/3.)*CmL(6);
	(*C_barred)(6) = 0.5*CmL(4) + 2*CmL(6);
	(*C_barred)(7) = Ceff_7 + runC7(CNP(7),CNP(8),scale);
	(*C_barred)(8) = Ceff_8 + runC8(CNP(8),scale);
	(*C_barred)(9) = (CmL(9)/as1) + CNP(9);
	(*C_barred)(10) = (CmL(10)/as1) + CNP(10);
	(*C_barred)(11) = CNP(11);
	(*C_barred)(12) = CNP(12);
	
	DEBUGPRINT("Ceff[7]: ",Ceff_7);
	DEBUGPRINT("Ceff[8]: ", Ceff_8);
	DEBUGPRINT("Cb[9]: ", (CmL(9)/as1));
	DEBUGPRINT("Cb[10]: ", (CmL(10)/as1));

	//now the right handed terms - we neglect CR(1-6) as they will be v. small
	const double mb_scale = (scale == MU_MBP) ? WilsonCoefficients<WilsonType>::getScaleValue(scale) : constants::mb;
	(*CR_barred)(7) = (constants::ms/mb_scale)*(*C_barred)(7) + runC7(CR(7),CR(8),scale);
	(*CR_barred)(8) = (constants::ms/mb_scale)*(*C_barred)(8) + runC8(CR(8),scale);
	(*CR_barred)(9) = CR(9);
	(*CR_barred)(10) = CR(10);

	DEBUGPRINT("End of Evolution: C_barred: ", *C_barred);
	DEBUGPRINT("End of Evolution: CR_barred: ", *CR_barred);
	return new qcd::WilsonPair(C_barred,CR_barred);
}




