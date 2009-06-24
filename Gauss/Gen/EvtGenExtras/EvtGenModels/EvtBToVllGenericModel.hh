#ifndef EVTBTOVLLGENERICMODEL_HH_
#define EVTBTOVLLGENERICMODEL_HH_

#include "EvtBToVllPhysicsModel.hh"
#include "EvtBToVllWC.hh"
#include "EvtBToVllQCDUtils.hh"

namespace qcd{

class GenericModel : public SMPhysicsModel {
public:
	static const std::string modelCommand;
	std::string getModelName() const{
			return "Generic_Model";
	}
	void setCommand(const std::string& _cmd){
		command = _cmd;
	}
	bool hasRightHandedCurrents() const{
		return true;
	}
	qcd::WCPtr getLeftNewPhysicsDeltasMW() const;
	qcd::WCPtr getRightWilsonCoefficientsMW() const;
private:
	std::string command;
protected:
	void parseCommand(const WilsonCoefficients<WilsonType>* C,
			const WilsonCoefficients<WilsonType>* CR) const;
};

}

#endif /*EVTBTOVLLGENERICMODEL_HH_*/
