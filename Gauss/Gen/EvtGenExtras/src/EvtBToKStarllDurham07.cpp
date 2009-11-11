//--------------------------------------------------------------------------
// Module: EvtBToKStarllDurham07.cc
//
// Description: Routine to implement b->sll decays according to Beneke et al. 
//
// Modification history:
//
//    wreece     October, 2008        Module created
//
//------------------------------------------------------------------------
// 
#include <cstdlib>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtBToKStarllDurham07.hh"

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <sstream>

#include "EvtGenModels/EvtBToVllPhysicsModel.hh"
#include "EvtGenModels/EvtBToVllBenchMarkPhysicsModels.hh"
#include "EvtGenModels/EvtBToVllGenericModel.hh"
#include "EvtGenModels/EvtBToVllParameterisedFFCalc.hh"

#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensor4C.hh"
#include "EvtGenBase/EvtDiracSpinor.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtAmp.hh"
#include "EvtGenBase/EvtScalarParticle.hh"
#include "EvtGenBase/EvtVectorParticle.hh"
#include "EvtGenBase/EvtDiLog.hh"

//for controlling the error handling
#include "gsl/gsl_errno.h"


qcd::IPhysicsModel* EvtBToKStarllDurham07::_model = 0;
QCDFactorisation* EvtBToKStarllDurham07::_calculator = 0;
double EvtBToKStarllDurham07::_poleSize = 0;

double EvtBToKStarllDurham07::_lowq2Cut = 0.0;//default is not to use the cuts
double EvtBToKStarllDurham07::_highq2Cut = 10000;
bdkszmm::PARAMETERIZATIONS EvtBToKStarllDurham07::_ffModel = bdkszmm::BALL07PRIVATE;
bool EvtBToKStarllDurham07::_calcConstraints = false;
bool EvtBToKStarllDurham07::_writeProbProfile = false;

//strings to be parsed from the decay file
const std::string EvtBToKStarllDurham07::constraintsCommand = "calcConstraints";
const std::string EvtBToKStarllDurham07::formFactorCommand = "formFactorModel";
const std::string EvtBToKStarllDurham07::probablityProfileCommand = "writeProbProfile";
const std::string EvtBToKStarllDurham07::highq2CutCommand = "highq2Cut";
const std::string EvtBToKStarllDurham07::lowq2CutCommand = "lowq2Cut";
const std::string EvtBToKStarllDurham07::modelCommand = "physicsModel";


EvtBToKStarllDurham07::EvtBToKStarllDurham07():
	EvtDecayAmp::EvtDecayAmp()
{
	//disable the GSL default error handling
	gsl_set_error_handler_off();
}

EvtDecayBase* EvtBToKStarllDurham07::clone(){
	return new EvtBToKStarllDurham07;
}

EvtBToKStarllDurham07::~EvtBToKStarllDurham07(){
}
void EvtBToKStarllDurham07::decay( EvtParticle *parent ){
	/**
	 * set the MC weight and decay the particle
	 */
	setWeight(parent->initializePhaseSpace(getNDaug(),getDaugs(),false,getPoleSize(),1,2));
	_calculator->getAmp(parent,_amp2);
}


