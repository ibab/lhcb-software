#ifndef IPHYSICSMODEL_HH_
#define IPHYSICSMODEL_HH_

#include <memory>
#include <string>

#include "EvtGenModels/EvtBToVllWC.hh"

namespace qcd{

typedef std::auto_ptr<const WilsonCoefficients<WilsonType> > WCPtr;

class IPhysicsModel {

public:
	
	/** A Unique model name. Should have no spaces. */ 
	virtual std::string getModelName() const = 0;
	/** A set of Wilson Coefficients for the model*/
	virtual qcd::WCPtr getWilsonCoefficientsMW() const = 0;
	/** The decay file command */
	virtual void setCommand(const std::string&){
		//default is to do nothing
	}
	virtual ~IPhysicsModel(){}
	
};

enum dimensions {DIMENSION8 = 8, DIMENSION10 = 10};

}

#endif /*IPHYSICSMODEL_HH_*/
