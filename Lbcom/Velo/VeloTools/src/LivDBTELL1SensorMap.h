// $Id: LivDBTELL1SensorMap.h,v 1.1 2009-09-08 16:22:39 krinnert Exp $
#ifndef LIVDBTELL1SENSORMAP_H 
#define LIVDBTELL1SENSORMAP_H 1

#include <map>
#include <vector>

#include "GaudiAlg/GaudiTool.h"
#include "Tell1Kernel/ITELL1SensorMap.h"            // Interface


namespace Velo
{
  /** @class LivDBTELL1SensorMap LivDBTELL1SensorMap.h
   * Provides mapping from TELL1 hardware IDs to sofware sensor
   * number and vice versa. 
   *
   * The map is initialized from the Liverpool hardware DB.  This requires
   * network access, a wget installation and a number of system calls.  Hence
   * DB access will not be available on Windows.
   *  
   *
   *  @author Kurt Rinnert
   *  @date   2009-08-11
   */

  class LivDBTELL1SensorMap : public GaudiTool, virtual public Velo::ITELL1SensorMap {
    public: 
      /// Standard constructor
      LivDBTELL1SensorMap( const std::string& type, 
          const std::string& name,
          const IInterface* parent);

      virtual ~LivDBTELL1SensorMap( ); ///< Destructor

      virtual StatusCode initialize(); ///< Initializes map from Liverpool DB
      virtual StatusCode finalize(); 

      virtual unsigned int tell1ToSensor(unsigned int tell1); ///< Map TELL1 to sensor number
      virtual unsigned int sensorToTell1(unsigned int sensor);///< Map sensor number to TELL1

    private:

      // configuration
      std::string m_httpProxy;
      std::string m_initFromFile;
      std::vector<unsigned int> m_puTell1List;

      // mappings
      std::map<unsigned int, unsigned int> m_tell1ToSensor;
      std::map<unsigned int, unsigned int> m_sensorToTell1;

  };
}
#endif // LIVDBTELL1SENSORMAP_H 
