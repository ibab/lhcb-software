// $Id: EvtInfo.h,v 1.3 2007-09-18 09:10:46 cattanem Exp $
#ifndef EVTINFO_H 
#define EVTINFO_H 1

// Include files
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"
#include "Tell1Kernel/VeloDecodeCore.h"

/** @class EvtInfo EvtInfo.h
 *  
 * The class implements object that can store data contained in
 * EventInfo block placed at the end of the non-zero suppressed
 * data block in VeloFull raw bank. For each sensor we have four
 * EventInfo blocks (one per each PP-FPGA). The class inherits
 * from KeyedObject with sensor (TELL1 number) as a key.
 *
 *  @author Tomasz Szumlak
 *  @date   2006-04-27
 */

namespace EvtInfoLocation {
  static const std::string& Default = "Raw/Velo/EvtInfo";
}

class EvtInfo: public KeyedObject<int>{
public:
  ///
  EvtInfo(const int numberOfTELL1):
    KeyedObject<int>(numberOfTELL1),
    m_evtInfo ( )
  { }
  /// Standard constructor
  EvtInfo(){ }
  virtual ~EvtInfo( ){ }; ///< Destructor
  //  
  void setEvtInfo(VeloTELL1::allEvt& inData);
  unsigned int bunchCounter(const int PPFPGA=0) const;
  unsigned int detectorID(const int PPFPGA=0) const;
  unsigned int bankList(const int PPFPGA=0) const;
  unsigned int eventInformation(const int PPFPGA=0) const;
  unsigned int l0EventID(const int PPFPGA=0) const;
  unsigned int FEMPCN(const int PPFPGA=0) const;
  unsigned int processInfo(const int PPFPGA=0) const;
  unsigned int checker(const int PPFPGA=0) const;
  unsigned int chipAddress(const int PPFPGA=0) const;
  unsigned int channelError(const int PPFPGA=0) const;
  unsigned int adcFIFOError(const int PPFPGA=0) const;
  unsigned int headerPseudoErrorFlag(const int PPFPGA=0) const;
  unsigned int PCNError(const int PPFPGA=0) const;
  unsigned int FEMPCNG(const int PPFPGA=0) const;
  VeloTELL1::dataVec IHeader(const int PPFPGA=0) const;
  VeloTELL1::dataVec PCNBeetle(const int PPFPGA=0) const;

protected:

private:

  VeloTELL1::allEvt m_evtInfo;

};
//
inline void EvtInfo::setEvtInfo(VeloTELL1::allEvt& inData)
{
  m_evtInfo=inData;
}
//
inline unsigned int EvtInfo::bunchCounter(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( anInfo[0]&VeloTELL1::bitMask12 );
}
//
inline unsigned int EvtInfo::detectorID(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[0]>>VeloTELL1::bitShift12)&VeloTELL1::bitMask4 );
}
//
inline unsigned int EvtInfo::bankList(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[0]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask8 );
}
//
inline unsigned int EvtInfo::eventInformation(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[0]>>VeloTELL1::bitShift24)&VeloTELL1::bitMask8 );
}
//
inline unsigned int EvtInfo::l0EventID(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( anInfo[1] );
}
//
inline unsigned int EvtInfo::FEMPCN(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[2])&VeloTELL1::bitMask8 );
}
//
inline unsigned int EvtInfo::processInfo(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[2]>>VeloTELL1::bitShift8)&VeloTELL1::bitMask8 );
}
//
inline unsigned int EvtInfo::chipAddress(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[3]>>VeloTELL1::bitShift30)&VeloTELL1::bitMask2 );
}
//
inline unsigned int EvtInfo::channelError(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[3]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask4 );
}
//
inline unsigned int EvtInfo::adcFIFOError(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[3])&VeloTELL1::bitMask16 );
}
inline unsigned int EvtInfo::headerPseudoErrorFlag(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[4]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask16 );
}
//
inline unsigned int EvtInfo::PCNError(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[4]>>VeloTELL1::bitShift8)&VeloTELL1::bitMask4 );
}
//
inline unsigned int EvtInfo::FEMPCNG(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[4])&VeloTELL1::bitMask8 );
}
// method to check if decoding is working
inline unsigned int EvtInfo::checker(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  //
  return ( (anInfo[2]>>VeloTELL1::bitShift16)&VeloTELL1::bitMask16 );
}
//
inline VeloTELL1::dataVec EvtInfo::IHeader(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  VeloTELL1::dataVec IHeaders;
  for(int headNumber=0; headNumber<4; headNumber++){
    IHeaders.push_back((anInfo[5]>>(headNumber*VeloTELL1::bitShift8))&VeloTELL1::bitMask8);
  }
  return ( IHeaders );
}
//
inline VeloTELL1::dataVec EvtInfo::PCNBeetle(const int PPFPGA) const
{
  VeloTELL1::evtInfo anInfo=m_evtInfo[PPFPGA];
  VeloTELL1::dataVec PCNBeetles;
  for(int PCNBeetle=0; PCNBeetle<4; PCNBeetle++){
    PCNBeetles.push_back((anInfo[6]>>(PCNBeetle*VeloTELL1::bitShift8))&VeloTELL1::bitMask8);
  }
  return ( PCNBeetles );
}
//
typedef KeyedContainer<EvtInfo, Containers::HashMap> EvtInfos;
#endif // EVTINFO_H
