// $Id: ITDataCache.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
#include "ITDataCache.h"


ITDataCache::ITDataCache(unsigned int layer): 
 m_layer(layer)
{
  // constructer
}

void ITDataCache::configure(Tsa::STRange range){

  // configure the cache (typically once per event)
  if (range.empty() == true){
    m_empty = true;
  }
  else{
    m_empty = false;
    m_begin = range.begin();
    m_end = range.end();
  }
}


