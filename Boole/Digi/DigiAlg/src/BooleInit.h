// $Id: BooleInit.h,v 1.11 2007-02-05 16:06:08 cattanem Exp $
#ifndef BOOLEINIT_H 
#define BOOLEINIT_H 1

// Include files
// from LHCbKernel
#include "Kernel/LbAppInit.h"

class IGenericTool;
class IEventTimeDecoder;

/** @class BooleInit BooleInit.h
 *  First TopAlg for Boole. Initializes random number and fills memory histogram
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-15
 */
class BooleInit : public LbAppInit {
public: 
  /// Standard constructor
  BooleInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BooleInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  IGenericTool*    m_memoryTool; ///< Pointer to (private) memory histogram tool
  IEventTimeDecoder* m_odinTool; ///< Pointer to odin creation tool
};
#endif // BOOLEINIT_H
