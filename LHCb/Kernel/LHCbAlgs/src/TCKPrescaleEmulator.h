#ifndef TCKPRESCALEEMULATOR_H 
#define TCKPRESCALEEMULATOR_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "GaudiAlg/GaudiAlgorithm.h"
#include "DetDesc/Condition.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "Kernel/IPropertyConfigSvc.h"
#include "Kernel/TCK.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/HltDecReports.h"
using namespace LHCb;

/** @class TCKPrescaleEmulator TCKPrescaleEmulator.h
 *  
 *
 *  @author Conor Fitzpatrick
 *  @date   2012-09-13
 */
class TCKPrescaleEmulator : public GaudiAlgorithm {
    public: 
	/// Standard constructor
	TCKPrescaleEmulator( const std::string& name, ISvcLocator* pSvcLocator );

	virtual ~TCKPrescaleEmulator( ); ///< Destructor

	virtual StatusCode initialize();    ///< Algorithm initialization
	virtual StatusCode execute   ();    ///< Algorithm execution

    protected:

    private:
    	std::string m_scalerName; //< Name prepended to PropertyConfig 
	std::string m_postScalerName; //< Name postpended to PropertyConfig if postscaled
	std::string m_preScalerName; //< Name postpended to PropertyConfig if prescaled
	
	StatusCode getPrescalesFromTCK(unsigned int, std::map<std::string, double> &, std::map<std::string, double> &, std::map<std::string, double> &); ///< Fill maps with prescale, postscale and post*prescale based on specificed TCK

	StatusCode getReports(); ///< get the DecReports for each event, calculate the ratio of prescales if the TCK has changed or first event and call updatePrescalers if so

	StatusCode getPrescalers(); ///< Initialise Prescalers
	StatusCode updatePrescalers(); ///< Set accept rates of Prescalers based on ratio of prescales from the user-specced TCK and the TCK in (the first, or changed) MC event TCK


	StatusCode i_cacheTriggerData();              ///< Function extracting data from Condition
	
	//Pre, post scales we get from the MC data itself
	std::map<std::string, double> prescalesInMC; //name of prescalers
	std::map<std::string, double> postscalesInMC; //name of postscalers
	std::map<std::string, double> scaleProductsInMC; //pre*post-scales 

	//Pre, post scales we get from the user-specified TCK
	std::map<std::string, double> prescalesToEmulate; //name of prescalers
	std::map<std::string, double> postscalesToEmulate; //name of postscalers
	std::map<std::string, double> scaleProductsToEmulate; //pre*post-scales 

	std::map<std::string, double> scaleProductsToApply; ///< map of scaleProducts that will get applied to the MC, (ratio of MC and ToEmulate scaleProducts)
	std::map<std::string, Algorithm*> prescalers; ///< map of prescaler algorithms

	mutable IPropertyConfigSvc * m_propertyConfigSvc;
	unsigned int m_tck;			      //
	std::string m_propertyConfigSvcName;          ///< Name of PropertyConfigSvc
	Condition *m_condTrigger;                     ///< Condition for sampling coefficients
	unsigned int m_triggerTCK;                    ///< tck for these data
	std::string m_hltDecReportsLocation;	      ///< Location of the DecReports
	std::vector<std::string> m_linesToKill;	      ///< lines to prescale to zero in MC regardless of existence in TCK or not
	bool endedWith(const std::string &lineName, const std::string &ending);
	bool firstevent;
	unsigned int lasttck;

	HltDecReports* decreports;
};
#endif // TCKPRESCALEEMULATOR_H
