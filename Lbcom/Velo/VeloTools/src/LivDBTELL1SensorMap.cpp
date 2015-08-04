#include <sstream>

#include <stdlib.h>
#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#endif // !WIN32

#include "VeloDet/DeVelo.h"

#include "LivDBTELL1SensorMap.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LivDBTELL1SensorMap
//
// 2008-08-11 : Kurt Rinnert
//-----------------------------------------------------------------------------
namespace Velo
{

  DECLARE_TOOL_FACTORY( LivDBTELL1SensorMap )

  //=============================================================================
  // Standard constructor
  //=============================================================================
  LivDBTELL1SensorMap::LivDBTELL1SensorMap( const std::string& type,
      const std::string& name,
      const IInterface* parent )
    : GaudiTool ( type, name , parent )
    {
      declareInterface<Velo::ITELL1SensorMap>(this);

      declareProperty("HTTPProxy", m_httpProxy = "", "Set this to 'http://netgw:8080/' when running inside the P8 online network.");
      declareProperty("InitFromFile", m_initFromFile = "", "If not empty, this is the path to a map file fetched from Liverpool earlier and there will be no DB access.");
      declareProperty("PUTELL1List", m_puTell1List, "List ot TELL1 IDs for PU sensors 128-131. Presence in the DB will always override this setting!");
    }

  //=============================================================================
  // Destructor
  //=============================================================================
  LivDBTELL1SensorMap::~LivDBTELL1SensorMap() {}

  //=============================================================================
  // Initializes maps from Liverpool DB
  //=============================================================================
  StatusCode LivDBTELL1SensorMap::initialize()
  {
    StatusCode sc = GaudiTool::initialize();
    if ( !sc ) return sc;
    
    // PU sensor are not yet in the data base.  If not configured by the users,
    // use the mapping as of 2009-08-11.
    if ( m_puTell1List.empty() || 4 != m_puTell1List.size() ) {
      info() << "PU TELL1 list not configured or wrong size.  Will use default mapping [128,129,130,131] -> [21,414,22,26]. DB or user file will always override!" << endmsg;
      m_tell1ToSensor[21]  = 128;
      m_tell1ToSensor[414] = 129;
      m_tell1ToSensor[22]  = 130;
      m_tell1ToSensor[26]  = 131;
      m_sensorToTell1[128] = 21;
      m_sensorToTell1[129] = 414;
      m_sensorToTell1[130] = 22;
      m_sensorToTell1[131] = 26;
    } else {
      m_tell1ToSensor[m_puTell1List[0]] = 128;
      m_tell1ToSensor[m_puTell1List[1]] = 129;
      m_tell1ToSensor[m_puTell1List[2]] = 130;
      m_tell1ToSensor[m_puTell1List[3]] = 131;
      m_sensorToTell1[128] = m_puTell1List[0]; 
      m_sensorToTell1[129] = m_puTell1List[1]; 
      m_sensorToTell1[130] = m_puTell1List[2]; 
      m_sensorToTell1[131] = m_puTell1List[3]; 
    } 

    std::string mapFileName(m_initFromFile);

    // only try to access the DB when no map file was specified
    // and we are not on Windows(TM)
    if ( m_initFromFile.empty() )  {

      // Get the mappings from the Liverpool DB.
      // This involves establishing an http connection and the creation
      // of a temporary file. In case this fails, the job will fail
      // as well.
#ifndef WIN32
      if ( !m_httpProxy.empty() ) {
        setenv("http_proxy", m_httpProxy.c_str(), 1);
      }

      unsigned int pid = static_cast<unsigned int>(getpid());
      std::ostringstream pidStr;
      pidStr << pid;
      mapFileName = "/tmp/" + pidStr.str() + "_tell1map.txt";

      std::string command = "wget -q --cache=off --connect-timeout=5 --tries=1 -O " + mapFileName + " http://hep.ph.liv.ac.uk/velodb/VELO/ExportTell1SW.txt >& /dev/null";

      if ( system(command.c_str()) ) { // could not connect to DB webserver
        error() << "Could access Liverpool DB. Check proxy settings!" << endmsg;
        return StatusCode::FAILURE;
      }
#endif // !WIN32      
    } else {
      info() << "Using user supplied map file '" << mapFileName << "'. There will be no DB access." << endmsg;
    }

    if ( mapFileName.empty() ) {
      info() << "Looks like you are running on Windows. DB access is not available on the platform and you did not specify a map file. Will use trivial mapping." << endmsg;
      DeVelo* velo = getDet<DeVelo>( DeVeloLocation::Default );
      for ( std::vector<DeVeloSensor*>::const_iterator si = velo->sensorsBegin();
          si != velo->sensorsEnd(); ++si ) {
        unsigned int sensor = (*si)->sensorNumber();
        m_sensorToTell1[sensor] = sensor;
        m_tell1ToSensor[sensor] = sensor;
      }
    } else {
      FILE* mapFile;
      mapFile = fopen(mapFileName.c_str(),"r");

      if ( 0 == mapFile ) { // tmp file invalid
        error() << "Could not open TELL1 map file '" << mapFileName << "'." << endmsg;
        return StatusCode::FAILURE;
      }

      unsigned int tell1, sensor;
      fscanf(mapFile,"Tell1 SW #\n");
      while ( EOF != fscanf(mapFile,"TL-%u %u\n",&tell1, &sensor) ) {
        m_sensorToTell1[sensor] = tell1;
        m_tell1ToSensor[tell1]  = sensor;
      }
      fclose(mapFile);
    }

#ifndef WIN32
    // do not delete user supplied map file
    if ( m_initFromFile.empty() )  {
      std::string command = "rm -f " + mapFileName + " >& /dev/null";
      system(command.c_str());
    }
#endif // !WIN32      

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Finalize
  //=============================================================================
  StatusCode LivDBTELL1SensorMap::finalize()
  {
    return GaudiTool::finalize();
  }
  
  //=============================================================================
  // Provide sensor number by TELL1 ID
  //=============================================================================
  unsigned int LivDBTELL1SensorMap::tell1ToSensor(unsigned int tell1)
  {
    return m_tell1ToSensor[tell1];
  }

  //=============================================================================
  // Provide TELL1 ID by sensor number
  //=============================================================================
  unsigned int LivDBTELL1SensorMap::sensorToTell1(unsigned int sensor)
  {
    return m_sensorToTell1[sensor];
  }
}


