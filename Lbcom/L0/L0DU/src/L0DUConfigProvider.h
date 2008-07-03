// $Id: L0DUConfigProvider.h,v 1.4 2008-07-03 18:33:11 odescham Exp $
#ifndef L0DUCONFIGPROVIDER_H 
#define L0DUCONFIGPROVIDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interfaces
#include "L0Interfaces/IL0DUConfigProvider.h"            

/** @class L0DUConfigProvider L0DUConfigProvider.h
 *  
 *  Provide a **SINGLE** L0DU configuration according to 
 *  the Trigger Configuration Key
 *  Configuration described in options file
 *
 *  @author Olivier Deschamps

 *  @date   2007-09-19
 */
class L0DUConfigProvider : public GaudiTool, virtual public IL0DUConfigProvider {
public: 
  /// Standard constructor
  L0DUConfigProvider( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  

  virtual ~L0DUConfigProvider( ); ///< Destructor

  LHCb::L0DUConfig*  config(long tck){
    if( tck != m_tckopts)warning() << "The requested TCK " << tck  
                                   <<" is not consistent with the options definition " 
                                   << m_tckopts << " CHECK your options ! " << endreq;
    if(tck < 0 || tck > 0xFFFF){
      warning() << "requested TCK is not a 16 bit word" << endreq;
      return NULL;
    }
    return m_configs.object(tck); 
  }; 

  LHCb::L0DUConfigs  configs(){return m_configs;};
  
  

protected:



private:
  std::vector<std::string> Parse(std::string flag, std::vector<std::string> config );
  

  std::map<std::string,int> m_constData;
  typedef std::vector<std::vector<std::string> > ConfigType;
  typedef ConfigType::iterator ConfigIterator;
  
  ConfigType m_data;
  ConfigType m_conditions;
  ConfigType m_channels;
  ConfigType m_triggers;
  

  bool m_print;

  void printConfig(LHCb::L0DUConfig config);

  void constantData();
  void predefinedData(const std::string& name);
  void hardcodedData();
  StatusCode createData();
  StatusCode createConditions();
  StatusCode createChannels();
  StatusCode createTriggers();

  //options-defined data/conditions/channels
  LHCb::L0DUElementaryData::Map m_dataMap;
  LHCb::L0DUElementaryCondition::Map m_conditionsMap;
  LHCb::L0DUChannel::Map m_channelsMap;
  LHCb::L0DUTrigger::Map m_triggersMap;

  LHCb::L0DUConfigs m_configs;
  LHCb::L0DUConfig* m_config;
  


  unsigned long m_cData;
  int m_pData;
  long m_tckopts;
  std::vector<std::string> m_comparators;
  std::vector<std::pair<std::string, unsigned int> > m_operators;

  std::string m_def;
  std::string m_recipe;
  
  std::map<std::string,std::string> m_sepMap;
  std::pair <std::string,std::string> m_separators;
  std::vector<std::string> m_dataFlags;
  std::vector<std::string> m_condFlags;
  std::vector<std::string> m_chanFlags;
  std::vector<std::string> m_trigFlags;
  
};
#endif // L0DUCONFIGPROVIDER_H
