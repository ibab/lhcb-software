// $Id: StoreExplorer.h,v 1.1 2004-06-17 15:29:33 cattanem Exp $
#ifndef STOREEXPLORER_H 
#define STOREEXPLORER_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class StoreExplorer StoreExplorer.h
 *  Navigates the transient event store and prints out the directory tree
 *
 *  @author Markus Frank
 */
class StoreExplorer : public GaudiAlgorithm {
public:
  /// Standard constructor
  StoreExplorer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~StoreExplorer() {}; ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution
  void printObj(IRegistry* pReg, std::vector<bool>& flg );
  void explore(IRegistry* pObj, std::vector<bool>& flg );
 
protected:

private:
  long    m_print;             ///< Number of events to print
  double  m_freq;              ///< Print frequency
  bool    m_exploreRelations;  ///< If true, explore also Relations objects
  long    m_total, m_frqPrint; ///< Internal event counters
};
#endif // STOREEXPLORER_H
