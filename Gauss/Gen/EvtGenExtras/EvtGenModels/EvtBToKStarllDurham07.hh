#ifndef EVTBTOKSTARLLDURHAM_HH
#define EVTBTOKSTARLLDURHAM_HH

#include <map>
#include <string>

#include "EvtGenBase/EvtDecayAmp.hh"

#include "EvtGenModels/EvtBToVllPhysicsModel.hh"
#include "EvtGenModels/EvtBToVllWC.hh"
#include "EvtGenModels/EvtBToKstarllQCDFactorisation.hh"
#include "EvtGenModels/EvtBToVllParameterisedFFCalc.hh"

class EvtBToKStarllDurham07 : public EvtDecayAmp {

public:

	EvtBToKStarllDurham07();
	virtual ~EvtBToKStarllDurham07();

	virtual std::string getName() {
		return "BTOKSTARLLDURHAM07";
	}
	EvtDecayBase* clone();

	virtual void decay(EvtParticle *p);
	virtual void init();
	virtual void initProbMax();

	virtual std::string commandName() {
		return "BTOKSTARLLDURHAM07_MODEL";
	}
	virtual void command(std::string cmd);

	static const std::string constraintsCommand;
	static const std::string formFactorCommand;
	static const std::string probablityProfileCommand;
	static const std::string highq2CutCommand;
	static const std::string lowq2CutCommand;
	static const std::string modelCommand;

	

private:
	
	EvtBToKStarllDurham07(const EvtBToKStarllDurham07&);
	EvtBToKStarllDurham07& operator=(const EvtBToKStarllDurham07&);
	
	//must be static as is shared between models
	static qcd::IPhysicsModel* _model;
	static QCDFactorisation* _calculator;
	static double _poleSize;
	static double _lowq2Cut;
	static double _highq2Cut;
	static bdkszmm::PARAMETERIZATIONS _ffModel;
	static bool _calcConstraints;
	static bool _writeProbProfile;
	
protected:
	
	double getPoleSize() const{
		return _poleSize;
	}
	
	void setPoleSize(const double poleSize){
		_poleSize = poleSize;
	}

  bool isWithinQ2Range(const double q2){
    return ( ((q2+1e-5) > _lowq2Cut) && ((q2-1e-5) < _highq2Cut) ) ? true : false;
  }
  
  void handleCommand(const std::string& key, const std::string& value);
  void handleModelCommand(const std::string& model);
  void getOptions(const std::string& cmd, std::map<std::string, std::string>* optionsMap) const;
   

};

#endif /* EVTBTOKSTARLLDURHAM */

