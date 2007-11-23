//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineDummyReader/OnlineDummyReader/OnlineDummyReader.h,v 1.2 2007-11-23 17:53:22 evh Exp $    //
#ifndef ONLINEDUMMYREADER_H
#define ONLINEDUMMYREADER_H

// Framework include files
#include "GaudiKernel/Algorithm.h"  // Required for inheritance
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h" 

// LHCb includes

#include "Event/RawEvent.h"
/** @class MEPMaker OnlineDummyReader.h

    MEPMaker class to write MEP files from Raw buffers

    @author Niko Neufeld
*/
namespace LHCb {
class OnlineDummyReader : public Algorithm {
private:
  MsgStream *m_log;
  IHistogramSvc* m_histosvc;
  IHistogram1D*   myhisto;
  IHistogram1D*   myhisto2;
public:
  OnlineDummyReader(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~OnlineDummyReader() { };
  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize();
  /// Event callback
  virtual StatusCode execute();
  /// Worker routine  
  StatusCode analyze(LHCb::RawEvent *);
};
}
#endif // ONLINEDUMMYREADER_H
