#ifndef IPHYSICSMODEL_HH_
#define IPHYSICSMODEL_HH_

#include <memory>
#include <string>

#include "EvtGenModels/EvtBToVllWC.hh"

namespace qcd{

class IPhysicsModel {

public:
	
	/** A Unique model name. Should have no spaces. */ 
	virtual std::string getModelName() const = 0;
	/** A set of the left handed Wilson Coefficients for the model*/
	virtual qcd::WCPtr getLeftWilsonCoefficientsMW() const = 0;
	/** A set of differences from the SM wilson coefficients */
	virtual qcd::WCPtr getLeftNewPhysicsDeltasMW() const = 0;

	/** A set of the right handed Wilson Coefficients for the model*/
	virtual qcd::WCPtr getRightWilsonCoefficientsMW() const = 0;
	/** The decay file command */
	virtual void setCommand(const std::string&){
		//default is to do nothing
	}
	virtual ~IPhysicsModel(){}
	
};

enum dimensions {DIMENSION8 = 8, DIMENSION10 = 10};

}

#endif /*IPHYSICSMODEL_HH_*/
