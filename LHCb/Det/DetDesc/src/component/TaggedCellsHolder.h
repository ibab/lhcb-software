// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/TaggedCellsHolder.h,v 1.1 2001-02-03 19:49:18 ibelyaev Exp $
#ifndef CALORECONS_TAGGEDCELLSHOLDER_H
#define CALORECONS_TAGGEDCELLSHOLDER_H

// Include files 

#include <vector>
#include <string>
#include <iostream>
#include "DetecHolder.h"
#include "HitsHolder.h"
#include "SeedsHolder.h"
#include "Clustering/CelAutoTaggedCell.h"
#include "CaloEvent/CaloCellID.h"
#include "Gaudi/Kernel/SmartRefVector.h"

// Forward declarations

//------------------------------------------------------------------
//
// ClassName  : TaggedCellsHolder
//
// Description: Find and contain tagged cells
//
// Author     : Nicole Brun
// Date       : 12 october 2000
// Changes    :
//
//------------------------------------------------------------------



class TaggedCellsHolder {

public:

  // Constructors

  TaggedCellsHolder( SeedsHolder& seedsBox,
                     bool infoStepByStep = false ); 
  TaggedCellsHolder( HitsHolder& hitsBox,
                     DetecHolder& geomBox,
                     bool infoStepByStep = false ); 
    
  // Destructor
 
  virtual ~TaggedCellsHolder() {}
  
  // Another members

  HitsHolder& hitsBox() {
    return *m_hitsBox;
  }
  DetecHolder& geomBox() {
    return *m_geomBox;
  }
  const std::vector<CelAutoTaggedCell*>& taggedCellsSeq() {
    return m_taggedCellsSeq;
  }  
  const std::vector<CelAutoTaggedCell*>& taggedCellsDirect();
  const size_t numberNotTaggedCells() {
    return m_numberNotTaggedCells;
  } 
  void printCelAutoTaggedCellCells();
  void printCelAutoTaggedCell1Cell( CelAutoTaggedCell** cell);
  void printCaloCellIDCells( std::vector<CaloCellID>& cells,
                             const std::string& tabHeaderLignWithWhite );  
  void printCaloDigitHits( SmartRefVector<CaloDigit>& hits,
                           const std::string& tabHeaderLignWithWhite );  

private:
  
  std::vector<CelAutoTaggedCell> m_taggedCells;
  std::vector<CelAutoTaggedCell*> m_taggedCellsSeq;
  std::vector<CelAutoTaggedCell*> m_taggedCellsDirect;
  size_t m_numberNotTaggedCells;
  bool m_infoStepByStep; 
  HitsHolder* m_hitsBox;
  DetecHolder* m_geomBox; 
  void createData( HitsHolder& hitsBox );
  void createSeqAccess();
  void createDirectAccess( std::vector<CelAutoTaggedCell*>::iterator itSeqInFirst,
                           std::vector<CelAutoTaggedCell*>::iterator itSeqInLast,
                           std::vector<CelAutoTaggedCell*>& bufDirectOut );
  void setSeedsSeedsFind( SeedsHolder& seedsBox );
  void setSeedsSeedsNotFind( HitsHolder& hitsBox,
                             DetecHolder& geomBox );
  void evoleTagging ( DetecHolder& geomBox );
  void oneEvoleTagging( std::vector<CelAutoTaggedCell*>::iterator itTagFirst,
                        std::vector<CelAutoTaggedCell*>::iterator itTagLast,
                        DetecHolder& geomBox );
  void appliRulesTagger( CelAutoTaggedCell*& taggedCell,
                         std::vector<CelAutoTaggedCell*>& bufNotTaggedDirect,
                         DetecHolder& geomBox);
  void findNeighborsSeeds( std::vector<CaloCellID>& neighborsCells,
                           CelAutoTaggedCell*& taggedCells );
  void findTaggedNeighbors( std::vector<CaloCellID>& neighborsCells,
                            std::vector<CelAutoTaggedCell*>& bufNotTaggedDirect,
                            std::vector<CaloCellID>& taggedNeighnors );
};

#endif // CALORECONS_TAGGEDCELLSHOLDER_H





































