//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MEPMaker/src/MEPMaker.h,v 1.1 2006-05-23 17:24:17 niko Exp $	//
#ifndef GAUDIEXAMPLES_READALG_H
#define GAUDIEXAMPLES_READALG_H
#include "Event/RawBuffer.h"
#include "Event/L1Buffer.h"
#include "Event/RawEvent.h"
#include "Event/L1Event.h"
#include "Event/L1Bank.h"

// Framework include files
#include "GaudiKernel/Algorithm.h"  // Required for inheritance
#include "AIDA/IHistogram1D.h"
/** @class MEPMaker MEPMaker.h

    MEPMaker class for the RootIOExample

    @author Markus Frank
*/

#define MAXRAWTYPE  16
#define MAXRAWSRC  127

class MEPMaker : public Algorithm {
public:
	/// Constructor: A constructor of this form must be provided.
   MEPMaker(const std::string& name, ISvcLocator* pSvcLocator);
  /// Standard Destructor
  virtual ~MEPMaker() { }
  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize() {
    return StatusCode::SUCCESS;
  }
  /// Event callback
  virtual StatusCode execute();
  virtual StatusCode writel1(l1_int *, int);
  virtual StatusCode writeraw(raw_int *, int);
  int rawtell1id(int, int);
  int l1tell1id(int, int);
  std::string m_l1pfx;
  std::string m_rawpfx;
  std::string m_l1hpfx;
  std::string m_rawhpfx;
  int m_l1fd[300];
  int m_rawfd[300];
  IHistogram1D *m_l1hh[300];
  IHistogram1D *m_rawhh[300];
  MsgStream *m_log;
};

#endif // GAUDIEXAMPLES_READALG_H
