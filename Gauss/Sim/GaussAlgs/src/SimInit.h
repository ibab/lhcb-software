// $Id: SimInit.h,v 1.2 2006-02-16 15:23:22 gcorti Exp $
#ifndef SIMINIT_H 
#define SIMINIT_H 1

// Include files
// from LHCbKernel
#include "Kernel/LbAppInit.h"

class IGenericTool;
class IDetDataSvc;

/** @class SimInit SimInit.h
 *  
 *  First TopAlg Simulation phase of Gauss.
 *  Initializes random number and fills memory histogram.
 *  It also creates and fills the MCHeader.
 * 
 *  @author Gloria CORTI
 *  @date   2006-01-16
 */
class SimInit : public LbAppInit {
public: 
  /// Standard constructor
  SimInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SimInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IGenericTool* m_memoryTool;   ///< Pointer to (private) memory histogram tool
  IDetDataSvc*  m_detDataSvc;   ///< Pointer to (private) detector service

};
#endif // SIMINIT_H
