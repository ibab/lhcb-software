// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/HitsHolder.h,v 1.1 2001-02-03 19:49:18 ibelyaev Exp $
#ifndef CALORECONS_HITSHOLDER_H
#define CALORECONS_HITSHOLDER_H

// Include files

#include <vector>
#include <string>
#include <iostream> 
#include "DetecHolder.h"
#include "CaloEvent/CaloDigit.h"
#include "CaloEvent/CaloCellID.h"
#include "Gaudi/Interfaces/IDataProviderSvc.h"
#include "Gaudi/Interfaces/IMessageSvc.h"

// Forward declarations

//------------------------------------------------------------
// 
// ClassName  : HitsHolder 
//
// Description: Find and contain the hits 
//
// Author     : Nicole Brun
// Date       : 12 october 2000
// Changes    : 
//
//------------------------------------------------------------------

class HitsHolder {

public:

  // Constructor

  HitsHolder( IDataProviderSvc* dataService,
              const std::string& address,
              const std::string& calorimeter,
              IMessageSvc* messageService = 0 );
    
  // Destructor

  virtual ~HitsHolder(){};

  // Anothers members

  const std::vector<CaloDigit*>& hitsSeq() {
    return m_hits; }
  const std::vector<CaloDigit*>& hitsDirect();
  bool isLocMax( CaloDigit** hit,
                 DetecHolder& geomBox ); 
  const std::string& calorimeter() const {
    return m_calorimeter; }  
  void printCaloDigitHits();
  void printCaloDigit1Hit( CaloDigit** hit );
  void printCaloCellIDNeighborsHits( DetecHolder& geomBox );
  void printCaloDigitNeighborsHits( DetecHolder& geomBox );
  void printCaloDigitNeighbors1Hit( CaloDigit** hit,
                                    DetecHolder& geomBox,
                                    bool printWithCell );
 
private:
 
  std::vector<CaloDigit*> m_hits;
  std::vector<CaloDigit*> m_hitsDirect;
  std::string m_calorimeter;  

};

#endif // CALORECONS_HITSHOLDER_H