void EvtBToKStarllDurham07::initProbMax(){

	//method very much based on that from EvtbTosllAmp
	if (getPoleSize()) {
		//only set the polesize once
		return;
	}

	const EvtId parent = getParentId();
	const EvtId meson = getDaug(0);
	const EvtId lepton1 = getDaug(1);
	const EvtId lepton2 = getDaug(2);
	double poleSize = 0;

	//This routine takes the arguements parent, meson, and lepton
	//number, and a form factor model, and returns a maximum
	//probability for this semileptonic form factor model.  A
	//brute force method is used.  The 2D cos theta lepton and
	//q2 phase space is probed.

	//Start by declaring a particle at rest.

	//It only makes sense to have a scalar parent.  For now. 
	//This should be generalized later.

	EvtScalarParticle *scalar_part;
	EvtParticle *root_part;

	scalar_part = new EvtScalarParticle;

	//cludge to avoid generating random numbers!
	scalar_part->noLifeTime();

	EvtVector4R p_init(EvtPDL::getMass(parent), 0.0, 0.0, 0.0);

	scalar_part->init(parent, p_init);
	root_part=(EvtParticle *)scalar_part;
	root_part->setDiagonalSpinDensity();

	EvtParticle *daughter, *lep1, *lep2;

	EvtAmp amp;

	EvtId listdaug[3];
	listdaug[0] = meson;
	listdaug[1] = lepton1;
	listdaug[2] = lepton2;

	amp.init(parent, 3, listdaug);

	root_part->makeDaughters(3, listdaug);
	daughter=root_part->getDaug(0);
	lep1=root_part->getDaug(1);
	lep2=root_part->getDaug(2);

	//cludge to avoid generating random numbers!
	daughter->noLifeTime();
	lep1->noLifeTime();
	lep2->noLifeTime();

	//Initial particle is unpolarized, well it is a scalar so it is 
	//trivial
	EvtSpinDensity rho;
	rho.setDiag(root_part->getSpinStates());

	double mass[3];

	double m = root_part->mass();//B_d mass

	EvtVector4R p4meson, p4lepton1, p4lepton2, p4w;
	double q2max;

	double q2, elepton, plepton;
	int i, j;
	double erho, prho, costl;

	double maxfoundprob = -1e6;
	double minfoundprob = 1e6;
	double prob = -10.0;
	int massiter;

	double maxpole=0;

	report(INFO, "EvtGen")	<< "Initialising the decay model with lowq2Cut(" << _lowq2Cut << "),highq2Cut("
							<< _highq2Cut << ")" << std::endl; 
	
	std::vector<std::pair<double,double> > q2values;

	for (massiter=0; massiter<3; massiter++) {

		mass[0] = EvtPDL::getMeanMass(meson);
		mass[1] = EvtPDL::getMeanMass(lepton1);
		mass[2] = EvtPDL::getMeanMass(lepton2);
		if (massiter==1) {
			mass[0] = EvtPDL::getMinMass(meson);
		}
		if (massiter==2) {
			mass[0] = EvtPDL::getMaxMass(meson);
			if ( (mass[0]+mass[1]+mass[2])>m)
				mass[0]=m-mass[1]-mass[2]-0.00001;
		}

		//set the q2 cut if needed
		q2max = (m-mass[0])*(m-mass[0]);
		if (_highq2Cut < q2max) {
			q2max = _highq2Cut;
		}
		const double four_m_mu = 4*(mass[1]*mass[1]);
		const double q2low = (four_m_mu > _lowq2Cut) ? four_m_mu : _lowq2Cut;

		//loop over q2 from 4m_mu^2  to (MB^2 - mK*^2) -q2
		const int maxq2Iter = 25;
		for (i=0; i < maxq2Iter; i++) {
			//sample in 25 bins over q2 spectrum
			q2 =  q2low + ((i*(q2max-q2low))/26.0);

			erho = (m*m + mass[0]*mass[0] - q2 )/(2.0*m);

			prho = sqrt(erho*erho-mass[0]*mass[0]);

			p4meson.set(erho, 0.0, 0.0, -1.0*prho);
			p4w.set(m-erho, 0.0, 0.0, prho);

			//This is in the W rest frame
			elepton = (q2+mass[1]*mass[1])/(2.0*sqrt(q2));
			plepton = sqrt(elepton*elepton-mass[1]*mass[1]);

			double probctl[3];

			//now iterate over cos theta_l
			for (j=0; j<3; j++) {

				costl = 0.99*(j - 1.0);

				//These are in the W rest frame. Need to boost out into
				//the B frame.
				p4lepton1.set(elepton, 0.0, plepton*sqrt(1.0-costl*costl),
						plepton*costl);
				p4lepton2.set(elepton, 0.0, -1.0*plepton*sqrt(1.0-costl*costl),
						-1.0*plepton*costl);

				EvtVector4R boost((m-erho), 0.0, 0.0, 1.0*prho);
				p4lepton1=boostTo(p4lepton1, boost);
				p4lepton2=boostTo(p4lepton2, boost);

				//Now initialize the daughters...
				daughter->init(meson, p4meson);
				lep1->init(lepton1, p4lepton1);
				lep2->init(lepton2, p4lepton2);

				_calculator->getAmp(root_part, amp);

				//Now find the probability at this q2 and cos theta lepton point
				//and compare to maxfoundprob.

				//Do a little magic to get the probability!!

				//report(INFO,"EvtGen") <<"amp:"<<amp.getSpinDensity()<<std::endl;

				prob = rho.normalizedProb(amp.getSpinDensity());

				//report(INFO,"EvtGen") << "prob:"<<q2<<" "<<costl<<" "<<prob<<std::endl;

				probctl[j]=prob;
			}

			//probclt contains prob at ctl=-1,0,1.
			//prob=a+b*ctl+c*ctl^2

			double a=probctl[1];
			double b=0.5*(probctl[2]-probctl[0]);
			double c=0.5*(probctl[2]+probctl[0])-probctl[1];

			prob=probctl[0];
			if (probctl[1]>prob)
				prob=probctl[1];
			if (probctl[2]>prob)
				prob=probctl[2];

			if (fabs(c)>1e-20) {
				double ctlx=-0.5*b/c;
				if (fabs(ctlx)<1.0) {
					double probtmp=a+b*ctlx+c*ctlx*ctlx;
					if (probtmp>prob)
						prob=probtmp;
				}

			}
			//store the probablity if we are going to write it out
			if(_writeProbProfile){
				q2values.push_back(std::make_pair(q2, prob));
			}

			if (i==0) {
				maxpole=prob;
				continue;
			}

			if (prob > maxfoundprob) {
				maxfoundprob = prob;
			}
			if (prob < minfoundprob) {
				minfoundprob = prob;
			}

		}
		if (EvtPDL::getWidth(meson) <= 0.0) {
			//if the particle is narrow dont bother with changing the mass.
			massiter = 4;
		}

	}

	root_part->deleteTree();

	poleSize=0.04*(maxpole/maxfoundprob)*4*(mass[1]*mass[1]);

	//write out a file showing the probability profile if requested
	//this can be used for choosing q2cuts to get the MC efficiency up
	if(_writeProbProfile){
		
		std::ostringstream os;
		os << "Prob_Profile_" << _model->getModelName() << ".dat";
		const std::string outFile = os.str();
		std::ofstream outputdata(outFile.c_str());

		for (std::vector<std::pair<double,double> >::iterator it = q2values.begin(); it
		!= q2values.end(); ++it) {
			outputdata << it->first << "\t" << it->second << std::endl;
		}
		outputdata.close();
	}

	maxfoundprob *=1.15;
	
	report(INFO,"EvtGen") << "maximum probability : " << maxfoundprob << "minimum probability : " << minfoundprob << std::endl;
	if( (maxfoundprob/minfoundprob) > 10){
		report(WARNING,"EvtGen") << "The ratio between the maximum and minimum probabilities is large. Generation will not be efficient. " <<
		"Consider setting lowq2Cut and highq2Cut in your decay file." << std::endl;
	}
	setProbMax(maxfoundprob);
	setPoleSize(poleSize);
}

