#ifndef CELAUTOTAGGEDCEL_H
#define CELAUTOTAGGEDCEL_H

// Include files

#include <vector>
#include "CaloEvent/CaloCellID.h"

// Forward declarations

//------------------------------------------------------------------
//
// ClassName  : CelAutoTaggedCell
//
// Description: Object tagged cell
//
// Author     : Nicole Brun
// Date       : 12 octobre 2000
// Changes    :
//
//------------------------------------------------------------------

class CelAutoTaggedCell {

public:

  // Constructor

  CelAutoTaggedCell( const CaloCellID& id, const double energy ):
     m_identCel ( id ),
     m_energy ( energy ),
     m_edge ( false ),
     m_clustered ( false ) {}

  // Destructor

  virtual ~CelAutoTaggedCell() {}

  // Retrieve data

  const CaloCellID& cellID() const { return m_identCel; }
  double energy() const { return m_energy;}
  bool isEdge() const { return m_edge; }
  bool isClustered() const { return m_clustered; }
  const CaloCellID& seedForClustered() const { return m_seeds[0]; }
  const std::vector<CaloCellID>& seeds() const { return m_seeds; }  
  size_t numberSeeds() const { return m_seeds.size(); }
  bool isSeed() const {
    return( ( m_seeds.size() != 1 ) ? false :
            (CaloCellID::ContentType) m_identCel ==
            (CaloCellID::ContentType) m_seeds [0] ); }
  bool isWithSeed ( const CaloCellID& seed ) {
    std::vector<CaloCellID>::iterator itSeed = m_seeds.begin();
    while ( itSeed != m_seeds.end() &&
            ( CaloCellID::ContentType) (*itSeed) !=
            (CaloCellID::ContentType) seed ) itSeed++;
    return( itSeed != m_seeds.end() );
  }

  // Updata data

  void setId( const CaloCellID& ident ) { m_identCel = ident; }
  void setEnergy( const double energy ) { m_energy = energy; }
  void setEdge( const bool edge  ) { m_edge = edge; } 
  void setClustered( const bool clustered) { m_clustered = clustered; }
  void addSeed( const CaloCellID& seed ) { m_seeds.push_back ( seed ); } 

private:

  CaloCellID m_identCel;
  double m_energy;

  // True if cel. is shared with one or more another cluster

  bool m_edge;

  // True if cel. is a seed or owned by only one cluster

  bool m_clustered;
  
  // Ident.seed(s)

  std::vector<CaloCellID> m_seeds;

};

#endif // CALORECONS_CELAUTOTAGGEDCELL_H;

