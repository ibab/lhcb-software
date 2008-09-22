// $Id: CellularAutomatonAlg.h,v 1.2 2008-09-22 01:41:23 odescham Exp $
#ifndef CELLULARAUTOMATONALG_H 
#define CELLULARAUTOMATONALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "CaloInterfaces/ICaloClusterization.h"


/** @class CellularAutomatonAlg CellularAutomatonAlg.h
 *  
 *
 *  @author Victor Egorychev
 *  @date   2008-04-03
 */
class CellularAutomatonAlg : public GaudiAlgorithm {

public: 
  /// Standard constructor
  CellularAutomatonAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CellularAutomatonAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputData;
  std::string m_outputData;
  std::string m_detData;
  const DeCalorimeter* m_detector;

  bool m_sort     ;
  bool m_sortByET ;
  
  std::string m_toolName;
  ICaloClusterization* m_tool;
  
  unsigned int m_neig_level;
  unsigned long m_passMin , m_passMax;
  double m_pass , m_clus ,m_event;
};
#endif // CELLULARAUTOMATONALG_H
