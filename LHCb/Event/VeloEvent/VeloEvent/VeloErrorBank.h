// $Id: VeloErrorBank.h,v 1.5 2008-08-26 09:59:45 szumlat Exp $
#ifndef VELOERRORBANK_H 
#define VELOERRORBANK_H 1

#include <stdexcept>
#include <exception>
#include <string>
#include <ostream>

// Include files
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"
#include "Tell1Kernel/VeloDecodeCore.h"
#include "VeloEvent/EvtInfo.h"

/** @class VeloErrorBank VeloErrorBank.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-05-02
 */

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
  void setErrorInfoSection(VeloTELL1::allError& inSec);
  void setErrorSources(VeloTELL1::dataVec& sources);
  void setBankLength(const unsigned int bl);
  void setBankType(const unsigned int bt);
  void setBankVersion(const unsigned int bv);
  void setMagicPattern(const unsigned int mp);
  unsigned int bankLength() const;
  unsigned int bankType() const;
  unsigned int bankVersion() const;
  unsigned int magicPattern() const;
  unsigned int eventInformation(const int PPFPGA=0) const;
  unsigned int bankList(const int PPFPGA=0) const;
  unsigned int detectorID(const int PPFPGA=0) const;
  unsigned int bunchCounter(const int PPFPGA=0) const;
  unsigned int l0EventID(const int PPFPGA=0) const;
  bool isEmpty() const;
  EvtInfo* evtInfo() const;
  VeloTELL1::dataVec errorSources() const;
  unsigned int clusterDataSectionLength(const int PPFPGA=0) const;
  unsigned int adcDataSectionLength(const int PPFPGA=0) const;
  unsigned int processInfo(const int PPFPGA=0) const;
  unsigned int PCN(const int PPFPGA=0) const;
  unsigned int numberOfCluster(const int PPFPGA=0) const;
  unsigned int errorBankLength(const int PPFPGA=0) const;
  unsigned int nzsBankLength(const int PPFPGA=0) const;

protected:

private:

  EvtInfo m_evtInfoData;
  VeloTELL1::allError m_errorInfoData;
  VeloTELL1::dataVec m_errorSources;
  unsigned int m_bankLength;
  unsigned int m_bankType;
  unsigned int m_bankVersion;
  unsigned int m_magicPattern;

};
//
inline void VeloErrorBank::setEvtInfoSection(EvtInfo& inInfo)
{
  m_evtInfoData=inInfo;
}
//
inline void VeloErrorBank::setBankLength(unsigned int bl)
{
  m_bankLength=bl;
}
//
inline void VeloErrorBank::setBankType(unsigned int bt)
{
  m_bankType=bt;
}
//
inline void VeloErrorBank::setBankVersion(unsigned int bv)
{
  m_bankVersion=bv;
}
//
inline void VeloErrorBank::setMagicPattern(unsigned int mp)
{
  m_magicPattern=mp;
}
//
inline unsigned int VeloErrorBank::bankVersion() const
{
  return ( m_bankVersion );
}
//
inline unsigned int VeloErrorBank::bankLength() const
{
  return ( m_bankLength );
}
//
inline unsigned int VeloErrorBank::bankType() const
{
  return ( m_bankType );
}
//
inline unsigned int VeloErrorBank::magicPattern() const
{
  return ( m_magicPattern );
}
//
inline void VeloErrorBank::setErrorInfoSection(VeloTELL1::allError& inSec)
{
  m_errorInfoData=inSec;
}
//
inline void VeloErrorBank::setErrorSources(VeloTELL1::dataVec& sources)
{
  m_errorSources=sources;
}
//
inline unsigned int VeloErrorBank::eventInformation(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[0]>>VeloTELL1::bitShift24)&VeloTELL1::bitMask8 );
}
//
inline unsigned int VeloErrorBank::bankList(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[0]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask8 );
}
//
inline unsigned int VeloErrorBank::detectorID(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[0]>>VeloTELL1::bitShift12)&VeloTELL1::bitMask4 );
}
//
inline unsigned int VeloErrorBank::bunchCounter(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( anInfo[0]&VeloTELL1::bitMask12 );
}
//
inline unsigned int VeloErrorBank::l0EventID(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
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
inline VeloTELL1::dataVec VeloErrorBank::errorSources() const
{
  return ( m_errorSources );
}
//
inline unsigned int 
VeloErrorBank::clusterDataSectionLength(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[2]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask16 );
}
//
inline unsigned int
VeloErrorBank::adcDataSectionLength(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( anInfo[2]&VeloTELL1::bitMask16 );
}
//
inline unsigned int VeloErrorBank::processInfo(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[3]>>VeloTELL1::bitShift24)&VeloTELL1::bitMask8 );
}
//
inline unsigned int VeloErrorBank::PCN(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( (anInfo[3]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask8 );
}
//
inline unsigned int VeloErrorBank::numberOfCluster(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  //
  return ( anInfo[3]&VeloTELL1::bitMask16 );
}
//
inline unsigned int VeloErrorBank::errorBankLength(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  // 
  unsigned int errorBankLength=0;
  try{
    if(((anInfo[4])&VeloTELL1::bitMask8)==0x00){
      errorBankLength=(anInfo[4]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask16;
    }else{
      throw std::string(" ==> Error Bank data corruption at first marker!");
    }
  }catch (std::string& str){
    std::cout<< ( str ) <<std::endl;
  }
  //
  return ( errorBankLength );
}
inline unsigned int VeloErrorBank::nzsBankLength(const int PPFPGA) const
{
  VeloTELL1::errorInfo anInfo=m_errorInfoData[PPFPGA];
  unsigned int bankSize=0;
  try{
    if((anInfo[7]&VeloTELL1::bitMask8)==0x03){
      std::cout<< " non-zero suppressed data bank length: ";
      bankSize=(anInfo[7]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask16;
    }else if((anInfo[7]&VeloTELL1::bitMask8)==0x04){
      std::cout<< " pedestal bank length: " <<std::endl;
      bankSize=(anInfo[7]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask16;
    }else{
      throw std::string(" ==> No NZS data bank present in file");
    }
  }catch (std::string& str){
    std::cout<< ( str ) <<std::endl;
  }
  //
  return ( bankSize );
}
//
typedef KeyedContainer<VeloErrorBank, Containers::HashMap> VeloErrorBanks;
#endif // VELOERRORBANK_H
