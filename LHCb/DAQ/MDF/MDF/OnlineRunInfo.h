// $Id: OnlineRunInfo.h,v 1.2 2007-12-14 11:42:22 frankb Exp $
//====================================================================
//	OnlineRunInfo.h
//--------------------------------------------------------------------
//
//====================================================================
#ifndef MDF_ONLINERUNINFO_H
#define MDF_ONLINERUNINFO_H
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/OnlineRunInfo.h,v 1.2 2007-12-14 11:42:22 frankb Exp $

// Framework include files
#include "Event/RawBank.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations
  class RawBank;

  /** @class OnlineRunInfo OnlineRunInfo.h MDF/OnlineRunInfo.h
    *
    * Basic run information from ODIN bank.
    */
  struct OnlineRunInfo  {
    unsigned int  Run;
    short    int  EventType;
    short    int  CalibrationStep;
    unsigned int  Orbit;
    long long int L0ID;
    long long int GPSTime;
    unsigned      detStatus   : 24;
    unsigned      errors      :  8;
    unsigned      bunchID     : 12;
    unsigned      pad0        :  4;
    unsigned      triggerType :  3;
    unsigned      readoutType :  2;
    unsigned      forceBit    :  1;
    unsigned      bxType      :  2;
    unsigned      bunchCurrent:  8;
  };
}      // end namespace LHCb
#endif // MDF_ONLINERUNINFO_H
