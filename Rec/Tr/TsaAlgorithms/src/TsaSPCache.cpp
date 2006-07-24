// $Id: TsaSPCache.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $

#include "TsaSPCache.h"

void TsaSPCache::configure(Tsa::SpacePoints::iterator first, 
                           Tsa::SpacePoints::iterator last){

  m_begin = first;
  m_end = last;
  reset();
}

void TsaSPCache::reset(){

  m_cachedFirst = m_begin;
  m_cachedLast = m_begin;
  
}
