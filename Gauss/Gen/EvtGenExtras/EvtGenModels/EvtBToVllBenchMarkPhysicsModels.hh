#ifndef BENCHMARKPHYSICSMODELS_H_
#define BENCHMARKPHYSICSMODELS_H_

#include "EvtBToVllPhysicsModel.hh"
#include "EvtBToVllWC.hh"
#include "EvtBToVllQCDUtils.hh"

#include "EvtGenBase/EvtReport.hh"

#include <utility>

namespace qcd{

class LeftHandedPhysicsModel : public IPhysicsModel {
public:
	/**
	 * Parent class for all physics models with no
	 * significant right-handed components. 
	 */
	qcd::WCPtr getRightWilsonCoefficientsMW() const;
	bool hasRightHandedCurrents() const{
		return false;
	}
};

class GenericModel : public IPhysicsModel {
public:
	static const std::string modelCommand;
	std::string getModelName() const{
			return "Generic_Model";
	}
	void setCommand(const std::string& _cmd){
		command = _cmd;
	}
	qcd::WCPtr getLeftWilsonCoefficientsMW() const;
	qcd::WCPtr getRightWilsonCoefficientsMW() const;
private:
	std::string command;
protected:
	void parseCommand(const WilsonCoefficients<WilsonType>* C,
			const WilsonCoefficients<WilsonType>* CR) const;
};

class SMPhysicsModel : public LeftHandedPhysicsModel {
public:	
	std::string getModelName() const{
		return "Standard_Model";
	}
	qcd::WCPtr getLeftWilsonCoefficientsMW() const;
};

class NegC7PhysicsModel : public LeftHandedPhysicsModel {
public:	
	std::string getModelName() const{
		return "NegC7_Model";
	}
	qcd::WCPtr getLeftWilsonCoefficientsMW() const;
};

class LHTPhysicsModel : public LeftHandedPhysicsModel {
public:	
	std::string getModelName() const{
			return "LHT_Model";
		}
	qcd::WCPtr getLeftWilsonCoefficientsMW() const;
};

class UEDPhysicsModel : public LeftHandedPhysicsModel {
public:	
	std::string getModelName() const{
			return "UED_Model";
		}
	qcd::WCPtr getLeftWilsonCoefficientsMW() const;

};

class MSSMLowTanBeta : public LeftHandedPhysicsModel {
/** 
 * Point in Parameter space is...
 * MH=650.;
 * MU=492.83;
 * M2=489.68;
 * Mg=1000.;
 * beta=ArcTan[2.56];
 * mug=1000.;
 * MUL2=Transpose[Vckm].DiagonalMatrix[{339.97,128.18,393.52}]^2.Vckm;
 * MUR2=DiagonalMatrix[{232.08,351.41,234.77}]^2;
 * AtU=DiagonalMatrix[{-419.3,525.64,-540.81}];
 * MD2=DiagonalMatrix[{339.97,128.18,393.52,339.97,128.18,393.52}]^2;
 * 
 * Point Corresponding to P2 in hep-ph/0409293, but mH=600;
 */
public:	
	std::string getModelName() const{
			return "MSSM_Low_Tan_Beta_Model";
		}
	qcd::WCPtr getLeftWilsonCoefficientsMW() const;
};

class MSSMHighTanBeta : public LeftHandedPhysicsModel {
public:	
	std::string getModelName() const{
			return "MSSM_High_Tan_Beta_Model";
		}
	qcd::WCPtr getLeftWilsonCoefficientsMW() const;
};

class FBMSSMPhysicsModel : public IPhysicsModel {
public:	
	std::string getModelName() const{
			return "FBMSSM_Model";
		}
	qcd::WCPtr getLeftWilsonCoefficientsMW() const;
	qcd::WCPtr getRightWilsonCoefficientsMW() const;
};	

}
#endif /*BENCHMARKPHYSICSMODELS_H_*/
