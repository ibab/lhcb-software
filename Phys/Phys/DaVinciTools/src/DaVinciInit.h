// $Id: DaVinciInit.h,v 1.1 2009-03-02 21:10:02 pkoppenb Exp $
#ifndef DAVINCIINIT_H 
#define DAVINCIINIT_H 1

// Include files
// from LHCbKernel
#include "Kernel/LbAppInit.h"

class IGenericTool;
class IIncidentSvc;

/** @class DaVinciInit DaVinciInit.h
 *
 *  Algorithm to initialize DaVinci
 *
 *  @author Patrick Koppenburg
 *  @date   2009-03-02
 */
class DaVinciInit : public LbAppInit 
{

public: 

  /// Standard constructor
  DaVinciInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DaVinciInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  IGenericTool* m_memoryTool;   ///< Pointer to (private) memory histogram tool
  IIncidentSvc* m_incidentSvc;  ///< Pointer to the incident service.
  bool m_print ;                ///< Print event and run
  unsigned long m_increment ;   ///< Number of events to measure memory on
  unsigned long m_lastMem ;     ///< Last memory

};

#endif // DAVINCIINIT_H
