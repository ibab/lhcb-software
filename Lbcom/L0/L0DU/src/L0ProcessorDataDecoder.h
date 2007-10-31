// $Id: L0ProcessorDataDecoder.h,v 1.1 2007-10-31 15:04:46 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/02/21 15:56:49  ocallot
// Decrease verbosity of L0DU initialisation
//
//
//
// ============================================================================
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
  bool   setL0ProcessorData(std::vector<std::string> dataLocs );
  bool   setL0ProcessorData(std::vector<LHCb::L0ProcessorDatas*> datass );
  bool   setL0ProcessorData(std::string dataLoc );
  bool   setL0ProcessorData(LHCb::L0ProcessorDatas* datas );
  unsigned long digit( const unsigned int   data[L0DUBase::Index::Size]);
  double value( const unsigned int base[L0DUBase::Index::Size]);
  bool   isValid(){return m_ok;};

private:
  LHCb::L0ProcessorDatas* m_dataContainer;
  bool   m_ok;
  IL0CondDBProvider* m_condDB;
};

#endif // L0DU_L0PROCESSORDATADECODER_H 
