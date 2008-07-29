// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/RawEventPrintout.cpp,v 1.3 2008-07-29 07:48:05 cattanem Exp $
//  ====================================================================
//  MIFWriter.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "MDF/RawEventPrintout.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

#include <sstream>
#include <iomanip>

using namespace LHCb;

std::string RawEventPrintout::bankHeader(const RawBank* r)  {
  std::stringstream s; 
  s << "Size:"    << std::setw(4) << int(r->size()) 
    << " Type:"   << std::setw(2) << int(r->type())
    << ":"        << std::setw(5) << bankType(r->type())
    << " Source:" << std::setw(3) << int(r->sourceID())
    << " Vsn:"    << std::setw(2) << int(r->version()) 
    << " "        << std::hex << r->magic();
  return s.str();
}

std::string RawEventPrintout::bankType(const RawBank* r)  {
  if ( r ) return bankType(r->type());
  return "BAD_BANK";
}

std::string RawEventPrintout::bankType(int i)  {
#define PRINT(x)  case RawBank::x : return #x;
  switch(i)  {
    PRINT(L0Calo);          //  0
    PRINT(L0DU);            //  1
    PRINT(PrsE);            //  2
    PRINT(EcalE);           //  3
    PRINT(HcalE);           //  4
    PRINT(PrsTrig);         //  5
    PRINT(EcalTrig);        //  6
    PRINT(HcalTrig);        //  7
    PRINT(Velo);            //  8
    PRINT(Rich);            //  9
    PRINT(TT);              // 10
    PRINT(IT);              // 11
    PRINT(OT);              // 12
    PRINT(Muon);            // 13
    PRINT(L0PU);            // 14
    PRINT(DAQ);             // 15
    PRINT(ODIN);            // 16
    PRINT(HltDecReports);   // 17
    PRINT(VeloFull);        // 18
    PRINT(TTFull);          // 19
    PRINT(ITFull);          // 20
    PRINT(EcalPacked);      // 21
    PRINT(HcalPacked);      // 22
    PRINT(PrsPacked);       // 23
    PRINT(L0Muon);          // 24
    PRINT(ITError);         // 25
    PRINT(TTError);         // 26
    PRINT(ITPedestal);      // 27
    PRINT(TTPedestal);      // 28
    PRINT(VeloError);       // 29
    PRINT(VeloPedestal);    // 30
    PRINT(VeloProcFull);    // 31
    PRINT(OTRaw);           // 32
    PRINT(OTError);         // 33
    PRINT(EcalPackedError); // 34
    PRINT(HcalPackedError); // 35  
    PRINT(PrsPackedError);  // 36
    PRINT(L0CaloFull);      // 37
    PRINT(L0CaloError);     // 38
    PRINT(L0MuonCtrlAll);   // 39
    PRINT(L0MuonProcCand);  // 40
    PRINT(L0MuonProcData);  // 41
    PRINT(L0MuonRaw);       // 42
    PRINT(L0MuonError);     // 43
    PRINT(GaudiSerialize);  // 44
    PRINT(GaudiHeader);     // 45
    PRINT(TTProcFull);      // 46
    PRINT(ITProcFull);      // 47
    PRINT(TAEHeader);       // 48
    PRINT(MuonFull);        // 49
    PRINT(MuonError);       // 50
    PRINT(TestDet);         // 51
    PRINT(L0DUError);       // 52
    PRINT(HltRoutingBits);  // 53
    PRINT(HltSelReports);   // 54
    PRINT(HltVertexReports);// 55
    PRINT(HltLumiSummary);  // 56

    default: return "UNKNOWN";
#undef PRINT
  }
  return "UNKNOWN";
}
