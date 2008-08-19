// $Id: OMAMsgStdOut.h,v 1.1 2008-08-19 22:45:32 ggiacomo Exp $
#ifndef OMALIB_OMAMSGSTDOUT_H 
#define OMALIB_OMAMSGSTDOUT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "OMAlib/IOMAMsgTool.h"            // Interface


/** @class OMAMsgStdOut OMAMsgStdOut.h OMAlib/OMAMsgStdOut.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2008-08-14
 */
class OMAMsgStdOut : public GaudiTool, virtual public IOMAMsgTool {
public: 
  /// Standard constructor
  OMAMsgStdOut( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~OMAMsgStdOut( ); ///< Destructor
  virtual bool raiseAlarm(OMAMessage& message);
  virtual bool lowerAlarm(OMAMessage& message);
  

};
#endif // OMALIB_OMAMSGSTDOUT_H
