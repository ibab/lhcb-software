// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/CellularAutomaton.h,v 1.1 2001-02-03 19:49:17 ibelyaev Exp $
#ifndef CALORECONS_CELLULARAUTOMATON_H
#define CALORECONS_CELLULARAUTOMATON_H

// Include files
#include <string>
#include "Gaudi/Algorithm/Algorithm.h"
#include "Gaudi/Interfaces/ISvcLocator.h"

//------------------------------------------------------------ 
//
// ClassName  : CellularAutomaton 
//  
// Description: Algorithm of clustering 
//
// Author     : Nicole Brun
// Date       : 12 october 2000
// Changes    :   
// 
//------------------------------------------------------------


class CellularAutomaton : public Algorithm {
  
public:
  
  // Constructor

  CellularAutomaton(const std::string& name, ISvcLocator* svcloc);

  // Destructor

  virtual ~CellularAutomaton(){};

  // Override default behaviour

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
 
  std::string m_pathHits;
  std::string m_pathGeom;
  std::string m_pathClusters;
  std::string m_calorimeter;
  std::string m_steps;
  
};

#endif  // CALORECONS_CELLULARAUTOMATON_H



