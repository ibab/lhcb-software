// $Id: HltFilters.h,v 1.1 2008-01-22 09:56:29 hernando Exp $
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


class HltTrackPrepare: public HltTFilter<LHCb::Track> 
{
public:
  HltTrackPrepare( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~HltTrackPrepare() {}
};

class HltVertexPrepare: public HltTFilter<LHCb::RecVertex> 
{
public:
  HltVertexPrepare( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~HltVertexPrepare() {}
};

#endif // HLTFILTERS_H
