// $Id:
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/05/23 11:09:14  ibelyaev
//  see $CALOALGSROOT/doc/release.notes
//
// Revision 1.3  2002/04/04 20:11:41  ibelyaev
//  update for New Calorimeter Event Model (v10r0)
//
// Revision 1.2  2001/11/25 15:31:53  ibelyaev
//  update for newer CaloKernel package
//
// ============================================================================
#ifndef CALOCA_CELAUTOTAGGEDCEL_H
#define CALOCA_CELAUTOTAGGEDCEL_H 1 
// Include files
#include <vector>
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// CaloKernel 
#include "Kernel/CaloCellID.h"

/** @class CelAutoTaggedCell CelAutoTaggedCell.h CaloCA/CelAutoTaggedCell.h
 *
 *  Object Tagged Cell 
 *
 *  @author  Nicole Brun 
 *  @date    27/02/2001
 */

class CelAutoTaggedCell {

public:

  enum Tag {
    DefaultFlag,
    Clustered  ,
    Edge        };

  enum FlagState {
    NotTagged,
    Tagged    };

  // Constructor

  CelAutoTaggedCell( const CaloCellID& id ):
     m_identCel ( id ),
     m_tag ( DefaultFlag ),
     m_status ( NotTagged ) {}

  // Destructor

  virtual ~CelAutoTaggedCell() {}
  
  // Retrieve data

  const CaloCellID& cellID() const {
    return m_identCel;
  }
  bool isEdge() const { 
    return ( ( Tagged == m_status ) && ( Edge == m_tag ) );
  }
  bool isClustered() const {
    return ( ( Tagged == m_status ) && ( Clustered == m_tag ) );
  }
  const CaloCellID& seedForClustered() const {
    return m_seeds[0];
  }
  const std::vector<CaloCellID>& seeds() const {
    return m_seeds;
  }  
  size_t numberSeeds() const {
    return m_seeds.size();
  }
  bool isSeed() const {
    return( ( m_seeds.size() != 1 ) ? false :
              m_identCel ==  m_seeds [0] );
  }
  bool isWithSeed ( const CaloCellID& seed ) {
    std::vector<CaloCellID>::iterator itSeed = m_seeds.begin();
    while ( itSeed != m_seeds.end() &&
            (*itSeed) != seed ) itSeed++;
    return( itSeed != m_seeds.end() );
  }

  // Updata data

  void setId( const CaloCellID& ident ) {
    m_identCel = ident;
  }
  void setIsSeed() {
    m_tag = Clustered;
    m_status = Tagged;
    m_seeds.push_back ( m_identCel);
  }
  void setEdge() {
    m_tag = Edge;
  } 
  void setClustered() {
    m_tag  = Clustered;
  }
  void setStatus() {
    if ( ( Edge == m_tag ) ||
         ( Clustered == m_tag ) ) {
      m_status = Tagged;
    }
  }
  void addSeed( const CaloCellID& seed ) {
    m_seeds.push_back ( seed );
  }  

private:

  CaloCellID m_identCel;
  Tag m_tag;
  FlagState m_status;

  // Ident.seed(s)

  std::vector<CaloCellID> m_seeds;

};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOCA_CELAUTOTAGGEDCELL_H
// ============================================================================










