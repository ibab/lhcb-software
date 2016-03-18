// $Id: L0ProcessorDataDecoder.h,v 1.5 2008-01-29 16:03:36 odescham Exp $
#ifndef L0DU_L0PROCESSORDATADECODER_H
#define L0DU_L0PROCESSORDATADECODER_H

// from GaudiKernel
#include "GaudiAlg/GaudiTool.h"
// Interfaces
#include  "L0Interfaces/IL0ProcessorDataDecoder.h"
#include  "L0Interfaces/IL0CondDBProvider.h"

class L0ProcessorDataDecoder : public GaudiTool, virtual public IL0ProcessorDataDecoder{
 public:
  
  L0ProcessorDataDecoder ( const std::string& type   , 
                   const std::string& name   ,
                   const IInterface*  parent ) ;

  virtual ~L0ProcessorDataDecoder () ;
  
  virtual StatusCode initialize () ;
  virtual StatusCode finalize () ;
  bool   setL0ProcessorData(std::vector<std::string> dataLocs );
  bool   setL0ProcessorData(std::vector<LHCb::L0ProcessorDatas*> datass );
  bool   setL0ProcessorData(std::string dataLoc );
  bool   setL0ProcessorData(LHCb::L0ProcessorDatas* datas );
  unsigned long digit( const unsigned int   data[L0DUBase::Index::Size],int bx=0);
  double value( const unsigned int base[L0DUBase::Index::Size],int bx=0);
  std::vector<int> bxList( const unsigned int base[L0DUBase::Index::Size]);
  bool   isValid(){return m_ok;};

private:
  LHCb::L0ProcessorDatas* m_dataContainer;
  bool   m_ok;
  IL0CondDBProvider* m_condDB;
};

#endif // L0DU_L0PROCESSORDATADECODER_H 
