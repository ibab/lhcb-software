// $Id: L0DUFromRawTool.h,v 1.16 2010-01-29 07:54:33 graven Exp $
#ifndef L0DUFROMRAWTOOL_H 
#define L0DUFROMRAWTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interface
#include "L0Interfaces/IL0DUFromRawTool.h"            
#include "L0Interfaces/IL0DUConfigProvider.h"
#include "L0Interfaces/IL0DUEmulatorTool.h"
#include "L0Interfaces/IL0CondDBProvider.h"
// from Event
#include "Event/RawEvent.h"
#include "Event/RawBankReadoutStatus.h"
#include "DAQKernel/DecoderToolBase.h"


/** @class L0DUFromRawTool L0DUFromRawTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-19
 */
class L0DUFromRawTool : public extends<Decoder::ToolBase, IL0DUFromRawTool> {
public: 
  /// Standard constructor
  L0DUFromRawTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  StatusCode initialize() override;
  bool decodeBank(int ibank=0 );

  unsigned int data(const std::string& name) const;
  double dataScaled(const std::string& name) const;
  unsigned int version() const {return m_vsn;};
  unsigned int tck() const {  return m_tck; }
  unsigned int firmware() const {  return m_pgaVsn; }
  const std::pair<unsigned int,unsigned int> bcid() const {  return {m_bcid2,m_bcid3}; }
  unsigned int rsda() const {  return m_rsda; }
  unsigned int muonCleaningPattern() const {  return m_muCleanPattern; }
  unsigned int status() const {  return m_status; }
  unsigned int size() const {return m_size;  }
  unsigned long roStatus() const {return m_roStatus.status();  }
  void fillDataMap(bool fill = true){m_fill = fill;}
  std::string dump() const ;

  LHCb::L0DUReport report() const {return m_report;}
  LHCb::L0ProcessorDatas* L0ProcessorDatas(){return m_processorDatas.get();}
  const std::map<std::string, std::pair<unsigned int,double> >& datas() const {return m_dataMap;}
  
  virtual StatusCode  _setProperty(const std::string& p,const std::string& v){return  setProperty(p,v);};
  
private:
  bool decoding(int ibank);
  bool getL0DUBanksFromRaw();
  inline void encode(const std::string& name, unsigned int data ,  const unsigned int base[L0DUBase::Index::Size],int bx=0);
  inline void dataMap(const std::string& name, unsigned int data , double scale = 1.);
  void fillBCIDData();
  double scale(const unsigned int base[L0DUBase::Index::Size]) const;
  inline bool nextData();
  void putStatusOnTES();
  //
  std::string m_emulatorType;
  std::string m_configName;
  std::string m_configType;
  bool m_muonNoZsup;
  int m_force;
  int m_sumSize;
  

  IL0DUConfigProvider*   m_confTool = nullptr;
  IL0DUEmulatorTool*     m_emuTool = nullptr;
  IL0CondDBProvider*     m_condDB = nullptr;
  //
  const std::vector<LHCb::RawBank*>* m_banks;  
  std::map<std::string, std::pair<unsigned int,double> > m_dataMap;
  unsigned int m_vsn = 0;
  unsigned int m_status;
  unsigned int m_pgaVsn;
  unsigned int m_bcid2;
  unsigned int m_bcid3;
  unsigned int m_rsda;
  unsigned int m_muCleanPattern;
  std::map<std::pair<int , unsigned int>, unsigned int > m_ecs;
  std::map<std::pair<int , unsigned int>, unsigned int > m_cds;
  std::map<std::pair<int , unsigned int>, unsigned int > m_tcs;
  std::map<int , unsigned int> m_sumEt;
  
  LHCb::L0DUReport m_report;
  std::unique_ptr<LHCb::L0ProcessorDatas> m_processorDatas;
  unsigned int m_tck;  
  bool m_warning = true;
  unsigned int m_size = 0;
  LHCb::RawBankReadoutStatus m_roStatus;
  //
  unsigned int* m_data = nullptr;
  unsigned int  m_source = 0;
  bool m_warn;
  bool m_fill;
  bool m_emu;
  bool m_encode;
  bool m_stat;
  std::string m_slot = "T0";
  std::vector<unsigned int> m_dump;
  int m_dumping = -1;
  int m_count = 0;
  
};
#endif // L0DUFROMRAWTOOL_H




inline void L0DUFromRawTool::encode(const std::string& name, unsigned int data ,  const unsigned int base[L0DUBase::Index::Size],int bx){
  if(name != "")dataMap(name,data,scale(base));
  if(!m_encode)return;
  LHCb::L0ProcessorData* fiber = m_processorDatas->object( base[ L0DUBase::Index::Fiber ]  )  ;
  unsigned int word = fiber->word(bx);  
  //  word |= ( (data << base[L0DUBase::Index::Shift]) & base[L0DUBase::Index::Mask] );
  word |= ( (data & ( base[L0DUBase::Index::Mask] >> base[L0DUBase::Index::Shift] )) << base[L0DUBase::Index::Shift]);
  fiber->setWord( word,bx);
  if( L0DUBase::Fiber::Empty != base[ L0DUBase::Index::Fiber2 ]  ) {
    fiber = m_processorDatas->object( base[ L0DUBase::Index::Fiber2 ]  )  ;
    word = fiber->word(bx);
    unsigned int val = data >> base[L0DUBase::Index::Offset];
    word |= ( ( val << base[L0DUBase::Index::Shift2]) & base[L0DUBase::Index::Mask2] );
    fiber->setWord( word,bx);
  }
}

inline void L0DUFromRawTool::dataMap(const std::string& name, unsigned int data,double scale){
  if(m_fill) m_dataMap[name]=std::make_pair(data,scale);
}

inline bool L0DUFromRawTool::nextData(){
  if( NULL == ++m_data){
    Error("READOUTSTATUS : No more data in bank --> CORRUPTION",StatusCode::SUCCESS).ignore();
    m_roStatus.addStatus( m_source , LHCb::RawBankReadoutStatus::Corrupted );
    m_roStatus.addStatus( m_source , LHCb::RawBankReadoutStatus::Incomplete);
    return false;
  }
  if ( msgLevel( MSG::VERBOSE) )verbose() << "data = " <<  format("0x%04X", *m_data) << endmsg;
  m_dump.push_back(*m_data);
  return true;
}
