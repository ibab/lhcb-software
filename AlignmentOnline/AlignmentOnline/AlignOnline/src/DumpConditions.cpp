// Include files 
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiAlg/GaudiHistoAlg.h"

#include "DetDesc/RunChangeIncident.h"
#include "DetDesc/Condition.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/LVolume.h"

// Others
#include <string>
#include <sstream>
#include <fstream>


/** @class DumpConditions 
 *  
 *
 *  @author Giulio Dujany
 *  @date   2015-07-05
 */
// Forward declarations
class DataObject;
class Condition;
class IDetDataSvc;
class AlignmentCondition;

class DumpConditions : public GaudiHistoAlg {
public: 
   /// Standard constructor
   DumpConditions( const std::string& name, ISvcLocator* pSvcLocator );

   virtual ~DumpConditions( ); ///< Destructor

   virtual StatusCode initialize();    ///< Algorithm initialization
   virtual StatusCode execute   ();    ///< Algorithm execution
   virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

   bool debugLevel() const { return msgLevel( MSG::DEBUG ) || msgLevel( MSG::VERBOSE ) ; }

   /// Names of the conditions to print
   std::vector<std::string> m_condPaths;

   /// Container of the conditions to print
   GaudiUtils::Map<std::string,Condition*> m_conditions;

   std::string  m_outFileName ;

private:
  
   double m_startTime;
   int m_runNumber;
   IIncidentSvc* m_incSvc;
   IDetDataSvc *m_dds;
};

//-----------------------------------------------------------------------------
// Implementation file for class : DumpConditions
//
// 2015-07-05 : Giulio Dujany
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DumpConditions )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpConditions::DumpConditions(const std::string& name,
                               ISvcLocator* pSvcLocator )
: GaudiHistoAlg( name , pSvcLocator ),
   m_startTime{0},
   m_runNumber{0},
   m_incSvc{nullptr},
   m_dds{nullptr}
{
   declareProperty("Conditions", m_condPaths,
                   "list of paths to conditions in the detector transient store");
   declareProperty("OutFile", m_outFileName = "conditions.txt" ) ;
   declareProperty("RunStartTime", m_startTime ) ;
   declareProperty("RunNumber", m_runNumber ) ;
}
//=============================================================================
// Destructor
//=============================================================================
DumpConditions::~DumpConditions()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DumpConditions::initialize()
{
   StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
   if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

   // Print time
   m_dds = svc<IDetDataSvc>("DetectorDataSvc", true);
   info() << "-------------------------------------" << endmsg;
   info() << "EventTime = " << m_dds->eventTime() << endmsg;
   info() << "-------------------------------------" << endmsg;
   
   m_incSvc = svc<IIncidentSvc>("IncidentSvc", true);
   
   info() << "*** register conditions ***" << endmsg;
   try {
      // register conditions from list given 
      std::vector<std::string>::const_iterator path;
      for (const auto& path : m_condPaths) {
         if ( msgLevel(MSG::DEBUG) ) debug() << "Registering condition" << path << endmsg;
         m_conditions[path] = nullptr;
         registerCondition<DumpConditions>(path, m_conditions[path], nullptr);
      }
      return updMgrSvc()->update(this);
   }
   catch (GaudiException &e){
      fatal() << e << endmsg;
      return StatusCode::FAILURE;
   }
  
   return StatusCode::SUCCESS;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DumpConditions::finalize()

{
   return GaudiHistoAlg::finalize() ;  // must be called after all other actions
}

StatusCode DumpConditions::execute()
{
   // Fire RunChange incident to load XML used in data.
   m_dds->setEventTime(m_startTime);
   m_incSvc->fireIncident(RunChangeIncident(name(), m_runNumber));
   updMgrSvc()->newEvent();
   updMgrSvc()->update(this);

   // Print Parameters
   std::ofstream outFile(m_outFileName.c_str()) ;
   outFile << "Requested Conditions:\n\n";
   for (const auto& entry : m_conditions) {
      if (0 != entry.second) {
         outFile << "BEGIN CONDITION\n" << entry.first << "\n" << entry.second->printParams() << "END CONDITION\n\n" ;
      } else {
         error() << "Could not retrieve condition " <<  entry.first << endmsg;
         return StatusCode::FAILURE;
      }
   }
   return StatusCode::SUCCESS ;
}
