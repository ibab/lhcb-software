//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MEPMaker/src/RawMEP.h,v 1.1 2006-05-23 17:24:18 niko Exp $	//
#ifndef RAWMEP_H
#define RAWMEP_H
#include "Event/RawBuffer.h"
#include "Event/L1Buffer.h"
#include "Event/RawEvent.h"
#include "Event/L1Event.h"
#include "Event/L1Bank.h"
#include <stdio.h>
// Framework include files
#include "GaudiKernel/Algorithm.h"  // Required for inheritance
#include "AIDA/IHistogram1D.h"
/** @class RawMEP RawMEP.h

    RawMEP class to write MEP files from Raw buffers

    @author Niko Neufeld
*/

#define MAXRAWTYPE  16
#define MAXRAWSRC  127

class RawMEP : public Algorithm {
public:
	/// Constructor: A constructor of this form must be provided.
   RawMEP(const std::string& name, ISvcLocator* pSvcLocator);
  /// Standard Destructor
  virtual ~RawMEP() { }
  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize();
  /// Event callback
  virtual StatusCode execute();
  virtual StatusCode writeraw(raw_int *, int);
  int rawtell1id(int, int);
  int inactivetell1(int);
  int l1tell1id(int, int);
  std::string m_rawpfx;
  std::string m_rawhpfx;
  FILE *m_rawfd[300];
  unsigned long m_missbank[300];
  unsigned long long m_fgwritten[300];
  int m_rawntell1;
  int m_rmout;
  int m_nomep;
  std::string m_rawbufout;
  IHistogram1D *m_rawhh[300];
  MsgStream *m_log;
};

#endif // GAUDIEXAMPLES_READALG_H
