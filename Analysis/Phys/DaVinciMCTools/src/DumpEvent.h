// $Id: DumpEvent.h,v 1.1 2004-09-14 11:59:07 pkoppenb Exp $
#ifndef DUMPEVENT_H 
#define DUMPEVENT_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


// forward declaration
class IDebugTool;
//class IAssociator;

/** @class DumpEvent DumpEvent.h
 *  
 *  This algorithm calls all the methods of the DebugTool to test it.
 *  It's also a simple exemple about how to use the DebugTool.
 *
 *  @author Olivier Dormond
 *  @date   11/04/2002
 */
class DumpEvent : public GaudiAlgorithm {
public:
  /// Standard constructor
  DumpEvent( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DumpEvent( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IDebugTool *m_debug;

  std::string m_debug_name;
};
#endif // DUMPEVENT_H
