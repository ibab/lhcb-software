
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"


//From TfKernel
#include "TfKernel/STHit.h"
#include "TfKernel/OTHit.h"
#include "TfKernel/RecoFuncs.h"

// local
#include "PatKernel/PatTStationHitManager.h"


DECLARE_TOOL_FACTORY( PatTStationHitManager )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatTStationHitManager::PatTStationHitManager( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : Tf::TStationHitManager<PatForwardHit> ( type, name , parent )
{
}

//=============================================================================
// Destructor
//=============================================================================
PatTStationHitManager::~PatTStationHitManager() { }

void PatTStationHitManager::prepareHits() const
{
  if ( !allHitsPrepared() )   
    sortMyHits();
}


void PatTStationHitManager::prepareHitsInWindow(const Tf::IStationSelector & selector) 
{ 

  Tf::TStationHitManager<PatForwardHit>::prepareHitsInWindow(selector);

  sortMyHits();
}

void PatTStationHitManager::sortMyHits() const
{
  PatFwdHit* prevHit = NULL;

  for (Tf::TStationID sta=0; sta<maxStations(); ++sta)
  {
    for (Tf::TLayerID lay=0; lay<maxLayers(); ++lay)
    {
      for (Tf::OTRegionID t=0; t<maxOTRegions(); ++t)
      {

        double lastCoord = -10000000.;

        Tf::TStationHitManager<PatForwardHit>::HitRange range = hits(sta,lay,t);

        for ( PatFwdHits::const_iterator itH = range.begin();
              range.end() != itH; ++itH ) {
          PatFwdHit* hit = *itH;
          if ( hit->hit()->xAtYEq0() - lastCoord < 3. ) {
            hit->setHasPrevious( true );
            prevHit->setHasNext( true );
          }
          lastCoord = hit->hit()->xAtYEq0();
          prevHit = hit;
        }
      
	this->sortHits < Tf::increasingByXAtYEq0<PatForwardHit> > (sta,lay,t);
      }

      for (Tf::ITRegionID t=0; t<maxITRegions(); t++)
      {
        const int index = t+maxOTRegions();
	Tf::TStationHitManager<PatForwardHit>::HitRange range = hits(sta,lay,index);
        this->sortHits < Tf::increasingByXAtYEq0<PatForwardHit> > (sta,lay,index);
      }
    }
  }
}




