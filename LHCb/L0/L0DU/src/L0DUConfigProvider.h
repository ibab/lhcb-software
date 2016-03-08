#ifndef L0DUCONFIGPROVIDER_H 
#define L0DUCONFIGPROVIDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interfaces
#include "L0Interfaces/IL0DUConfigProvider.h"            
#include "L0Interfaces/IL0CondDBProvider.h"
#include "Event/L0DUBase.h"

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

  LHCb::L0DUConfig*  config(long tck = LHCb::L0DUTemplateConfig::TCKValue ,std::string slot="T0"){
    if (!m_uptodate) { reset();update(); }
    if( slot == "") slot = "T0";
    // first : handle TEMPLATE configuration
    if( m_template && tck == LHCb::L0DUTemplateConfig::TCKValue ){
        if ( msgLevel(MSG::DEBUG) ) 
          debug()<<"loading the TEMPLATE Configuration" << endmsg;
        return m_config;
    }

    if( tck != m_tckopts)warning() << "The requested TCK " << tck  
                                   <<" is not consistent with the options definition " 
                                   << m_tckopts << " CHECK your options ! " << endmsg;
    if(tck < 0 || tck > 0xFFFF){
      warning() << "requested TCK is not a 16 bit word" << endmsg;
      return 0;
    }
    std::map<std::string,LHCb::L0DUConfigs*>::iterator it = m_configs.find( slot );
    if( it == m_configs.end() )createConfig(slot);
      
    it = m_configs.find( slot );
    if( it == m_configs.end() ){ // if re-creating failed (paranoid test)
      warning() << " no configs container found for slot " << slot << endmsg;
      return 0;
    }

    // it exists
    return m_configs[slot]->object(tck); 
  }; 

  LHCb::L0DUConfigs*  configs(std::string slot="T0"){
    if (!m_uptodate) { reset();update(); }
    if( slot == "") slot = "T0";
    return m_configs[slot];
  };
  
  

protected:


private:
  void handler(Property&);
  StatusCode update();
  void reset();

  void createConfig(std::string slot="T0");
  std::vector<std::string> Parse(std::string flag, std::vector<std::string> config );
  typedef std::vector<std::vector<std::string> > ConfigType;
  typedef ConfigType::iterator ConfigIterator;
  
  std::map<std::string,std::string> m_sepMap;
  std::map<std::string,int> m_constData;
  ConfigType m_data;
  ConfigType m_conditions;
  ConfigType m_channels;
  ConfigType m_triggers;

  std::string m_tck;
  std::string m_def;
  std::string m_recipe;
  bool m_detail;
  

  void printConfig(LHCb::L0DUConfig config,std::string slot);
  void constantData();
  void predefinedData();
  StatusCode createData();
  StatusCode createConditions();
  StatusCode createChannels();
  StatusCode createTriggers();
  void predefinedTriggers();
  bool getDataList(const std::string, std::vector<std::string>& );
  std::vector<std::string> triggerNameFromData( std::vector<std::string> );  
  bool configChecker();
  bool conditionCheck(LHCb::L0DUElementaryCondition* condition);
  bool conditionOrdering();

  //options-defined data/conditions/channels
  LHCb::L0DUElementaryData::Map m_dataMap;
  LHCb::L0DUElementaryCondition::Map m_conditionsMap;
  LHCb::L0DUChannel::Map m_channelsMap;
  LHCb::L0DUTrigger::Map m_triggersMap;

  std::map<std::string,LHCb::L0DUConfigs*> m_configs;

  LHCb::L0DUConfig*  m_config;

  IL0CondDBProvider* m_condDB;  

  unsigned long m_cData;
  int m_pData;
  long m_tckopts;
  bool m_template; 
  bool m_uptodate;
  bool m_forceOrder;
  std::vector<std::vector<LHCb::L0DUElementaryCondition*> > m_condOrder;
  std::vector<int> m_condMax;
  bool m_reOrder;
  bool m_check;
  unsigned int m_reported;
  std::vector<int> m_FOIx;
  std::vector<int> m_FOIy;
};
#endif // L0DUCONFIGPROVIDER_H
