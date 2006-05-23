//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MEPMaker/src/L1MEP.h,v 1.1 2006-05-23 17:24:17 niko Exp $	//
#ifndef GAUDIEXAMPLES_READALG_H
#define GAUDIEXAMPLES_READALG_H
#include "Event/RawBuffer.h"
#include "Event/L1Buffer.h"
#include "Event/RawEvent.h"
#include "Event/L1Event.h"
#include "Event/L1Bank.h"
#include <stdio.h>
// Framework include files
#include "GaudiKernel/Algorithm.h"  // Required for inheritance
#include "AIDA/IHistogram1D.h"
/** @class L1MEPMaker // L1MEP.h

    MEPMaker class for the RootIOExample

    @author Niko Neufeld
*/

#define MAXRAWTYPE  16
#define MAXRAWSRC  127

class L1MEP : public Algorithm {
public:
	/// Constructor: A constructor of this form must be provided.
   L1MEP(const std::string& name, ISvcLocator* pSvcLocator);
  /// Standard Destructor
  virtual ~L1MEP() { }
  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize();
  /// Event callback
  virtual StatusCode execute();
  virtual StatusCode writel1(l1_int *, int);
  int l1tell1id(int, int);
  virtual void decodel1(l1_int *, int);
  std::string m_l1pfx;
  std::string m_l1hpfx;
  std::string m_l1bufout;
  FILE *m_l1fd[300];
  long long m_fgwritten[300];
  IHistogram1D *m_l1hh[300];
  MsgStream *m_log;
  int m_rmout;
  int m_l1ntell1;
  int m_nomep;
  u_int32_t m_eventid;
};

#endif // GAUDIEXAMPLES_READALG_H
