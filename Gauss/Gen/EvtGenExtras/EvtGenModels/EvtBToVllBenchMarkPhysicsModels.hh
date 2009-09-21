#ifndef BENCHMARKPHYSICSMODELS_H_
#define BENCHMARKPHYSICSMODELS_H_

#include "EvtBToVllPhysicsModel.hh"
#include "EvtBToVllWC.hh"
#include "EvtBToVllQCDUtils.hh"

namespace qcd{

class SMPhysicsModel : public IPhysicsModel{
public:	
	std::string getModelName() const{
		return "Standard_Model";
	}
	qcd::WCPtr getLeftWilsonCoefficientsMW() const;
	qcd::WCPtr getLeftNewPhysicsDeltasMW() const{
		return getZeroWCs();
	}
	qcd::WCPtr getRightWilsonCoefficientsMW() const{
		return getZeroWCs();
	}
protected:
	qcd::WCPtr getZeroWCs() const;
};

class LHTPhysicsModel : public SMPhysicsModel {
public:	
	std::string getModelName() const{
			return "LHT_Model";
	}
	qcd::WCPtr getLeftNewPhysicsDeltasMW() const;
};

class UEDPhysicsModel : public SMPhysicsModel {
public:	
	std::string getModelName() const{
			return "UED_Model";
	}
	qcd::WCPtr getLeftNewPhysicsDeltasMW() const;
};

class FBMSSMPhysicsModel : public SMPhysicsModel {
public:	
	std::string getModelName() const{
			return "FBMSSM_Model";
	}
	qcd::WCPtr getLeftNewPhysicsDeltasMW() const;
	qcd::WCPtr getRightWilsonCoefficientsMW() const;
};	

class GMSSMPhysicsModel : public SMPhysicsModel {
public:	
	std::string getModelName() const{
			return "GMSSM_Model";
	}
	qcd::WCPtr getLeftNewPhysicsDeltasMW() const;
	qcd::WCPtr getRightWilsonCoefficientsMW() const;
};

}
#endif /*BENCHMARKPHYSICSMODELS_H_*/
