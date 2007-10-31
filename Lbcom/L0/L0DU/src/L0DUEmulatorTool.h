// $Id: L0DUEmulatorTool.h,v 1.1 2007-10-31 15:04:45 odescham Exp $
#ifndef L0DUEMULATORTOOL_H 
#define L0DUEMULATORTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interfaces
#include "L0Interfaces/IL0DUEmulatorTool.h"            
#include "L0Interfaces/IL0ProcessorDataDecoder.h"
#include "L0Interfaces/IL0CondDBProvider.h"


/** @class L0DUEmulatorTool L0DUEmulatorTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-10
 */
class L0DUEmulatorTool : public GaudiTool, virtual public IL0DUEmulatorTool {
public: 
  /// Standard constructor
  L0DUEmulatorTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~L0DUEmulatorTool( ); ///< Destructor

  virtual StatusCode initialize();
  StatusCode process(LHCb::L0DUConfig* config, LHCb::L0ProcessorDatas* datas);
  StatusCode process(LHCb::L0DUConfig* config, std::vector<std::string> dataLocs);  
  const std::vector<unsigned int> bank(unsigned int version);
  const LHCb::L0DUReport report();
  
protected:
private:
  const unsigned long digit(const unsigned int   base[L0DUBase::Index::Size]);
  const double scale (const unsigned int   base[L0DUBase::Index::Size]);
  void setDataValue(LHCb::L0DUElementaryData* data, 
                    const unsigned int  base[L0DUBase::Index::Size]);
  StatusCode processing();
  StatusCode fillData();
  StatusCode dataTree(LHCb::L0DUElementaryData* data, LHCb::L0DUElementaryData::Map dataMap);

  bool cleanMuon(int i, int j);

  //
  LHCb::L0DUReport             m_report;              ///<  L0DU Report
  std::map<std::string,int>    m_muonMap;             ///<  Mapping of L0 muon input
  bool                         m_muonCleaning;        ///<  Activate muon cleaning
  bool                         m_muZeroSup;           ///<  Activate muon 0-suppression
  std::vector<int>             m_muonMaxIndices;      ///<  Array of muon index in decreasing-Pt order
  long                         m_muonCleaningPattern; ///<  Pattern of muon cleaning 
  IL0ProcessorDataDecoder*  m_decoder;
  IL0CondDBProvider*        m_condDB;  
  LHCb::L0DUConfig*         m_config;

};
#endif // L0DUEMULATORTOOL_H
