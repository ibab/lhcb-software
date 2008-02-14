// $Id: PrintTree.h,v 1.3 2008-02-14 08:24:55 pkoppenb Exp $
#ifndef PRINTTREE_H 
#define PRINTTREE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

class IPrintMCDecayTreeTool;

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
  IPrintDecayTreeTool* m_debug;

};
#endif // PRINTTREE_H
