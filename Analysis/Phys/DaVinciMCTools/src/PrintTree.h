// $Id: PrintTree.h,v 1.2 2005-01-11 12:36:08 pkoppenb Exp $
#ifndef PRINTTREE_H 
#define PRINTTREE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

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

protected:

private:
  IDebugTool *m_debug;

};
#endif // PRINTTREE_H
