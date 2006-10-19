// $Id: RawEventPrintout.h,v 1.2 2006-10-19 09:07:41 frankb Exp $
//====================================================================
//	RawEventPrintout.h
//--------------------------------------------------------------------
//
//====================================================================
#ifndef MDF_RAWEVENTPRINTOUT_H
#define MDF_RAWEVENTPRINTOUT_H 1

#include "Event/RawBank.h"
#include "Event/RawEvent.h"

#include <sstream>
#include <iomanip>

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  struct RawEventPrintout  {

    static std::string bankHeader(const RawBank* r)  {
      std::stringstream s; 
      s << "Size:"    << std::setw(4) << int(r->size()) 
        << " Type:"   << std::setw(2) << int(r->type())
        << ":"        << std::setw(5) << bankType(r->type())
        << " Source:" << std::setw(3) << int(r->sourceID())
        << " Vsn:"    << std::setw(2) << int(r->version()) 
        << " "        << std::hex << r->magic();
      return s.str();
    }
    static std::string bankType(const RawBank* r)  {
      if ( r ) return bankType(r->type());
      return "BAD_BANK";
    }
    static std::string bankType(int i)  {
#define PRINT(x)  case RawBank::x : return #x;
      switch(i)  {
        PRINT(L0Calo);
        PRINT(L0DU);
        PRINT(PrsE);
        PRINT(EcalE);
        PRINT(HcalE);
        PRINT(PrsTrig);
        PRINT(EcalTrig);
        PRINT(HcalTrig);
        PRINT(Velo);
        PRINT(Rich);
        PRINT(TT);
        PRINT(IT);
        PRINT(OT);
        PRINT(Muon);
        PRINT(L0PU);
        PRINT(DAQ);
        PRINT(ODIN);
        PRINT(HLT);
        PRINT(VeloFull);
        PRINT(TTFull);
        PRINT(ITFull);
        PRINT(EcalPacked);
        PRINT(HcalPacked);
        PRINT(PrsPacked);
        PRINT(L0Muon);
        PRINT(ITError);
        PRINT(TTError);
        PRINT(ITPedestal);
        PRINT(TTPedestal);
        PRINT(VeloError);
        PRINT(VeloPedestal);
        PRINT(VeloProcFull);
        default: return "UNKNOWN";
#undef PRINT
      }
      return "UNKNOWN";
    }
  };
}   // End namespace LHCb
#endif // MDF_RAWEVENTPRINTOUT_H
