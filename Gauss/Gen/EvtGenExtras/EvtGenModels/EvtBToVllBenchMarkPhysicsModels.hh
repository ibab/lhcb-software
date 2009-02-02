#ifndef BENCHMARKPHYSICSMODELS_H_
#define BENCHMARKPHYSICSMODELS_H_

#include "EvtBToVllPhysicsModel.hh"
#include "EvtBToVllWC.hh"
#include "EvtBToVllQCDUtils.hh"

#include "EvtGenBase/EvtReport.hh"

namespace qcd{

class GenericModel : public IPhysicsModel {
public:
	static const std::string modelName;
	static const std::string modelCommand;
	
	std::string getModelName() const{
			return modelName;
	}
	void setCommand(const std::string& _cmd){
		command = _cmd;
	}
	qcd::WCPtr getWilsonCoefficientsMW() const;
private:
	std::string command;
};

class TestPhysicsModel : public IPhysicsModel {
public:	
	std::string getModelName() const{
			return "Test_Model";
	}
	qcd::WCPtr getWilsonCoefficientsMW() const;
};

class SMPhysicsModel : public IPhysicsModel {
public:	
	std::string getModelName() const{
		return "Standard_Model";
	}
	qcd::WCPtr getWilsonCoefficientsMW() const;
};

class NegC7PhysicsModel : public IPhysicsModel {
public:	
	std::string getModelName() const{
		return "NegC7_Model";
	}
	qcd::WCPtr getWilsonCoefficientsMW() const;
};

class LHTPhysicsModel : public IPhysicsModel {
public:	
	std::string getModelName() const{
			return "LHT_Model";
		}
	qcd::WCPtr getWilsonCoefficientsMW() const;
};

class UEDPhysicsModel : public IPhysicsModel {
public:	
	std::string getModelName() const{
			return "UED_Model";
		}
	qcd::WCPtr getWilsonCoefficientsMW() const;

};

class MSSMLowTanBeta : public IPhysicsModel {
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
	qcd::WCPtr getWilsonCoefficientsMW() const;
};

class MSSMHighTanBeta : public IPhysicsModel {
public:	
	std::string getModelName() const{
			return "MSSM_High_Tan_Beta_Model";
		}
	qcd::WCPtr getWilsonCoefficientsMW() const;
};


}
#endif /*BENCHMARKPHYSICSMODELS_H_*/
