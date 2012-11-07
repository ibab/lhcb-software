// $Id: LHCbFSRStream.h,v 1.1 2008/12/10 18:37:36 marcocle Exp $
#ifndef LHCB_LHCBFSRSTREAM_H
#define LHCB_LHCBFSRSTREAM_H

// Required for inheritance
#include "LHCbRecordStream.h"
#include "IIOFSRSvc.h"
#include "Event/IOFSR.h"

/** @class LHCbFSRStream LHCbFSRStream.h
  * Extension of RecordStream to add IOFsr and automatically clean up the TES
  * 
  *
  * @author  R.Lambert
  * @version 1.0
  */
class LHCbFSRStream : public LHCbRecordStream     {
public:
  /// Standard algorithm Constructor
  LHCbFSRStream(const std::string&, ISvcLocator*);
  /// Standard Destructor
  virtual ~LHCbFSRStream() {}
  /// Algorithm overload: initialization
  virtual StatusCode initialize();
  /// Algorithm overload: finalization
  virtual StatusCode finalize();
  /// Runrecords do not get written for each event: Event processing hence dummy....
  virtual StatusCode execute() {  return StatusCode::SUCCESS; }

private:
  IIOFSRSvc* m_ioFsrSvc; ///<reference to IOFSR service
  bool m_doIOFsr; //set to false to skip all the IOFSR stuff

};

#endif // LHCB_LHCBFSRSTREAM_H
