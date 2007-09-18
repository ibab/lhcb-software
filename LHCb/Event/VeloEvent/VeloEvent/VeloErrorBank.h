// $Id: VeloErrorBank.h,v 1.1 2007-09-18 00:11:45 szumlat Exp $
#ifndef VELOERRORBANK_H 
#define VELOERRORBANK_H 1

#include <stdexcept>
#include <exception>

// Include files
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"
#include "Kernel/VeloDecodeCore.h"
#include "VeloEvent/EvtInfo.h"

/** @class VeloErrorBank VeloErrorBank.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-05-02
 */

using namespace VeloTELL1;

namespace VeloErrorBankLocation {
  static const std::string& Default = "Raw/Velo/VeloErrorBank";
}

class VeloErrorBank: public KeyedObject<int> {
public:
  /// Standard constructor
  VeloErrorBank(const int numberOfTELL1):
    KeyedObject<int>(numberOfTELL1),
    m_evtInfoData(numberOfTELL1)
  { }

  virtual ~VeloErrorBank( ){ } ///< Destructor
  void setEvtInfoSection(EvtInfo& inInfo);
  void setErrorInfoSection(allError& inSec);
  void setErrorSources(dataVec& sources);
  unsigned int eventInformation(const int PPFPGA=0) const;
  unsigned int bankList(const int PPFPGA=0) const;
  unsigned int detectorID(const int PPFPGA=0) const;
  unsigned int bunchCounter(const int PPFPGA=0) const;
  unsigned int l0EventID(const int PPFPGA=0) const;
  bool isEmpty() const;
  EvtInfo* evtInfo() const;
  dataVec errorSources() const;
  unsigned int clusterDataSectionLength(const int PPFPGA=0) const;
  unsigned int adcDataSectionLength(const int PPFPGA=0) const;
  unsigned int processInfo(const int PPFPGA=0) const;
  unsigned int PCN(const int PPFPGA=0) const;
  unsigned int numberOfCluster(const int PPFPGA=0) const;
  unsigned int errorBankLength(const int PPFPGA=0) const;
  unsigned int bankLength(const int PPFPGA=0) const;

protected:

private:

  EvtInfo m_evtInfoData;
  allError m_errorInfoData;
  dataVec m_errorSources;

};
//
inline void VeloErrorBank::setEvtInfoSection(EvtInfo& inInfo)
{
  m_evtInfoData=inInfo;
}
//
inline void VeloErrorBank::setErrorInfoSection(allError& inSec)
{
  m_errorInfoData=inSec;
}
//
inline void VeloErrorBank::setErrorSources(dataVec& sources)
{
  m_errorSources=sources;
}
//
inline unsigned int VeloErrorBank::eventInformation(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[0]>>bitShift24)&bitMask8 );
}
//
inline unsigned int VeloErrorBank::bankList(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[0]>>bitShift16)&bitMask8 );
}
//
inline unsigned int VeloErrorBank::detectorID(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[0]>>bitShift12)&bitMask4 );
}
//
inline unsigned int VeloErrorBank::bunchCounter(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( anInfo[0]&bitMask12 );
}
//
inline unsigned int VeloErrorBank::l0EventID(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( anInfo[1] );
}
//
inline bool VeloErrorBank::isEmpty() const
{
  return ( (m_errorSources.size()==0) ? true : false );
}
//
inline EvtInfo* VeloErrorBank::evtInfo() const
{
  EvtInfo* anInfo=new EvtInfo(m_evtInfoData.key());
  (*anInfo)=m_evtInfoData;
  //
  return ( anInfo );
}
//
inline dataVec VeloErrorBank::errorSources() const
{
  return ( m_errorSources );
}
//
inline unsigned int 
VeloErrorBank::clusterDataSectionLength(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[2]>>bitShift16)&bitMask16 );
}
//
inline unsigned int
VeloErrorBank::adcDataSectionLength(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( anInfo[2]&bitMask16 );
}
//
inline unsigned int VeloErrorBank::processInfo(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[3]>>bitShift24)&bitMask8 );
}
//
inline unsigned int VeloErrorBank::PCN(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[3]>>bitShift16)&bitMask8 );
}
//
inline unsigned int VeloErrorBank::numberOfCluster(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( anInfo[3]&bitMask16 );
}
//
inline unsigned int VeloErrorBank::errorBankLength(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  // 
  try{
    if(((anInfo[4]>>bitShift8)&bitMask8)==0x8e){
      std::cout<< " ==> Data structure is coherent up to first marker! ";
    }else{
      throw std::string(" ==> Error Bank data corruption at first marker!");
    }
  }catch (std::string& str){
    std::cout<< ( str ) <<std::endl;
  }
  //
  return ( (anInfo[4]>>bitShift16)&bitMask16 );
}
inline unsigned int VeloErrorBank::bankLength(const int PPFPGA) const
{
  errorInfo anInfo=m_errorInfoData[PPFPGA];
  try{
    if((anInfo[7]&bitMask8)==0x03){
      std::cout<< " non-zero suppressed data bank length: ";
    }else if((anInfo[7]&bitMask8)==0x04){
      std::cout<< " pedestal bank length: ";
    }else{
      throw std::string(" ==> Error Bank data corruption (second marker)!");
    }
  }catch (std::string& str){
    std::cout<< ( str ) <<std::endl;
  }
  //
  return ( (anInfo[7]>>bitShift16)&bitMask16 );
}
//
typedef KeyedContainer<VeloErrorBank, Containers::HashMap> VeloErrorBanks;
#endif // VELOERRORBANK_H
