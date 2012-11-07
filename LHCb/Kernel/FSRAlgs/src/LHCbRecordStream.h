// $Id: LHCbRecordStream.h,v 1.1 2008/12/10 18:37:36 marcocle Exp $
#ifndef LHCB_RECORDSTREAM_H
#define LHCB_RECORDSTREAM_H

// Required for inheritance
#include "LHCbOutputStream.h"

/** @class LHCbRecordStream LHCbRecordStream.h
  * Extension of OutputStream to write run records after last event
  *
  * @author  M.Frank
  * @version 1.0
  */
class LHCbRecordStream : public LHCbOutputStream     {
public:
  /// Standard algorithm Constructor
  LHCbRecordStream(const std::string&, ISvcLocator*);
  /// Standard Destructor
  virtual ~LHCbRecordStream() {}
  /// Algorithm overload: finalization
  virtual StatusCode finalize();
  /// Runrecords do not get written for each event: Event processing hence dummy....
  virtual StatusCode execute() {  return StatusCode::SUCCESS; }
};

#endif // LHCB_RECORDSTREAM_H
