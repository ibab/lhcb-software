// $Id: DumpMCEventAlg.h,v 1.1.1.1 2005-12-15 14:29:30 gcorti Exp $
#ifndef DUMPMCEVENTALG_H 
#define DUMPMCEVENTALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class ICheckTool;

/** @class DumpMCEventAlg DumpMCEventAlg.h
 *  Dump contents of an MCEvent. 
 *  Reimplements functionality previously in DumpEventExample
 *  Amount printed depends on OutputLevel:
 *    INFO: prints EventHeader and Collisions
 *   DEBUG: prints also first "NumberOfObjectsToPrint" MCVertices and
 *          MCParticles (default is 5)
 * VERBOSE: prints all MCVertices and MCParticles
 *
 *  @author Marco Cattaneo
 *  @date   2004-07-14
 */
class DumpMCEventAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  DumpMCEventAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DumpMCEventAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  unsigned int m_numObjects; ///< Number of objects to print
  ICheckTool*  m_checker;    ///< Tool to check event integrity
};

#endif // DUMPMCEVENTALG_H
