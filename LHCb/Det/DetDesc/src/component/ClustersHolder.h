#ifndef   CALORECO_CLUSTERSHOLDER_H
#define   CALORECO_CLUSTERSHOLDER_H 1


// Include files 
#include <vector>
#include <iostream>
#include <string>
#include "GaudiKernel/IDataProviderSvc.h"

#include "CaloEvent/CelAutoCluster.h"

#include "PreClustersHolder.h"
#include "TaggedCellsHolder.h"
#include "SeedsHolder.h"
#include "HitsHolder.h"
#include "DetecHolder.h"



// Forward declarations

//-----------------------------------------------------------------
//
// ClassName  : ClustersHolder
//
// Description: For one calorimeter, find and contain clusters
//
// Author     : Nicole Brun
// Date       : 12 october 2000
// Changes:
//
//------------------------------------------------------------------

class ClustersHolder {

public:

  // Constructors

  ClustersHolder( PreClustersHolder& preClustersBox,
                  IDataProviderSvc* dataService,
                  const std::string& outputPath ); 
  ClustersHolder( TaggedCellsHolder& taggedCellsBox,
                  IDataProviderSvc* dataService,
                  const std::string& outputPath );

  // Destructor

  virtual ~ClustersHolder() {}

  // Another members

  void printCelAutoClusters();
  void printCelAuto1Cluster( CelAutoCluster** cluster ); 
 
private:

  CelAutoClusterVector* m_clustersSeq;
  TaggedCellsHolder* m_taggedCellsBox;
  CelAutoClusterVector* createOutput( IDataProviderSvc* dataService,
                                   const std::string& outputPath );
  void setOwnedHits( std::vector<CaloCellID>& cells,
                     HitsHolder& hitsBox, 
                     CelAutoCluster& cluster );
  void setOwnedHits( std::vector<CelAutoTaggedCell*>::iterator itTagFirst,
                     std::vector<CelAutoTaggedCell*>::iterator itTagLast,
                     HitsHolder& hitsBox,
                     CelAutoCluster& cluster );
  void setSharedHits( std::vector<CaloCellID>& cells,
                      HitsHolder& hitsBox, 
                      CelAutoCluster& cluster );
  void setSharedHits( std::vector<CelAutoTaggedCell*>::iterator itTagFirst,
                      std::vector<CelAutoTaggedCell*>::iterator itTagLast,
                      HitsHolder& hitsBox,
                      CelAutoCluster& cluster );
  void setEnergyBarycenter( CelAutoCluster& cluster );

}; 



#endif // CALORECO_CLUSTERSHOLDER_H





