// Include files 

// from Gaudi
#include "GaudiKernel/IAlgManager.h"
#include "GaudiAlg/GaudiSequencer.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "Kernel/IANNSvc.h"
#include "OfflineDeterministicPrescaler.h"
// local
#include "TCKPrescaleEmulator.h"
using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : TCKPrescaleEmulator
//
// 2012-09-13 : Conor Fitzpatrick
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TCKPrescaleEmulator )


	//=============================================================================
	// Standard constructor, initializes variables
	//=============================================================================
TCKPrescaleEmulator::TCKPrescaleEmulator( const std::string& name,
		ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
	m_propertyConfigSvc(0),
	m_condTrigger(0),
	m_triggerTCK(0),
  firstevent(true),
  lasttck(0),
  decreports(NULL)
{
	//TCK we wish to emulate prescales for:
	declareProperty("TCKtoEmulate",m_tck=0x9c0044);
	declareProperty("ConfigQualifier",m_scalerName="DeterministicPrescaler");
	declareProperty("PreScalerQualifier",m_preScalerName="PreScaler");
	declareProperty("PostScalerQualifier",m_postScalerName="PostScaler");
	declareProperty("HltDecReportsLocation",m_hltDecReportsLocation=HltDecReportsLocation::Default);
	declareProperty("LinesToAlwaysKill",m_linesToKill=std::vector<std::string>(0));
	declareProperty( "IPropertyConfigSvcInstance", m_propertyConfigSvcName = "PropertyConfigSvc");

}
//=============================================================================
// Destructor
//=============================================================================
TCKPrescaleEmulator::~TCKPrescaleEmulator() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TCKPrescaleEmulator::initialize() {
	if (UNLIKELY( msgLevel(MSG::VERBOSE))) verbose() << "TCKPrescaleEmulator::initialize() tck=" << std::hex << m_tck << endmsg;
	StatusCode sc = GaudiAlgorithm::initialize(); 
	if ( sc.isFailure() ) return sc;


	//Get property config service
	if (!service( m_propertyConfigSvcName, m_propertyConfigSvc).isSuccess()) {
		fatal() << "Failed to get the IConfigAccessSvc." << endmsg;
		return StatusCode::FAILURE;
	}

	//Get conditions
	if (this->existDet<Condition>("Conditions/Online/LHCb/RunInfo/Trigger")) {
		registerCondition("Conditions/Online/LHCb/RunInfo/Trigger",
				m_condTrigger, &TCKPrescaleEmulator::i_cacheTriggerData);
		if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "Conditions/Online/LHCb/RunInfo/Trigger found" << endmsg;
	} else {
		warning() << "Conditions/Online/LHCb/RunInfo/Trigger not found." << endmsg;
	}

	//If "LinesToKill" hasn't been filled, fill it with sensible defaults, if it has been filled, make sure the user knows
	if(m_linesToKill.size() == 0){
		m_linesToKill.push_back("Hlt1Global");
		m_linesToKill.push_back("Hlt2Global");
		m_linesToKill.push_back("Hlt1Phys");
		m_linesToKill.push_back("Hlt2Phys");
		m_linesToKill.push_back("Hlt1Lumi");
		//m_linesToKill.push_back("Hlt1MBNoBias");
		//m_linesToKill.push_back("Hlt1CharmCalibrationNoBias");
		m_linesToKill.push_back("Hlt2Transparent");
		info() << "By default the following decisions are killed as they're meaningless in MC: ";
		for( std::vector<std::string>::iterator i=m_linesToKill.begin(); i!=m_linesToKill.end(); ++i){
			info() << (*i) << "," << endmsg;
		}
		info() << "To change this behavior fill property LinesToAlwaysKill with your own choice of lines" << endmsg;
	}else{
		warning() << "You have chosen to add some lines to kill. This overwrites the default lines to be killed. Make sure you know what you are doing!" << endmsg;
	}

	sc = getPrescalesFromTCK(m_tck,prescalesToEmulate, postscalesToEmulate, scaleProductsToEmulate);
	if ( sc.isFailure() ){ 
		fatal() << "FAILED TO GET PRESCALES FROM TCK! " << endmsg; 
		return sc;
	}
	getPrescalers();
	firstevent = true;
	return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TCKPrescaleEmulator::execute() {
	StatusCode sc = StatusCode::SUCCESS;
	if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

	// Get the dec reports, update prescalers if first event or TCK has changed
	sc = getReports();
	if(sc){
		if(UNLIKELY(msgLevel(MSG::VERBOSE))) verbose() << *decreports << endmsg;
		HltDecReports* reports = new HltDecReports();
		reports->setConfiguredTCK(decreports->configuredTCK());
		reports->setTaskID( decreports->taskID() );
		HltDecReport report;
		std::string lineName;
		std::string strip= "Decision";
		//loop over the Decreports
		for(HltDecReports::Container::iterator itdec = decreports->begin(); itdec !=decreports->end(); ++itdec){
			//Get the name of line, if it ends with "Decision" remove it: 
			lineName =  (*itdec).first;
			if(endedWith(lineName,strip)){lineName.erase(lineName.end()-strip.length(),lineName.end());}
			//Get the report
			report = (*itdec).second;
			//We only care about lines that fired: 
			if(report.decision()){
				//Pull the OfflineDeterministicPrescaler for this line from out map of ODPs: 
				Algorithm* myAlg= prescalers[lineName];
				//If the algorithm with this lineName exists and is enabled: 
				if(myAlg && myAlg->isEnabled()){
					StatusCode result = myAlg->sysExecute();
					if ( ! result.isSuccess() ) return StatusCode::FAILURE;
					//If the ODP wants the line killed: 
					if(!myAlg->filterPassed()){
						if(UNLIKELY(msgLevel(MSG::VERBOSE))) verbose() << "Prescaling this line: " << lineName  << endmsg;
						report.setDecision(false);
						report.setExecutionStage(6);
					}

				}else{
					//We've found a line that wasn't in the TCK... should this happen? 
					//We kill it to be safe...
					Warning("DecReport found for line " + lineName + " but no prescaler found! Killing this line").ignore();
					report.setDecision(false);
					report.setExecutionStage(6);
				}
			}
			reports->insert((*itdec).first,report);
		}
		decreports->setDecReports(reports->decReports());
		if(UNLIKELY(msgLevel(MSG::VERBOSE)))  verbose() << *decreports << endmsg;
	}else{
		Warning("FAILED TO GET DECREPORTS! CANNOT PRESCALE THIS EVENT!").ignore();
	}
	setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
	return sc;
}

//============================================================================
// Get Dec reports, set prescaler acceptrates if first event or TCK changed
//============================================================================
StatusCode TCKPrescaleEmulator::getReports(){
	StatusCode sc = StatusCode::FAILURE;
	decreports = get<HltDecReports>(m_hltDecReportsLocation);
	if(decreports){
		if(!firstevent){
			if(decreports->configuredTCK() == lasttck){
				// TCK unchanged, no need to do anything
				if (UNLIKELY(msgLevel(MSG::VERBOSE))) verbose() << "TCK unchanged, continue " << endmsg;
				return StatusCode::SUCCESS;
			}else{
				// If we're already processing events and the TCK changes mid-run, warn the user, update the prescalers
				// These shouldn't be Warning, as if/when they occur the user needs to know. 
				warning() << "*************** DANGER ****************" << endmsg;
				warning() << "THE TCK HAS CHANGED!!!!!!!!!!!!!!" << endmsg;
				warning() << "PREV TCK: " << std::hex << lasttck << endmsg;
				warning() << "NEW TCK : " << std::hex << decreports->configuredTCK() << endmsg;
				warning() << "MAKE SURE YOU EXPECTED THIS TO HAPPEN" << endmsg;
				warning() << "*************** DANGER ****************" << endmsg;
				lasttck = decreports->configuredTCK();
				sc = getPrescalesFromTCK(lasttck, prescalesInMC, postscalesInMC, scaleProductsInMC);
				updatePrescalers();
			}
		}else{
			// If it's the first event save the tck to lasttck, update the prescalers
			lasttck=decreports->configuredTCK();
			if (UNLIKELY(msgLevel(MSG::VERBOSE))) verbose() << "First event, getting prescales for MC using TCK " << lasttck << endmsg;

			sc = getPrescalesFromTCK(lasttck, prescalesInMC, postscalesInMC, scaleProductsInMC);
			updatePrescalers();
			firstevent = false;
		}

	}
	return sc;
}

//=============================================================================
//Get map of line names and prescales from requested TCK 
//=============================================================================
StatusCode TCKPrescaleEmulator::getPrescalesFromTCK(unsigned int tck, std::map<std::string, double> &prescales, std::map<std::string, double> &postscales, std::map<std::string, double> &scaleProducts){
	prescales.clear();
	postscales.clear();
	scaleProducts.clear();
	TCK _tck(tck);
	_tck.normalize();
	ConfigTreeNodeAlias::alias_type alias( std::string("TCK/") +  _tck.str()  );
	const ConfigTreeNode* tree = m_propertyConfigSvc->resolveConfigTreeNode(alias);
	if (!tree) {
		// if we could not resolve the (non-zero) TCK we have a problem
		fatal() << "TCK " << std::hex << _tck << " could not be resolved! Cannot continue" << endmsg;
		return StatusCode::FAILURE;
	}
	const std::vector<PropertyConfig::digest_type>& digests =  m_propertyConfigSvc->collectLeafRefs( tree->digest() );
	std::string lineName;
	for(std::vector<PropertyConfig::digest_type>::const_iterator j = digests.begin(); j!= digests.end(); ++j){
		const PropertyConfig* config = m_propertyConfigSvc->resolvePropertyConfig( *j );
		if(config){
			//Check that fullyQualifiedName contains "DeterministicPrescaler"
			if(std::string::npos != config->fullyQualifiedName().find(m_scalerName)){
				//If it does, get the name
				lineName = config->name();
				//Loop to find the property "AcceptFraction" 
				for(PropertyConfig::Properties::const_iterator i =  config->properties().begin(); i!= config->properties().end(); ++i ) {
					if(i->first == "AcceptFraction"){
						double scale = -9999.;
						if (UNLIKELY( msgLevel(MSG::VERBOSE))) verbose() << "Got an AcceptFraction = " << i->second << " for " << lineName << endmsg;
						try {
							std::string str2 = (i->second);
							scale = std::strtod(str2.c_str(), 0);
						}catch (const std::exception&){
							Warning("could not find pre/post scale in " + i->first).ignore();
							scale=-9999.;
						}
						if(scale>-9999.){
							// Is it a Prescale, Postscale, or something else? 
							//prescale:
							if(endedWith(lineName,m_preScalerName)){
								lineName.erase(lineName.end()-m_preScalerName.length(),lineName.end());
								prescales[lineName]=scale; //Write the prescale to a map
								postscales.insert(std::pair<std::string,double>(lineName,-1.0)); //Initialise a postscale if it isn't already there with a nonsense value
								break;
							}else{
								//postscale:
								if(endedWith(lineName,m_postScalerName)){
									lineName.erase(lineName.end()-m_postScalerName.length(),lineName.end());
									postscales[lineName]=scale; //Write the postscale to a map
									prescales.insert(std::pair<std::string,double>(lineName,-1.0)); //Initialise a prescale if it isn't already there with a nonsense value
									break;
								}else{
									Warning("Found property AcceptFraction not associated to a pre/post scale! " + lineName).ignore();
								}
							}

						}
					}

				}

			}
		}
	}


	//Add lines we specifically want to kill
	for( std::vector<std::string>::iterator i=m_linesToKill.begin(); i!=m_linesToKill.end(); ++i){
		prescales[(*i)] = 0.0;
		postscales[(*i)] = 0.0;
	}

	//Make products of pre, post scales, print (if verbose)
	if(UNLIKELY( msgLevel(MSG::VERBOSE))){verbose() << "Line" << "	:	" << "Prescale" << "	:	" << "Postscale" << "	:	" << "Product" << endmsg;}
	for( std::map<std::string,double>::iterator i=prescales.begin(); i!=prescales.end(); ++i){
		double pre = (*i).second;
		if(pre<0.0){pre=1.0;} //If prescale was never found it'll be -9999., and we assume it will always pass
		double post = postscales[(*i).first];
		if(post<0.0){post=1.0;} //If postscale was never found it'll be -9999., and we assume it will always pass
		scaleProducts.insert(std::pair<std::string,double>((*i).first,pre*post));
		if(UNLIKELY( msgLevel(MSG::VERBOSE))){verbose() << (*i).first << "	:	" << (*i).second << "	:	" << postscales[(*i).first] << "	:	" << scaleProducts[(*i).first] << endmsg;}


	}
	return StatusCode::SUCCESS;
}

//========================================================================
// Create the OfflineDeterministicPrescaler instances
// These are initialised with acceptfractions from the TCK, then updated at runtime with the ratios
//========================================================================
StatusCode TCKPrescaleEmulator::getPrescalers(){
	if(UNLIKELY( msgLevel(MSG::DEBUG))){debug() << "Initialising prescalers	" << endmsg;}

	StatusCode final = StatusCode::SUCCESS;
	prescalers.clear();

	//== Get the "Context" option if in the file...
	IJobOptionsSvc* jos = svc<IJobOptionsSvc>( "JobOptionsSvc" );
	bool addedContext = false;  //= Have we added the context ?
	bool addedRootInTES = false;  //= Have we added the rootInTES ?
	bool addedGlobalTimeOffset = false;  //= Have we added the globalTimeOffset ?
	bool addedPrescale = false; //= have we added the prescale? 

	//= Get the Application manager, to see if algorithm exist
	IAlgManager* appMgr = svc<IAlgManager>("ApplicationMgr");
	for( std::map<std::string,double>::iterator it=scaleProductsToEmulate.begin(); it!=scaleProductsToEmulate.end(); ++it){
		std::string tn = "OfflineDeterministicPrescaler/";
		tn.append(it->first);
		tn.append("ODPForPrescaleEmulator");
		const Gaudi::Utils::TypeNameString typeName(tn);
		const std::string &theName = typeName.name();
		const std::string &theType = typeName.type();
		if(UNLIKELY( msgLevel(MSG::VERBOSE))){verbose() << "Initialising Algorithm:	" << theName << " of type:	" << theType << endmsg;}

		//== Check wether the specified algorithm already exists. If not, create it
		StatusCode result = StatusCode::SUCCESS;
		SmartIF<IAlgorithm> myIAlg = appMgr->algorithm(typeName, false); // do not create it now

		if ( !myIAlg.isValid() ) {

			if(UNLIKELY( msgLevel(MSG::VERBOSE))){verbose() << "AppManager doesn't already have an instance of " << theName << endmsg;}
			//== Set the Context if not in the jobOptions list
			if ( ""  != context() || ""  != rootInTES() ||0.0 != globalTimeOffset() ) {
				bool foundContext = false;
				bool foundRootInTES = false;
				bool foundGlobalTimeOffset = false;
				const std::vector<const Property*>* properties = jos->getProperties( theName );
				if ( 0 != properties ) {
					if(UNLIKELY( msgLevel(MSG::VERBOSE))){verbose() << "Found properties for " << theName << endmsg;}
					// Iterate over the list to set the options
					for ( std::vector<const Property*>::const_iterator itProp = properties->begin();
							itProp != properties->end();
							itProp++ )   {
						const StringProperty* sp = dynamic_cast<const StringProperty*>(*itProp);
						if ( 0 != sp )    {
							if ( "Context" == (*itProp)->name() ) {
								foundContext = true;
							}
							if ( "RootInTES" == (*itProp)->name() ) {
								foundRootInTES = true;
							}
							if ( "GlobalTimeOffset" == (*itProp)->name() ) {
								foundGlobalTimeOffset = true;
							}
						}
					}
				}
				if ( !foundContext && "" != context() ) {
					StringProperty contextProperty( "Context", context() );
					jos->addPropertyToCatalogue( theName, contextProperty ).ignore();
					addedContext = true;
				}
				if ( !foundRootInTES && "" != rootInTES() ) {
					StringProperty rootInTESProperty( "RootInTES", rootInTES() );
					jos->addPropertyToCatalogue( theName, rootInTESProperty ).ignore();
					addedRootInTES = true;
				}
				if ( !foundGlobalTimeOffset && 0.0 != globalTimeOffset() ) {
					DoubleProperty globalTimeOffsetProperty( "GlobalTimeOffset", globalTimeOffset() );
					jos->addPropertyToCatalogue( theName, globalTimeOffsetProperty ).ignore();
					addedGlobalTimeOffset = true;
				}
			}

			//Set the acceptfraction and verbosity for the ODP: 
			if(UNLIKELY( msgLevel(MSG::VERBOSE)))verbose() << "Setting an acceptfraction for " << theName << endmsg;
			IntegerProperty outputLevelProperty( "OutputLevel", MSG::WARNING);
			jos->addPropertyToCatalogue( theName, outputLevelProperty ).ignore();


			DoubleProperty acceptFractionProperty( "AcceptFraction", it->second);
			jos->addPropertyToCatalogue( theName, acceptFractionProperty ).ignore();
			//		addedPrescale = true;

			Algorithm *myAlg = 0;
			result = createSubAlgorithm( theType, theName, myAlg );
			// (MCl) this should prevent bug #35199... even if I didn't manage to
			// reproduce it with a simple test.
			if (result.isSuccess()) myIAlg = myAlg;
		} else {
			if(UNLIKELY( msgLevel(MSG::VERBOSE))){verbose() << "Algorithm " << theName << "already exists!" << endmsg;}
			Algorithm *myAlg = dynamic_cast<Algorithm*>(myIAlg.get());
			if (myAlg) {
				subAlgorithms()->push_back(myAlg);
				// when the algorithm is not created, the ref count is short by one, so we have to fix it.
				myAlg->addRef();
			}
		}

		//== Remove the property, in case this is not a GaudiAlgorithm...

		if ( addedContext ) {
			jos->removePropertyFromCatalogue( theName, "Context" ).ignore();
			addedContext = false;
		}
		if ( addedRootInTES ) {
			jos->removePropertyFromCatalogue( theName, "RootInTES" ).ignore();
			addedRootInTES = false;
		}
		if ( addedGlobalTimeOffset ) {
			jos->removePropertyFromCatalogue( theName, "GlobalTimeOffset" ).ignore();
			addedGlobalTimeOffset = false;
		}

		if(addedPrescale){
			jos->removePropertyFromCatalogue( theName, "AcceptFraction" ).ignore();
			addedPrescale = false;
		}

		// propagate the sub-algorithm into own state.
		if ( result.isSuccess () &&
				Gaudi::StateMachine::INITIALIZED <= FSMState() &&
				myIAlg.isValid   () &&
				Gaudi::StateMachine::INITIALIZED  > myIAlg->FSMState() )
		{
			StatusCode sc = myIAlg->sysInitialize() ;
			if  ( sc.isFailure() ) { result = sc ; }
		}

		// propagate the sub-algorithm into own state.
		if ( result.isSuccess () &&
				Gaudi::StateMachine::RUNNING <= FSMState() &&
				myIAlg.isValid   () &&
				Gaudi::StateMachine::RUNNING  > myIAlg->FSMState() )
		{
			StatusCode sc = myIAlg->sysStart () ;
			if  ( sc.isFailure() ) { result = sc ; }
		}

		//== Is it an Algorithm ?  Strange test...
		if ( result.isSuccess() ) {
			// TODO: (MCl) it is possible to avoid the dynamic_cast in most of the
			//             cases by keeping the result of createSubAlgorithm.
			Algorithm*  myAlg = dynamic_cast<Algorithm*>(myIAlg.get());
			if (myAlg!=0) {


				// Note: The reference counting is kept by the system of sub-algorithms
				prescalers.insert(std::pair<std::string,Algorithm*>(it->first,myAlg));
				if (msgLevel(MSG::DEBUG)) debug () << "Added algorithm " << theName << endmsg;
			} else {
				warning() << theName << " is not an Algorithm - failed dynamic_cast"
					<< endmsg;
				final = StatusCode::FAILURE;
			}
		} else {
			warning() << "Unable to find or create " << theName << endmsg;
			final = result;
		}

	}

	release(appMgr).ignore();
	release(jos).ignore();

	//== Print the list of algorithms
	MsgStream& msg = info();
	msg << "Member list: ";
	for ( std::map<std::string,Algorithm*>::const_iterator itE = prescalers.begin(); itE != prescalers.end(); ++itE ) {
		Algorithm *myAlg = (*itE).second;
		std::string myAlgType = System::typeinfoName( typeid( myAlg) ) ;
		if ( myAlg->name() == myAlgType ) {
			msg << myAlg->name();
		} else {
			msg << myAlgType << "/" << myAlg->name();
		}
		msg << ", ";
	}
	if ( "" != context() ) msg << ", with context '" << context() << "'";
	if ( "" != rootInTES() ) msg << ", with rootInTES '" << rootInTES() << "'";
	if ( 0.0 != globalTimeOffset() ) msg << ", with globalTimeOffset " << globalTimeOffset();
	msg << endmsg;
	return final;

}

//=============================================================================
// Compare scale products between MC to run on and the TCK to emulate
//=============================================================================
StatusCode TCKPrescaleEmulator::updatePrescalers(){
	if(UNLIKELY( msgLevel(MSG::DEBUG))){debug() << "Updating Prescalers" << endmsg;}
	StatusCode sc = StatusCode::SUCCESS;
	scaleProductsToApply.clear();

	//This is tricky- We have two setps of scaleproducts, one from the MC, one from the TCK to emulate. 
	//If the MC already has a prescale and it is less than the prescale in the TCK, panic! We can't do anything about that. 
	// If the MC already has a prescale and it is greater than the MC, calculate the ratio we need to scale the trigger by in MC, feed that to the prescaler
	// If the MC has lines that the TCK does not, always prescale them to zero
	// If the TCK has lines that the MC does not, panic! We can't do anything about that.

	//Loop over the TCK prescales: If the MC doesn't have this line, shout at the user 
	//otherwse write to the master list the ratio of TCK prescale to MC. If >1.0, shout at the user:
	for( std::map<std::string,double>::iterator i=scaleProductsToEmulate.begin(); i!=scaleProductsToEmulate.end(); ++i){
		OfflineDeterministicPrescaler* pre = dynamic_cast<OfflineDeterministicPrescaler*>(prescalers[(*i).first]);
		std::map<std::string,double>::iterator j = scaleProductsInMC.find((*i).first);
		if(j!=scaleProductsInMC.end()){
			if((*j).second > 0.0){
				double ratio = (*i).second/(*j).second;
				if(ratio<=1.0){
					//MC >0 ratio <=1 means prescales are compatible, fill. 
					if(UNLIKELY( msgLevel(MSG::VERBOSE))){verbose() <<  (*i).first  << " TCK, MC are compatible. ratio: " << ratio << endmsg;}
					scaleProductsToApply.insert(std::pair<std::string,double>((*i).first,ratio));
					pre->update(ratio);
				}else{
					//MC >0 ratio >1 means prescales incompatible (can't accept more than 100% of a decision!) complain, set to 100%
					sc = StatusCode::FAILURE;

					//These shouldn't be Warning, as if/when they occur the user needs to know. 
					warning() << "*********************************************************************************" << endmsg;
					warning() << "THIS TCK IS INCOMPATIBLE WITH MC! PRESCALE FOR " << (*i).first << " IN MC IS " << (*j).second << " IN THE TCK IT IS: " << (*i).second << endmsg;
					warning() << "NOT PRESCALING, RESULTS WILL BE APPROXIMATE!" << endmsg;
					warning() << "*********************************************************************************" << endmsg;
					scaleProductsToApply.insert(std::pair<std::string,double>((*i).first,1.0));
					pre->update(1.0);
				}
			}else{
				if((*i).second > 0.0){
					//MC =0, TCK >0 means prescales incompatible (can't create decisions where there are none!) complain, set to 0%:
					sc = StatusCode::FAILURE;
					warning() << "*********************************************************************************" << endmsg;
					warning() << "THIS TCK IS INCOMPATIBLE WITH MC! PRESCALE FOR " << (*i).first << " IN MC IS " << (*j).second << " IN THE TCK IT IS: " << (*i).second << endmsg;
					warning() << "I CANNOT PRESCALE A DECISION THAT IS NEVER THERE! " << endmsg;
					warning() << "*********************************************************************************" << endmsg;
					scaleProductsToApply.insert(std::pair<std::string,double>((*i).first,0.0));
					pre->update(0.0);

				}else{
					//MC = 0.0, TCK=0.0, prescales are compatible and no events will ever have decisions.  Set to 0%:
					if (msgLevel(MSG::DEBUG)) debug() <<  (*i).first  << " Is prescaled to zero in both TCK and MC" << endmsg;
					scaleProductsToApply.insert(std::pair<std::string,double>((*i).first,0.0));
					pre->update(0.0);
				}
			}

		}else{
			pre->update(0.0);
			if((*i).second > 0){
				sc = StatusCode::FAILURE;
				warning() << "*********************************************************************************" << endmsg;
				warning() << "THIS TCK IS INCOMPATIBLE WITH MC! " << (*i).first << " IS IN THE TCK, BUT NOT IN THE MC! " << endmsg;
				warning() << "I CANNOT EMULATE THIS LINE!" << endmsg;
				warning() << "*********************************************************************************" << endmsg;
			}else{
				if (msgLevel(MSG::DEBUG)) debug() << "TCK, MC Compatible, " << (*i).first << " has a prescale of 0 in TCK and doesn't exist in MC" << endmsg;
			}
		}
	}

	//Kill lines that are in the MC but not in the TCK (we don't care if they're not, we just zero them)
	for( std::map<std::string,double>::iterator i=scaleProductsInMC.begin(); i!=scaleProductsInMC.end(); ++i){
		std::map<std::string,double>::iterator j = scaleProductsToApply.find((*i).first);
		if(j==scaleProductsToApply.end()){
			if (msgLevel(MSG::DEBUG)) debug() << " MC contains a line not in the TCK: "<< (*i).first <<" prescaling it to zero" << endmsg;
			scaleProductsToApply.insert(std::pair<std::string,double>((*i).first,0.0));
		}
	}
	return sc;
}

//=========================================================================
// Find out if the lineName ends with a known string 
//=========================================================================
bool TCKPrescaleEmulator::endedWith(const std::string &lineName, const std::string &ending){
	if(lineName.length()>ending.length()){
		if(0 == lineName.compare (lineName.length() - ending.length(), ending.length(), ending)){
			return true;
		}
	}
	return false;
}

//=========================================================================
//  Extract data from Trigger
//=========================================================================
StatusCode TCKPrescaleEmulator::i_cacheTriggerData() {
	if (UNLIKELY( msgLevel(MSG::DEBUG))) debug() << "callback Trigger:" << endmsg;
	m_triggerTCK = (unsigned int) m_condTrigger->param<int>("TCK");
	if (UNLIKELY( msgLevel(MSG::VERBOSE))) verbose() << "tck from callback: " << m_triggerTCK << endmsg;
	return StatusCode::SUCCESS;
}

