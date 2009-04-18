// $Id: L0DUEmulatorTool.h,v 1.4 2009-04-18 23:21:43 odescham Exp $
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
  const LHCb::L0DUReport emulatedReport();
  
protected:
private:
  unsigned long digit(const unsigned int   base[L0DUBase::Index::Size]);
  double        scale(const unsigned int   base[L0DUBase::Index::Size]);
  long          max  (const unsigned int   base[L0DUBase::Index::Size]);
  void setDataValue(LHCb::L0DUElementaryData* data, 
                    const unsigned int  base[L0DUBase::Index::Size]);
  StatusCode processing();
  StatusCode fillData();
  StatusCode dataTree(LHCb::L0DUElementaryData* data, LHCb::L0DUElementaryData::Map dataMap);

  //
  LHCb::L0DUReport             m_report;              ///<  L0DU Report
  bool                         m_muCleaning;          ///<  Activate muon cleaning
  bool                         m_muZeroSup;           ///<  Activate muon 0-suppression
  std::vector<int>             m_muHighest;           ///<  Array of  index of the 3 highest muon (Pt-decreasing order)
  int                          m_muPattern;           ///<  Pattern of muon cleaning 
  int                          m_nMu;
  IL0ProcessorDataDecoder*  m_decoder;
  IL0CondDBProvider*        m_condDB;  
  LHCb::L0DUConfig*         m_config;

};
#endif // L0DUEMULATORTOOL_H
