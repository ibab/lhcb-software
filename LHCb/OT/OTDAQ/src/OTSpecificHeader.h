// $Id: OTSpecificHeader.h,v 1.1 2007-12-12 13:05:31 wouter Exp $
#ifndef OTDAQ_OTSPECIFICHEADER_H 
#define OTDAQ_OTSPECIFICHEADER_H 1

// Include files
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

// Forward declarations

namespace OTDAQ {

  class OTSpecificHeader
  {
  private:
    unsigned int m_data ;
  public:
    /// 
    //    static size_t BitsNumberOfGOLs = 16 ;
    //     static size_t BitsBunchCounter =  8 ;
    //     static size_t BitsErrorFlag    =  1 ;
    //     static size_t BitsTriggerType  =  3 ;
    
    /// Bitfield definition
    enum Shifts { NumberOfGOLsShift=0,
		  BunchCounterShift=16,
		  ErrorShift=24,
		  TriggerTypeShift=25 } ;
    
    enum Masks   { NumberOfGOLsMask=0x0000FFFFL,
		   BunchCounterMask=0x00FF0000L,
		   ErrorMask       =0x01000000L,
		   TriggerTypeMask =0x0E000000L } ;

    /// constructor with word, station, layer, quarter, module, otisErFlag and size
    OTSpecificHeader(unsigned int iError,
		     unsigned int iTriggerType,
		     unsigned int iBunchCounter,
		     unsigned int iNumberOfGOLs) 
      : m_data( (iError << NumberOfGOLsShift) + 
		(iTriggerType << TriggerTypeShift) +
		(iBunchCounter << BunchCounterShift) + 
		(iNumberOfGOLs << NumberOfGOLsShift) ) {};
    
    /// constructor with golHeader
    OTSpecificHeader(unsigned int data) : m_data(data) {}
    
    unsigned int numberOfGOLs() const { return (m_data&NumberOfGOLsMask)>>NumberOfGOLsShift ; }
    unsigned int bunchCounter() const { return (m_data&BunchCounterMask)>>BunchCounterShift ; }
    unsigned int error() const { return (m_data&ErrorMask)>>ErrorShift ; }
    unsigned int triggerType() const { return  (m_data&TriggerTypeMask)>>TriggerTypeShift ; }
    
    
    friend inline std::ostream& operator << ( std::ostream& os, const OTSpecificHeader& header)
    {
      return os << "[ triggerType = " << header.triggerType() 
		<< ", error = " << header.error() 
		<< ", bunchCounter = " << header.bunchCounter()
		<< ", numberOfGOLs = " << header.numberOfGOLs() << " ] " ;
    }
  } ;
}



#endif // GOLHEADERV3_H
