//	====================================================================
//  MEPMaker.cpp
//	--------------------------------------------------------------------
//
//	Package   : MEPMaker
//
//	Author    : Niko Neufeld
//
//	====================================================================

// Framework include files
#include <string>
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "OnlineDummyReader/OnlineDummyReader.h"

// Event Model related classes
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "GaudiKernel/System.h"
#include "AIDA/IHistogram1D.h"

using LHCb::RawBank;
using LHCb::RawEvent;

	 
//static const char  * rawtypstr[] = { "L0CA", "L0DU", "PRSE", "ECAE", "HCAE", "PRTR",
//				     "ECTR", "HCTR", "VELO", "RICH",   "TT",   "IT", 
//				     "OT", "MUON", "L0PU",  "DAQ",  "ODIN",  "HLT",
//				     "VELOFULL", "TTFULL", "ITFULL", "ECALPCK", "HCALPCK",
//				     "PRSPCK", "L0MU", "ITERR", "TTERR", "ITPED", "TTPED",
//				     "VELOERR","VELOPED", "TOOLARGE" };
static const char  * rawtypstr[] = { "L0Calo", "L0DU", "PrsE", "EcalE", "HcalE", "PrsTrig",
				     "EcalTrig", "HcalTrig", "Velo", "Rich",   "TT",   "IT", 
				     "OT", "Muon", "L0PU",  "DAQ",  "ODIN",  "HLT",
				     "VeloFull", "TTFull", "ITFull", "EcalPacked", "HcalPacked",
				     "PrsPacked", "L0Muon", "ITError", "TTError", "ITPedestal", "TTPedestal",
				     "VeloError","VeloPedestal", "VeloProcFull", "OTRaw",
                                     "OTError", "EcalPackedError", "HcalPackedError",
                                     "PrsPackedError", "L0CaloFUll", "L0CaloError",
                                     "L0MuonCtrlAll", "L0MuonProcCand", "L0MuonProcData",
                                     "L0MuonRaw", "L0MuonError", "GaudiSerialize", "GaudiHeader",
                                     "TTProcFull", "ITProcFull", "LastType" };


int rawtyp2det[] = { 0,      1,      2,      3,      4,      2, 
		     3,      4,      5 /* VELO */,   6 /* RICH */,     
		     7 /* TT */, 8 /* IT */, 9 /* OT */, 10 /* MUON */,
		     11 /* L0PU */, 12 /* DAQ */, 13 /* ODIN */, 14 /* HLT */,
                     5 /* VELOFULL */, 7 /* TTFULL */, 8 /* ITFULL */, 
                     3, 4, 2 /* PrsPacked */, 15 /* L0MUON */, 
                     8, 7, 8, 7, 5, 5 };


char *rawsubdetstr[16] = { "L0CA", "L0DU", "PRSH", "ECAL", "HCAL", "VELO", 
                           "RICH",   "TT",   "IT",   "OT", "MUON", "L0PU",  
                           "DAQ",  "ODIN",   "HLT", "L0MU"};


/* map bank-types to TELL1s */

namespace LHCb {
 
OnlineDummyReader::OnlineDummyReader(const std::string& name, 
				     ISvcLocator* pSvcLocator)  
  : Algorithm(name, pSvcLocator) { 
  
}

StatusCode 
OnlineDummyReader::initialize() {
    m_log = new MsgStream(msgSvc(), name());
    *m_log << "Initialize done" << endmsg;
    StatusCode sc = service("HistogramDataSvc", m_histosvc, true );

    myhisto = m_histosvc->book("eventsize", "Event size", 10, 0, 100);
    declareInfo("eventsize",myhisto,"Event size");
    myhisto2 = m_histosvc->book("banktype", "Bank type", 49, 0, 48);
    declareInfo("banktype",myhisto2,"Bank type");
    return StatusCode::SUCCESS;
}

StatusCode
OnlineDummyReader::analyze(RawEvent * evt)
{
  double eventsize=0.;
  for (int j = 0; j < RawBank::LastType; ++j)  {
    RawBank::BankType i = RawBank::BankType(j);
    const std::vector<RawBank*>& b = evt->banks(i);
    *m_log << MSG::DEBUG << b.size() << " banks of type: " << rawtypstr[i] << endmsg;
    if (b.size()>0) myhisto2->fill(j,b.size());
    int banklen=0;
    for (unsigned int k = 0; k < b.size(); ++k) {
      const RawBank* r = b[k];
      //int type = r->type();
      //int src  = r->sourceID();
      int len  = r->size();
      banklen=banklen+len;
  /*    const unsigned char *data = (const unsigned char *) r->data();
      *m_log << MSG::DEBUG;;
      int l; 
      for (l = 0; l < len; ++l) {
	*m_log << std::hex << data[l] << " ";
	if (!(l + 1 % 16)) *m_log << endmsg << MSG::DEBUG;
      }
      if (!(l % 16)) *m_log << endmsg; */
    }
    eventsize=eventsize+banklen;

  }
  *m_log << MSG::DEBUG << " eventsize: " << eventsize << endmsg;
  eventsize=eventsize/1024;
  myhisto->fill(eventsize);
    return StatusCode::SUCCESS;
}  
//--------------------------------------------------------------------
// Execute
//--------------------------------------------------------------------
StatusCode 
OnlineDummyReader::execute() {
  SmartDataPtr<RawEvent> raw(eventSvc(), "/Event/DAQ/RawEvent");

  if (raw == 0)  {
    *m_log << MSG::ERROR << "Unable to retrieve Event object" << endreq;
    return StatusCode::FAILURE;
  }
  return analyze(raw);
}

StatusCode
OnlineDummyReader::finalize() 
{
    return StatusCode::SUCCESS;
}

} // namespace LHCb
#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb, OnlineDummyReader)