/** Set up the physics model to use */
void EvtBToKStarllDurham07::command(std::string cmd){

	std::map<std::string, std::string> options;
	getOptions(cmd, &options);
	
	for(std::map<std::string, std::string>::iterator it = options.begin(); it != options.end(); ++it){
		handleCommand(it->first,it->second);
	}
	
	//set the physics command
	if(_model){
		if(options.count(qcd::GenericModel::modelCommand) > 0){
			_model->setCommand(options[qcd::GenericModel::modelCommand]);
		}
	}
		
}

void EvtBToKStarllDurham07::getOptions(const std::string& cmd, std::map<std::string, std::string>* optionsMap) const{
	
	if (cmd.empty()) {
		report(WARNING, "EvtGen")
				<< "EVTBTOKSTARLLDURHAM07_MODEL called with no arguments."
				<< " Reverting to default physics model." << std::endl;
	}

	//nasty and unreliable command parser
	std::ostringstream argString;
	std::ostringstream valueString;
	bool valBracket = false;

	//command seperator is ','. Then values are bracketed with ()
	for (std::string::const_iterator it = cmd.begin(); it != cmd.end(); ++it) {

		const char c = *it;

		switch (c) {

		case ',': //new command so reset
			if (valBracket) {

				report(ERROR, "EvtGen")
						<< "EVTBTOKSTARLLDURHAM07_MODEL command is missing a bracket."
						<< " Check your decay file." << std::endl;
				::abort();
			}
			(*optionsMap)[argString.str()] = valueString.str();

			//clear the buffers
			argString.str("");
			valueString.str("");

			break;
		case '(': //opening the value bracket
			valBracket = true;
			break;
		case ')'://closing the value bracket
			valBracket = false;
			break;
		default:
			if(valBracket) {
				valueString << c;
			} else {
				argString << c;
			}
		};
	}
	
	if(argString.str().size()){
		(*optionsMap)[argString.str()] = valueString.str();
	}
	
}


