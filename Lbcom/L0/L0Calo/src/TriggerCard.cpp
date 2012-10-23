#include "TriggerCard.h"

/** Clean up for a new event. Resets all arrays and values
 */

void TriggerCard::reset( ) {
  int i;
  int j;
  for ( i = 0; nColCaloCard >= i; ++i ){
    for ( j = 0; nRowCaloCard >= j; ++j ) {
      et[i][j] = 0;
    }
  }
  for ( i = 0; nColCaloCard > i; ++i ){
    for ( j = 0; nRowCaloCard > j; ++j ){
      prs[i][j] = 0;
      spd[i][j] = 0;
    }
  }
  m_etMax  = 0;
  m_etTot  = 0;
  m_colMax = m_detElem -> cardFirstValidColumn( m_number ) - 
    m_detElem -> cardFirstColumn( m_number ) ;
  m_rowMax = m_detElem -> cardFirstValidRow( m_number ) -
    m_detElem -> cardFirstRow( m_number ) ;
  m_empty  = true;
}

/**
 * Sets the Et of a cell. Perform the total sum and the computation 
 * of the local 2x2 maximum on the flight
 * Special : Neighbors have row or col equal to nRowCaloCard/nColCaloCard
 */

void TriggerCard::addEt( int col, int row, int digit ) {
  m_empty  = false;
  if ( (nColCaloCard > col) && (nRowCaloCard > row) ) {
    m_etTot += digit;
    if ( 255 < m_etTot ) {  m_etTot = 255 ; }
  }
  et[col][row] = digit; 

// Update the maximum...

  m_etMax = 0;
  int i;
  for ( i = 0; nColCaloCard > i; ++i ) {
    int j;
    for ( j = 0; nRowCaloCard > j; ++j ) {
      int cellEt = et[i][j] + et[i+1][j] + et[i][j+1] + et[i+1][j+1];
      if ( 255 < cellEt ) { cellEt= 255; };
      if ( cellEt > m_etMax ) {
        m_etMax  = cellEt;
        m_colMax = i;
        m_rowMax = j;
      }
    }
  }
}

void TriggerCard::setPrs( int col, int row ) {
  if (nRowCaloCard > row) {
    if (nColCaloCard > col) { prs[col]  [row]   += 8; }
    if (           0 < col) { prs[col-1][row]   += 4; }
  }
  if (0 < row) {
    if (nColCaloCard > col) { prs[col]  [row-1] += 2; }
    if (           0 < col) { prs[col-1][row-1] += 1; }
  }
}

void TriggerCard::setSpd( int col, int row ) {
  if (nRowCaloCard > row) {
    if (nColCaloCard > col) { spd[col]  [row]   += 8; }
    if (           0 < col) { spd[col-1][row]   += 4; }
  }
  if (0 < row) {
      if (nColCaloCard > col) { spd[col]  [row-1] += 2; }
      if (           0 < col) { spd[col-1][row-1] += 1; }
  }
}

/** Check if the ECAL candidate (current card) matches the HCAL
 *  candidate with the specified row and column
 */

bool TriggerCard::match_included ( int hCol, int hRow ) { 
  if ( m_empty ) { return false; }
  
// Bottom left projection of the HCAL cluster

  int fCol =     hCol * m_mag + m_offsetCol ;    
  int fRow =     hRow * m_mag + m_offsetRow ;

// Top right projection of the HCAL cluster

  int lCol = (hCol+2) * m_mag + m_offsetCol -2 ; 
  int lRow = (hRow+2) * m_mag + m_offsetRow -2 ;
  
  if ( (m_colMax < fCol) || (m_colMax > lCol) ) return false ;
  if ( (m_rowMax < fRow) || (m_rowMax > lRow) ) return false ;

  return true;
}

