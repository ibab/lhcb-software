// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/DetecHolder.h,v 1.1 2001-02-03 19:49:17 ibelyaev Exp $
#ifndef CALORECONS_DETECHOLDER_H
#define CALORECONS_DETECHOLDER_H

// Include files
#include <vector>
#include <iostream>
#include <string>
#include "Calo/DeCalorimeter.h"
#include "CaloEvent/CaloDigit.h"
#include "CaloEvent/CaloCellID.h"
#include "Gaudi/Interfaces/IDataProviderSvc.h"

// Forward declarations

//------------------------------------------------------------
//
// ClassName  : DetecHolder
//
// Description: For one calorimeter, to get geometry
//
// Author     : Nicole Brun
// Date       : 12 october 2000
// Changes    :
//
//------------------------------------------------------------

class DetecHolder {

public:

  // Constructor

  DetecHolder( IDataProviderSvc* dataService,
               const std::string& inputPath,
               const std::string&  calorimeter );
 
  // Destructor

  virtual ~DetecHolder(){};

  // Anothers members

  const DeCalorimeter* detector() {
    return m_detector;
  }
  const std::string& calorimeter() {
    return m_calorimeter;
  }
  void printCaloCellIDNeighbors1Cell( const CaloCellID& ID,
                                      bool printWithCell );

private:

  DeCalorimeter* m_detector;
  std::string m_calorimeter;

};

#endif // CALORECONS_DETECHOLDER_H











