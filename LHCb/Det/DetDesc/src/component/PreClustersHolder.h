// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/PreClustersHolder.h,v 1.1 2001-02-03 19:49:18 ibelyaev Exp $
#ifndef CALORECONS_PRECLUSTERSHOLDER_H
#define CALORECONS_PRECLUSTERSHOLDER_H

// Include files 
#include <vector>
#include <iostream>
#include <string>
#include "TaggedCellsHolder.h"
#include "Clustering/CelAutoPreCluster.h"

// Forward declarations

//-----------------------------------------------------------------
//
// ClassName  : PreClustersHolder
//
// Description: Find and contain preclusters
//
// Author     : Nicole Brun
// Date       : 12 october 2000
// Changes    :
//
//------------------------------------------------------------------

class PreClustersHolder {

public:

  // Constructor
 
  PreClustersHolder( TaggedCellsHolder& taggedCellsBox );
 
  // Destructor

  virtual ~PreClustersHolder() {}

  // Another members

  TaggedCellsHolder& taggedCellsBox() {
    return *m_taggedCellsBox;
  }
  const std::vector<CelAutoPreCluster*>& preClustersSeq() { return m_preClustersSeq; }
  void printCelAutoPreClusters();
  void printCelAuto1PreCluster( CelAutoPreCluster** preCluster );

private:

  TaggedCellsHolder* m_taggedCellsBox;
  std::vector<CelAutoPreCluster> m_preClusters;
  std::vector<CelAutoPreCluster*> m_preClustersSeq;
  void createSeqAccess();
  void setOwnedCells( std::vector<CelAutoTaggedCell*>::iterator itTagFirstInt,
                      std::vector<CelAutoTaggedCell*>::iterator itTagLastInt,
                      CelAutoPreCluster& preCluster );
  void setSharedCells( std::vector<CelAutoTaggedCell*>::iterator itTagFirstInt,
                       std::vector<CelAutoTaggedCell*>::iterator itTagLastInt,
                       CelAutoPreCluster& preCluster ); 
};

#endif // CALORECONS_PRECLUSTERSHOLDER_H























