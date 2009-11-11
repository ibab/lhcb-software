#ifndef PARAMETERISEDFFCALC_H_
#define PARAMETERISEDFFCALC_H_

#include "EvtGenModels/EvtBToVllConstants.hh"

#include <cassert>
#include <cmath>
#include <memory>
#include <string>

namespace bdkszmm{

enum PARAMETERIZATIONS {BALL07PRIVATE = 0, ALIBALL00 = 1, BALL05 = 2, BFS05 = 3};
enum FFLABELS {V,A0,A1,A2,T1,T2,T3,XIL,XIT};

class FFCalc{
public:

	FFCalc():
		mB(constants::mB),mKstar(constants::mKstar){
	}

	// all the form factors to calculate
	virtual double getV(const double q2) const;
	virtual double getA0(const double q2) const;
	virtual double getA1(const double q2) const;
	virtual double getA2(const double q2) const;
	virtual double getT1(const double q2) const;
	virtual double getT2(const double q2) const;
	virtual double getT3(const double q2) const;
	
	//uses eqn (3) of Beneke et al, Eur.Phys.J C41, 173-188 (2005)
	virtual double getXiL(const double q2) const{
		const double E = getE(q2);
		return ((mB + mKstar)/(2*E))*getA1(q2) -
			((mB - mKstar)/mB)*getA2(q2);
	}
	virtual double getXiT(const double q2) const{
		return (mB/(mB + mKstar))*getV(q2);
	}
	
	virtual std::string getName() const = 0;
	
	void setMasses(const double _mB, const double _mKstar){
		mB = _mB;
		mKstar = _mKstar;
	}

protected:	
	virtual double calc(const double*, const double) const{
		assert(false);
		return 0.0;
	}
	virtual const double* getCoeffs(const FFLABELS) const{
		assert(false);
		return 0;
	}
	
	double getE(const double q2) const{
		return ((mB*mB) - q2)/(2*mB);
	}
	
	double mB;
	double mKstar;


};
//get a form factor model to use
std::auto_ptr<FFCalc> getFFModel(const PARAMETERIZATIONS model = BALL07PRIVATE);


class Ball07Private: public FFCalc{
/**
 * parameterised version of FF from P. Ball
 * These are the same (within errors) as those used
 * in arXiv:0811.1214, but not absolutely the same...
 */	
public:
	
	virtual double getXiL(const double q2) const;
	virtual double getXiT(const double q2) const;
	
	virtual std::string getName() const{
		return "Ball '07";
	}
	
protected:	
	virtual double calc(const double* coeffs, const double q2) const;
	virtual const double* getCoeffs(const FFLABELS index) const;

private:

	static const double _VC[7];
	static const double _A0C[7];
	static const double _A1C[7];
	static const double _A2C[7];
	static const double _T1C[7];
	static const double _T2C[7];
	static const double _T3C[7];
	static const double _XiLC[7];
	static const double _XiTC[7];
	
};

class AliBall00: public FFCalc{
/**
 * Parameterised version of FF from
 * Ali, Ball, Handoko, and Hiller
 * Phys.Rev. D61, 074024  (2000)
 * 
 * We take the values from Table III
 * to be consistent with EvtbTosllBallFF.
  */	
	
public:
	virtual std::string getName() const{
		return "Ali '01";
	}
protected:	
	virtual double calc(const double* coeffs, const double q2) const;
	virtual const double* getCoeffs(const FFLABELS index) const;

private:
	static const double _VC[3];
	static const double _A0C[3];
	static const double _A1C[3];
	static const double _A2C[3];
	static const double _T1C[3];
	static const double _T2C[3];
	static const double _T3C[3];
	
};

class Ball05: public FFCalc{
/**
 * Parameterised version of FF from
 * Ball and Zwicky, Phys. Rev. D71, 014029 (2005)
 */	

public:
	virtual std::string getName() const{
		return "Ball '05";
	}
	
protected:	
	virtual double calc(const double* coeffs, const double q2) const;
	virtual const double* getCoeffs(const FFLABELS index) const;

private:
	static const double _VC[5];
	static const double _A0C[5];
	static const double _A1C[5];
	static const double _A2C[5];
	static const double _T1C[5];
	static const double _T2C[5];
	static const double _T3C[5];
	
	double calc_0(const double* coeffs, const double q2) const{
		const double r1 = coeffs[1];
		const double mR2 = coeffs[2];
		const double r2 = coeffs[3];
		const double mf2 = coeffs[4];
		
		return (r1/(1 - (q2/mR2))) + (r2/(1 - (q2/mf2))); 
	}
	double calc_1(const double* coeffs, const double q2) const{
		const double r1 = coeffs[1];
		const double r2 = coeffs[3];
		const double mf2 = coeffs[4];
		const double fact = (1 - (q2/mf2)); 
		return (r1/fact) + (r2/(fact*fact));
	}
	double calc_2(const double* coeffs, const double q2) const{
		const double r2 = coeffs[3];
		const double mf2 = coeffs[4];
		return (r2/(1 - (q2/mf2)));
	}
	
};


class BFS: public FFCalc{
public:
	
	virtual double getV(const double q2) const;
	virtual double getA0(const double q2) const;
	virtual double getA1(const double q2) const;
	virtual double getA2(const double q2) const;
	virtual double getT1(const double q2) const;
	virtual double getT2(const double q2) const;
	virtual double getT3(const double q2) const;
	
	//the following are from BFS 05 eqn (5)
	virtual double getXiL(const double q2) const{
		const double E = getE(q2);
		return isZero(q2) ? _xiL0 : ((mB + mKstar)/(2*E))*getA1(q2) -
		((mB - mKstar)/mB)*getA2(q2);
	}
	virtual double getXiT(const double q2) const{
		return isZero(q2) ? _xiT0 : (mB/(mB + mKstar))*getV(q2);
	}
	
	virtual std::string getName() const{
		return "Beneke '05";
	}
	
private:	
	static const double _xiT0;
	static const double _xiL0;
	
	static const double _VC[3];
	static const double _A0C[3];
	static const double _A1C[3];
	static const double _A2C[3];
	static const double _T1C[3];
	static const double _T2C[3];
	static const double _T3C[3];

	double getF(const double q2, const double _C[3]) const{
		const double mB2 = mB*mB;
		return _C[0]/(1 - ((_C[1]*q2)/mB2) +
				((_C[2]*q2*q2)/(mB2*mB2)));
	}
	
	bool isZero(const double q2) const{
		return (fabs(q2) < 1e-3); 
	}
};

}

#endif /*PARAMETERISEDFFCALC_H_*/
