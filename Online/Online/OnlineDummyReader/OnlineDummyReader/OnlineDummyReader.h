//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineDummyReader/OnlineDummyReader/OnlineDummyReader.h,v 1.1 2006-11-12 08:01:21 niko Exp $    //
#ifndef ONLINEDUMMYREADER_H
#define ONLINEDUMMYREADER_H

// Framework include files
#include "GaudiKernel/Algorithm.h"  // Required for inheritance
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
