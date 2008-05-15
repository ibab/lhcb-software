#include "HltBase/HltData.h"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"


//-----------------------------------------------------------------------------
// Implementation file for class : HltData
//
// 2007-11-25 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

Hlt::Data::~Data() {
  std::for_each( m_selections.begin(), 
                 m_selections.end(), 
                 boost::lambda::delete_ptr() ); 
} 
