#include "TriggerCard.h"

//--------------------------------------------------------------
// ** Clean up for a new event. Resets all arrays...
//--------------------------------------------------------------

void TriggerCard::reset( ) {
  for ( int i = 0; nColCaloCard >= i; ++i ){
    for ( int j = 0; nRowCaloCard >= j; ++j ) {
      et[i][j] = 0;
    }
  }
  for ( int i = 0; nColCaloCard > i; ++i ){
    for ( int j = 0; nRowCaloCard > j; ++j ){
      prs[i][j] = 0;
      spd[i][j] = 0;
    }
  }
  m_etMax  = 0;
  m_etTot  = 0;
  m_colMax = -1;
  m_rowMax = -1;
  m_empty  = true;
};

//--------------------------------------------------------------
// Add the Et of a cell. Perform the sum and the computation 
// of the local maximum on the flight
// Special : Neighbors have row or col equal to nXxxCaloCard
//--------------------------------------------------------------

void TriggerCard::addEt( int col, int row, int digit ) {
  m_empty  = false;
  if ( (nColCaloCard > col) && (nRowCaloCard > row) ) {
    m_etTot += digit;
    if ( 255 < m_etTot ) {  m_etTot = 255 ; }
  }
  et[col][row] = digit;
  
// ** Update the maximum...

  m_etMax = 0;
  for ( int i = 0; nColCaloCard > i; ++i ){
    for ( int j = 0; nRowCaloCard > j; ++j ) {
      int cellEt = et[i][j] + et[i+1][j] + et[i][j+1] + et[i+1][j+1];
      if ( 255 < cellEt ) { cellEt= 255; };
      if ( cellEt > m_etMax ) {
	m_etMax  = cellEt;
	m_colMax = i;
	m_rowMax = j;
      }
    }
  }
};

//--------------------------------------------------------------
// Check if the ECAL candidate (current card) matches the HCAL
// candidate with the specified row and column
//--------------------------------------------------------------

bool TriggerCard::match_included ( int hCol, int hRow ) { 
  if ( m_empty ) { return false; }
  
// ** Bottom left projection of the HCAL cluster

  int fCol =     hCol * m_mag + m_offsetCol ;    
  int fRow =     hRow * m_mag + m_offsetRow ;

// ** Top right projection of the HCAL cluster

  int lCol = (hCol+2) * m_mag + m_offsetCol -2 ; 
  int lRow = (hRow+2) * m_mag + m_offsetRow -2 ;
  
  if ( (m_colMax < fCol) || (m_colMax > lCol) ) return false ;
  if ( (m_rowMax < fRow) || (m_rowMax > lRow) ) return false ;

  return true;
};
