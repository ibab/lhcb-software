// $Id: PrintTree.h,v 1.1 2004-09-14 11:59:07 pkoppenb Exp $
#ifndef PRINTTREE_H 
#define PRINTTREE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "DaVinciTools/DVAlgorithm.h"

class IDebugTool;

/** @class PrintTree PrintTree.h
 *  
 *  Loops over all particles in the given InputLocation and prints them 
 *  as a tree
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-09-10
 */
class PrintTree : public DVAlgorithm {
public: 
  /// Standard constructor
  PrintTree( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrintTree( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IDebugTool *m_debug;

};
#endif // PRINTTREE_H
