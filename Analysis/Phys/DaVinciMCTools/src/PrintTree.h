// $Id: PrintTree.h,v 1.4 2008-02-15 14:57:07 pkoppenb Exp $
#ifndef PRINTTREE_H 
#define PRINTTREE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "Kernel/Particle2MCLinker.h"

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
  bool m_link ; ///< print truth
  Particle2MCLinker* m_linker ;
  
};
#endif // PRINTTREE_H