void EvtBToKStarllDurham07::handleCommand(const std::string& key, const std::string& value){
	
	if (key == modelCommand) {
		handleModelCommand(value);
	} else if (key == lowq2CutCommand) {

		double val = _lowq2Cut;
		std::istringstream in(value);
		in >> val;
		_lowq2Cut = val;

	} else if (key == highq2CutCommand) {

		double val = _highq2Cut;
		std::istringstream in(value);
		in >> val;
		_highq2Cut = val;

	} else if (key == formFactorCommand) {

		int val = 0;
		std::istringstream in(value);
		in >> val;
		//set the form factor model to use
		_ffModel = static_cast<bdkszmm::PARAMETERIZATIONS>(val);

	} else if (key == constraintsCommand) {
		//allow AFB zero and other constraints to be calculated
		bool val = false;
		std::istringstream in(value);
		in >> val;
		_calcConstraints = val;

	} else if (key == probablityProfileCommand) {
		//write out a probablity profile for testing
		bool val = false;
		std::istringstream in(value);
		in >> val;
		_writeProbProfile = val;

	}else if (key == qcd::GenericModel::modelCommand) {
		//the model parameters are set in the main command method
	}else {
		report(ERROR, "EvtGen")
				<< "EVTBTOKSTARLLDURHAM07_MODEL called with an command string: "
				<< key << " Check your decay file." << std::endl;
		::abort();
	}
  
}

void EvtBToKStarllDurham07::handleModelCommand(const std::string& modelName){

  	qcd::IPhysicsModel* _selectedModel = 0;
	
	//the list of models to choose from
	std::list<qcd::IPhysicsModel*> models;
	models.push_back(new qcd::GenericModel);
	models.push_back(new qcd::FBMSSMPhysicsModel);
	models.push_back(new qcd::GMSSMPhysicsModel);
	models.push_back(new qcd::LHTPhysicsModel);
	models.push_back(new qcd::SMPhysicsModel);
	models.push_back(new qcd::UEDPhysicsModel);

	//look for the specified model
	for(std::list<qcd::IPhysicsModel*>::iterator i = models.begin(); i != models.end(); ++i){
		if((*i)->getModelName() == modelName){
			_selectedModel = *i;
			break;
		}
	}
	
	//delete any unwanted models
	for(std::list<qcd::IPhysicsModel*>::iterator i = models.begin(); i != models.end(); ++i){
			if(*i != _selectedModel) delete *i;
		}
	
	if(!_selectedModel){
			report(ERROR,"EvtGen") 	<< "EVTBTOKSTARLLDURHAM07_MODEL called with an unrecognised model name."
									<< " Check your decay file." << std::endl;
			::abort();
		}
	assert(_selectedModel);
	//_selectedModel->setCommand();

	if(_model){
		if(_model->getModelName() != modelName){
			report(ERROR,"EvtGen") 	<< "EVTBTOKSTARLLDURHAM07_MODEL called multiple times with different models."
									<< " This should never happen and is a bug. Please report it!" << std::endl;
			::abort();
		}
	}else{
		_model = _selectedModel;
	}
	report(INFO,"EvtGen") << "Physics Model Selected was " << _selectedModel->getModelName() << std::endl;
  
}


void EvtBToKStarllDurham07::init(){
	
	EvtDecayAmp::init();

	checkNArg(0);
	checkNDaug(3);

	//We expect the parent to be a scalar 
	//and the daughters to be X lepton+ lepton-

	checkSpinParent(EvtSpinType::SCALAR);

	EvtSpinType::spintype mesontype=EvtPDL::getSpinType(getDaug(0));
	if (mesontype != EvtSpinType::VECTOR) {
		report(ERROR,"EvtGen") << "EvtBToKStarllDurham07 generator expected "
		<< " a VECTOR 1st daughter, found:"<<
		EvtPDL::name(getDaug(0)).c_str()<<std::endl;
		report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
		::abort();
	}

	checkSpinDaughter(1,EvtSpinType::DIRAC);
	checkSpinDaughter(2,EvtSpinType::DIRAC);
	
	//default to SM
	if(!_model){
		report(INFO,"EvtGen") << "EvtBToKStarllDurham07 defaulting to SM" << std::endl;
		_model = new qcd::SMPhysicsModel();
	}
	report(INFO,"EvtGen") << "formFactorModel is " << _ffModel << "." << std::endl;
	if(!_calculator){
		_calculator = new QCDFactorisation(*_model,_ffModel,_calcConstraints);
	}

}





