#include "LLTTriggerCard.h"

/** Clean up for a new event. Resets all arrays and values
 */

void LLTTriggerCard::reset( ) {
  int i;
  int j;
  for ( i = 0; nColCaloCard >= i; ++i ){
    for ( j = 0; nRowCaloCard >= j; ++j ) {
      et[i][j] = 0;
    }
  }

  m_etMax  = 0;
  m_multiplicity = 0 ;
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

void LLTTriggerCard::addEt( int col, int row, int digit , int mult_thresh ) {
  m_empty  = false;
  if ( (nColCaloCard > col) && (nRowCaloCard > row) ) {
    if ( digit > mult_thresh ) ++m_multiplicity ;
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
