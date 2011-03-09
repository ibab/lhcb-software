// $Id: RecInit.h,v 1.2 2007-12-11 12:44:29 cattanem Exp $
#ifndef RECINIT_H 
#define RECINIT_H 1

// Include files
// from LHCbKernel
#include "Kernel/LbAppInit.h"

class IGenericTool;
class IIncidentSvc;

/** @class RecInit RecInit.h
 *  Algorithm to initialize the reconstruction sequence
 *  Creates RecHeader and ProcStatus
 *
 *  @author Marco Cattaneo
 *  @date   2006-03-14
 */
class RecInit : public LbAppInit {
public: 
  /// Standard constructor
  RecInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RecInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  IGenericTool* m_memoryTool;   ///< Pointer to (private) memory histogram tool
  IIncidentSvc* m_incidentSvc;  ///< Pointer to the incident service.
  std::string m_rawEventLocation;               ///< Location where we get the RawEvent
  bool m_abortOnFID;               ///< If I can't find the raw file ID, do I abort?
};
#endif // RECINIT_H
