// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/SeedsHolder.h,v 1.1 2001-02-03 19:49:18 ibelyaev Exp $
#ifndef CALORECONS_SEEDSHOLDER_H
#define CALORECONS_SEEDSHOLDER_H

// Include files 
#include "HitsHolder.h"
#include "DetecHolder.h"
#include "CaloEvent/CaloDigit.h"
#include "CaloEvent/CaloCellID.h"
#include "Gaudi/Interfaces/IDataProviderSvc.h"
#include "Gaudi/Interfaces/IMessageSvc.h"

// Forward declarations

//------------------------------------------------------------
// 
// ClassName  : SeedsHolder 
//
// Description: For one calorimeter,
//              from hits and geometry associeted,
//              find and contain the seeds
//
// Author     : Nicole Brun
// Date       : 12 october 2000
// Changes    : 
//
//------------------------------------------------------------

class SeedsHolder {

public:

  // Constructor

  SeedsHolder( HitsHolder& hitsBox,
               DetecHolder& geomBox );

  // Destructor

  virtual ~SeedsHolder(){}

  // Another members

  HitsHolder& hitsBox() {
    return *m_hitsBox;
  }  
  DetecHolder& geomBox() {
    return *m_geomBox;
  }
  const std::vector<CaloCellID>& seeds() const { return m_seeds; }
  void printCaloCellIDSeeds(); 
  void printCaloDigitSeeds();

private:

  HitsHolder* m_hitsBox;
  DetecHolder* m_geomBox;
  std::vector<CaloCellID> m_seeds;

};

#endif // CALORECONS_SEEDSHOLDER_H






