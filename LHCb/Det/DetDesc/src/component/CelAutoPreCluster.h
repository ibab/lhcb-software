// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/CelAutoPreCluster.h,v 1.1 2001-02-03 19:52:28 ibelyaev Exp $
#ifndef CALORECONS_CELAUTOPRECLUSTER_H
#define CALORECONS_CELAUTOPRECLUSTER_H

// Include files
#include <vector>
#include "CaloEvent/CaloCellID.h"

// Forward declarations

//------------------------------------------------------------------
//
// ClassName  : CelAutoPreCluster
//
// Description: Object CelAutoPreCluster
//
// Author     : Nicole Brun
// Date       : 12 october 2000
// Changes    :
//
//------------------------------------------------------------------

class CelAutoPreCluster {

public:

  // Constructor

  CelAutoPreCluster( const CaloCellID& id):
    m_seed ( id ),
    m_numberOwnedCells ( 0 ),
    m_numberSharedCells ( 0 ) {
      m_ownedCells.clear();
      m_ownedCells.push_back( id ); 
      m_sharedCells.clear();
    };  

  // Destructor

  virtual ~CelAutoPreCluster() {};
  
  // Get data

  size_t numberOwnedCells() {
    return m_ownedCells.size();
  }
  std::vector<CaloCellID>& ownedCells() {
    return m_ownedCells;
  }
  size_t numberSharedCells() { 
    return m_sharedCells.size();
  }  
  std::vector<CaloCellID>& sharedCells() {
    return m_sharedCells;
  }
  CaloCellID& seed() { 
    return m_seed;
  }

  // Update data

  void addOwnedMember( CaloCellID ownedCell ) {
     m_ownedCells.push_back( ownedCell );
  }
  void addSharedMember( CaloCellID sharedCell ) {
    m_sharedCells.push_back( sharedCell );
  }
  void setNumberOwnedCells() {
    m_numberOwnedCells = m_ownedCells.size();
  }
  void setNumberSharedCells() {
    m_numberSharedCells = m_sharedCells.size();
  }

private:

  CaloCellID m_seed;
  size_t m_numberOwnedCells; 
  std::vector<CaloCellID> m_ownedCells;
  size_t m_numberSharedCells;
  std::vector<CaloCellID> m_sharedCells;

};
 
#endif // CALORECONS_CELAUTOPRECLUSTER_H




