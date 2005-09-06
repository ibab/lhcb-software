// $Id: MCProperty.cpp,v 1.1.1.1 2005-09-06 07:06:43 pkoppenb Exp $
// Include files

// local
#include "Event/MCProperty.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCProperty
//
// 2004-01-22 : Olivier Callot
//-----------------------------------------------------------------------------


//=========================================================================
//  Returns the second member of the pair with the specified key, or zero
//=========================================================================
int MCProperty::property ( const MCParticle* part) {
  if ( NULL == part ) return 0;
  int key = part->key();
  int indx;
  if ( findIndex( key, indx ) )  return m_property[indx].second;
  return 0;
}

//=========================================================================
//  
//=========================================================================
bool MCProperty::findIndex ( int key, int& index) {

  // binary search
  int iF = 0;
  int iL = m_property.size() - 1;
  while ( iF <= iL ) {
    int iM = (iF + iL ) / 2;
    int tmpKey = m_property[iM].first;

    if ( key == tmpKey ) {
      index = iM;
      return true;
    } else if ( key < tmpKey ) {
      iL = iM-1;
    } else {
      iF = iM+1;
    }
  }
  index = iF;
  return false;
}

//=========================================================================
//  Insert the pair (MCParticle->key,value) at the appropriate place.
//  l Replace if key exists
//=========================================================================
void MCProperty::setProperty ( const MCParticle* part, int value ) {
  if ( NULL == part ) return;
  int key = part->key();
  int iF;
  if ( findIndex( key, iF ) ) {
    m_property[iF].second = value;
  } else {
    m_property.push_back( std::pair<int,int>( 0, 0 ) );
    int iL = m_property.size() - 1;
    while ( iL > iF ) {
      m_property[iL] = m_property[iL-1];
      --iL;
    }
    m_property[iF] = std::pair<int,int>( key, value );
  }
}
//=============================================================================
