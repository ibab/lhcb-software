// $Id: VeloEffChecker.h,v 1.2 2008-03-28 14:33:52 cattanem Exp $
#ifndef _VeloEffChecker_H_
#define _VeloEffChecker_H_

/** @class VeloEffChecker VeloEffChecker.h
 *
 *  Algorithm to check monitor sensor efficiency in MC
 *
 *  @author M.Needham
 *  @date   25/03/2008
 */

#include "GaudiAlg/GaudiHistoAlg.h"

#include <string>
#include <vector>

// linkers
#include "Linker/LinkerTool.h"


namespace LHCb{
  class MCHit;
  class VeloCluster;
}

class DeVelo;

class VeloEffChecker: public GaudiHistoAlg {

public:


  // Constructors and destructor
  VeloEffChecker(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~VeloEffChecker();  

  virtual StatusCode initialize();
  
  virtual StatusCode execute();

  virtual StatusCode finalize();  

  
 
private:

  typedef LinkerTool<LHCb::VeloCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::InverseType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;


  void fillEffHistos(const LHCb::MCHit* aHit);
  bool inActive(const LHCb::MCHit* aHit) const;


  Table* m_table;
  DeVelo* m_tracker;


  int m_effOffset;
  int m_inEffOffset;

  double m_pMin;
  double m_tMax;

};

#endif // _VeloEffChecker_H_
