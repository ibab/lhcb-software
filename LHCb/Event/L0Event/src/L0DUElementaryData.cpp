// $Id: L0DUElementaryData.cpp,v 1.11 2010-01-22 09:12:42 odescham Exp $
// Include files 
#include <utility>
#include <string>
#include <map>
#include <sstream>

#include "Event/L0DUElementaryData.h"

//-----------------------------------------------------------------------------
// Auxilliary Implementation file for L0DUElementaryData Event classes
//
// 09/02/2006 : Olivier Deschamps
//-----------------------------------------------------------------------------

void LHCb::L0DUElementaryData::evaluate(int bx){
  if( LHCb::L0DUElementaryData::Compound != m_type)return;                        // not a compound  to be evaluated
  if( m_evaluated && m_digit.find(bx)!= m_digit.end() )return;                    // already evaluated
  if( m_componentsName.size() != m_components.size() ){m_evaluated=false;return;} // wrong setting

  if( m_components.size() == 2 ){
    // -- 2D operators :
    LHCb::L0DUElementaryData* d1 = m_components[0];
    LHCb::L0DUElementaryData* d2 = m_components[1];
    if( d1 == NULL || d2 == NULL){m_evaluated=false;return;} // missing data
    bool hasData  = (d1->hasData(bx) && d2->hasData(bx)); 
    if( !hasData){m_evaluated=false;return;} // incomplete data
    unsigned int dd1=d1->digit( bx );
    unsigned int dd2=d2->digit( bx );
    if      ( "+" == m_operator )m_digit[bx] = dd1 + dd2 ; 
    else if ( "-" == m_operator )m_digit[bx] = ( dd1 > dd2 ) ? dd1 - dd2 : 0;
    else if ( "&" == m_operator )m_digit[bx] = dd1 & dd2 ;
    else if ( "^" == m_operator )m_digit[bx] = dd1 ^ dd2 ;
    else m_evaluated=false;
    m_saturation = d1->saturation() + d2->saturation();
    m_scale      = (d1->scale() != d2->scale() ) ? 1. : d1->scale();
  }else{
    // -- Ndim operators (not defined so far)
    m_evaluated=false;
  }
  return;
}


std::string LHCb::L0DUElementaryData::summary(){
  std::ostringstream s(" ");
  for(std::map<int,unsigned int>::iterator ibx = m_digit.begin() ; m_digit.end()!= ibx; ibx++){
    s << summary(ibx->first) << std::endl;
  }
  return s.str();
}

std::string LHCb::L0DUElementaryData::summary(int bx){
  std::ostringstream s(" ");
  s << description(bx);
  s << " = [" << digit(bx) << "] ";
  if(scale() != 1.)s << " => [" << value(bx) << " MeV] " ;
  if( ! hasData(bx) )s<< " : DATA UNSET !";
  return s.str();
}

std::string LHCb::L0DUElementaryData::description(int bx){
  std::ostringstream s(" ");
  s << " " << m_name ;
  if( bx != 0 )s << " [BX="<<bx<<"] ";
  if(m_type == LHCb::L0DUElementaryData::Compound)s << " = [" << m_operator << "](" << m_componentsName << ")" ;
  return s.str();

}

