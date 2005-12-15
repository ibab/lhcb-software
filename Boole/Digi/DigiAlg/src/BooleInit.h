// $Id: BooleInit.h,v 1.7 2005-12-15 13:56:37 cattanem Exp $
#ifndef BOOLEINIT_H 
#define BOOLEINIT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IGenericTool;
class IRndmTool;

/** @class BooleInit BooleInit.h
 *  First TopAlg for Boole. Initializes random number and fills memory histogram
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-15
 */
class BooleInit : public GaudiAlgorithm {
public: 
  /// Standard constructor
  BooleInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BooleInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IGenericTool* m_memoryTool;   ///< Pointer to (private) memory histogram tool
  IRndmTool*    m_initRndmTool; ///< Pointer to (private) rndm intialization tool
  int           m_eventCounter; ///< Number of events processed 
};
#endif // BOOLEINIT_H
