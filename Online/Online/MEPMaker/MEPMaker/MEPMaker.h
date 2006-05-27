//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MEPMaker/MEPMaker/MEPMaker.h,v 1.1 2006-05-27 17:48:19 niko Exp $	//
#ifndef MEPMAKER_H
#define MEPMAKER_H
#include "Event/RawEvent.h"
// Framework include files
#include "GaudiKernel/Algorithm.h"  // Required for inheritance
#include "AIDA/IHistogram1D.h"
/** @class MEPMaker MEPMaker.h

    MEPMaker class to write MEP files from Raw buffers

    @author Niko Neufeld
*/
#include <sys/types.h>
#define MAXRAWTYPE  16
#define MAXRAWSRC  127
typedef u_int32_t raw_int;
#define RAW_MAX_SRC 320

class MEPMaker : public Algorithm {
public:
	/// Constructor: A constructor of this form must be provided.
    static const int MAXSRC = RAW_MAX_SRC;

   MEPMaker(const std::string& name, ISvcLocator* pSvcLocator);
  /// Standard Destructor
  virtual ~MEPMaker() { }
  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize();
  /// Event callback
  virtual StatusCode execute();
  virtual StatusCode writeraw(const LHCb::RawEvent *);
  int rawtell1id(int, int);
  int ignoreTELL1(int);
  int l1tell1id(int, int);
  std::string m_rawpfx;
  std::string m_rawhpfx;
  FILE *m_rawfd[RAW_MAX_SRC];
  unsigned long m_missbank[RAW_MAX_SRC];
  unsigned long long m_fgwritten[RAW_MAX_SRC];
  int m_rawntell1;
  int m_rmout;
  int m_nomep;
  std::string m_rawbufout;
  IHistogram1D *m_rawhh[RAW_MAX_SRC];
  MsgStream *m_log;
};

#endif // GAUDIEXAMPLES_READALG_H
