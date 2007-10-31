// $Id: L0DUConfigProvider.h,v 1.1 2007-10-31 15:04:45 odescham Exp $
#ifndef L0DUCONFIGPROVIDER_H 
#define L0DUCONFIGPROVIDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interfaces
#include "L0Interfaces/IL0DUConfigProvider.h"            

/** @class L0DUConfigProvider L0DUConfigProvider.h
 *  
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

  LHCb::L0DUConfig*  config(long tck){return m_configs.object(tck);};
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
  

  bool m_print;

  void printConfig(LHCb::L0DUConfig config);

  void constantData();
  void predefinedData(const std::string& name);
  void hardcodedData();
  StatusCode createData();
  StatusCode createConditions();
  StatusCode createChannels();

  //options-defined data/conditions/channels
  LHCb::L0DUElementaryData::Map m_dataMap;
  LHCb::L0DUElementaryCondition::Map m_conditionsMap;
  LHCb::L0DUChannel::Map m_channelsMap;
  LHCb::L0DUConfigs m_configs;
  LHCb::L0DUConfig* m_config;
  
  unsigned long m_cData;
  int m_pData;
  unsigned long m_tckopts;
  std::vector<std::string> m_comparators;
  std::vector<std::pair<std::string, unsigned int> > m_operators;

  
  std::map<std::string,std::string> m_sepMap;
  std::pair <std::string,std::string> m_separators;
  std::vector<std::string> m_dataFlags;
  std::vector<std::string> m_condFlags;
  std::vector<std::string> m_chanFlags;
  
  
  std::string m_def;
};
#endif // L0DUCONFIGPROVIDER_H
