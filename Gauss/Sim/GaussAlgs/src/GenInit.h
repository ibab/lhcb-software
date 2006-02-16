// $Id: GenInit.h,v 1.2 2006-02-16 15:23:22 gcorti Exp $
#ifndef GENINIT_H 
#define GENINIT_H 1

// Include files
// from LHCbKernel
#include "Kernel/LbAppInit.h"

class IGenericTool;

/** @class GenInit GenInit.h
 *  
 *  First TopAlg for Generator phase of Gauss.
 *  Initializes random number and fills memory histogram.
 *  It also creates and fill the GenHeader.
 * 
 *  @author Gloria CORTI
 *  @date   2006-01-16
 */
class GenInit : public LbAppInit {
public: 
  /// Standard constructor
  GenInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GenInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Property to set the first event number, if event header is missing
  int           m_firstEvent;

  /// Property to set the run number, if event header is missing
  int           m_runNumber;

  IGenericTool* m_memoryTool;   ///< Pointer to (private) memory histogram tool

};
#endif // GENINIT_H
