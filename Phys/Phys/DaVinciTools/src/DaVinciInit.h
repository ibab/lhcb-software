// $Id$
#ifndef DAVINCIINIT_H 
#define DAVINCIINIT_H 1

// from LHCbKernel
#include "Kernel/LbAppInit.h"

class IGenericTool;

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

  IGenericTool* m_memoryTool;     ///< Pointer to (private) memory histogram tool
  bool m_print;                   ///< Print event and run

};

#endif // DAVINCIINIT_H
