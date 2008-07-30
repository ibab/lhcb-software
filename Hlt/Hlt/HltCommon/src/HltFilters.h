// $Id: HltFilters.h,v 1.2 2008-07-30 13:37:32 graven Exp $
#ifndef HLTFILTERS_H 
#define HLTFILTERS_H 1

#include "HltTFilter.h"

class HltTrackFilter: public HltTFilter<LHCb::Track> 
{
public:
  HltTrackFilter( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~HltTrackFilter() {}
};

class HltVertexFilter: public HltTFilter<LHCb::RecVertex> 
{
public:
  HltVertexFilter( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~HltVertexFilter() {}
};

#endif // HLTFILTERS_H
