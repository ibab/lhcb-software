// $Id: RawEventPrintout.h,v 1.3 2007-11-23 18:34:07 frankb Exp $
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
        PRINT(L0Calo);          // 0
        PRINT(L0DU);
        PRINT(PrsE);
        PRINT(EcalE);
        PRINT(HcalE);
        PRINT(PrsTrig);
        PRINT(EcalTrig);
        PRINT(HcalTrig);
        PRINT(Velo);
        PRINT(Rich);
        PRINT(TT);              //10
        PRINT(IT);
        PRINT(OT);
        PRINT(Muon);
        PRINT(L0PU);
        PRINT(DAQ);
        PRINT(ODIN);
        PRINT(HLT);
        PRINT(VeloFull);
        PRINT(TTFull);
        PRINT(ITFull);          // 20
        PRINT(EcalPacked);
        PRINT(HcalPacked);
        PRINT(PrsPacked);
        PRINT(L0Muon);
        PRINT(ITError);
        PRINT(TTError);
        PRINT(ITPedestal);
        PRINT(TTPedestal);
        PRINT(VeloError);
        PRINT(VeloPedestal);    // 30
        PRINT(VeloProcFull);
        PRINT(OTRaw);
        PRINT(OTError);
        PRINT(EcalPackedError); //34
        PRINT(HcalPackedError); //35  
        PRINT(PrsPackedError);  //36
        PRINT(L0CaloFull);      //37
        PRINT(L0CaloError);     //38
        PRINT(L0MuonCtrlAll);   //39
        PRINT(L0MuonProcCand);  //40
        PRINT(L0MuonProcData);  //41
        PRINT(L0MuonRaw);       //42
        PRINT(L0MuonError);     //43
        PRINT(GaudiSerialize);  //44
        PRINT(GaudiHeader);     //45
        PRINT(TTProcFull);      //46
        PRINT(ITProcFull);      //47

        default: return "UNKNOWN";
#undef PRINT
      }
      return "UNKNOWN";
    }
  };
}   // End namespace LHCb
#endif // MDF_RAWEVENTPRINTOUT_H
