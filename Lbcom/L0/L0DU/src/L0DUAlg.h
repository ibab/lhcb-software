// $Id: L0DUAlg.h,v 1.2 2008-01-29 16:02:29 odescham Exp $
#ifndef L0DUAlg_H 
#define L0DUAlg_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//Event
#include "Event/RawEvent.h"
// Interface
#include "L0Interfaces/IL0DUConfigProvider.h"
#include "L0Interfaces/IL0DUEmulatorTool.h"

/** @class L0DUAlg L0DUAlg.h
 *  L0DUAlg report
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2005-01-28
 */



class L0DUAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  L0DUAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~L0DUAlg( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
private:
  // I/O Locations  
  std::vector<std::string> m_dataLocations;
  
  // parameters
  unsigned int m_rawSrcID;
  std::string m_emulatorType;
  bool m_fillRaw;
  std::string m_rawLocation;
  bool m_writeOnTES;
  std::string m_reportLocation;
  std::string m_tck;
  std::string m_configName;

//
  unsigned int m_rawVsn ;  
  LHCb::RawBank::BankType m_rawBankType;

  // Configuration 
  LHCb::L0DUConfig* m_config;

  // Tools
  IL0DUConfigProvider* m_confTool;
  IL0DUEmulatorTool*   m_emulator;
  
  // PGA3 block building
  long m_muonWord[3];
  std::map<std::string,double> m_muonMap;
};
#endif // L0DUAlg_H
