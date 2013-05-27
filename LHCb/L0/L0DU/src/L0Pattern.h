#ifndef L0PATTERN_H 
#define L0PATTERN_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// Interface
#include "L0Interfaces/IL0DUConfigProvider.h"
#include "L0Interfaces/IL0DUEmulatorTool.h"
#include "L0Interfaces/IL0DUFromRawTool.h"

/** @class L0Pattern L0Pattern.h
 *  L0 Processors (L0Calo/L0Muon/PUVETO) -> L0DU 
 *  bit assignment from EDMS 528259
 *
 *  @author Olivier Deschamps
 *  @date   2005-01-28
 */
class L0Pattern : public GaudiTupleAlg{
public:
  /// Standard constructor
  L0Pattern( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0Pattern( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  void encode(unsigned int data ,  const unsigned int base[L0DUBase::Index::Size]);


  // Tools
  IL0DUConfigProvider* m_config;
  IL0DUEmulatorTool*   m_emulator;
  IL0DUFromRawTool*    m_fromRaw;
  LHCb::L0ProcessorDatas* m_datas;
  unsigned long m_bcid;
  //
  std::string m_emulatorTool;
  std::string m_fromRawTool;
  std::string m_configTool;
  std::vector<std::string> m_list;
  bool m_setbcid;


};
#endif // L0PATTERN_H
