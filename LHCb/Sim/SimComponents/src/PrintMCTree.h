// $Id: PrintMCTree.h,v 1.1 2007-10-10 12:11:35 jpalac Exp $
#ifndef PRINTMCTREE_H 
#define PRINTMCTREE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
class IDebugTool;


/** @class PrintMCTree PrintMCTree.h
 *  
 *  Prints the decay tree of all MC particles with a given PID 
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-09-10
 */
class PrintMCTree : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PrintMCTree( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrintMCTree( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IPrintMCDecayTreeTool *m_printMCTree;
  std::vector<std::string> m_particleNames ; ///< particle names
  int m_depth ;  ///< depth of tree
  std::vector<int> m_particleIDs ;

};
#endif // PRINTMCTREE_H
