
//-----------------------------------------------------------------------------
/** @file STHitCreator.h
 *
 *  Implementation file for class : Tf::STHitCreator
 *
 *  $Id: STHitCreator.cpp,v 1.5 2007-09-10 08:54:31 wouter Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Kernel/STChannelID.h"
#include "Kernel/STDataFunctor.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

#include "TfKernel/RegionID.h"

#include "TfKernel/IITHitCreator.h"
#include "TfKernel/ITTHitCreator.h"
#include "Event/STCluster.h"

#include "STHitCreator.h"
#include "HitCreatorGeom.h"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

namespace Tf
{
  namespace HitCreatorGeom
  {

    class STModule : public Envelope<DeSTSector>
    {
    public:
      // typedefs, basically for template traits
      typedef DeSTDetector DetectorType ;
      typedef Tf::STHit HitType ;
      typedef Tf::STHitRange HitRangeType ;
      typedef std::vector<STHit> HitContainer ;
      // constructor
      STModule(const DeSTSector& detelement)  :  Envelope<DeSTSector>(detelement), m_det(&detelement), m_isloaded(false) {}
      void clearEvent() { m_ownedhits.clear() ; m_isloaded = false ;}
      const STHitRange& hits() const { return m_hitrange ; }
      const DeSTSector& detelement() const { return *m_det ; }
      void setRange( STHits::const_iterator begin, STHits::const_iterator end ) { m_hitrange = STHitRange(begin,end) ; }
      HitContainer& ownedhits() { return m_ownedhits ; }
    private:
      const DeSTSector* m_det ;
      HitContainer m_ownedhits ; // these are the hits which the module owns
      STHitRange m_hitrange ;    // these are pointers in the global list of hits, used for navigation
      bool m_isloaded ;
    } ;

    class STDetector ;

    typedef RegionOfModules<STModule, STDetector> STRegionImp ;

    class STDetector : public Detector<STRegionImp>
    {
    public:
      STDetector(const DeSTDetector& stdetector, const std::string& clusterlocation, GaudiTool& parent) ;
      void loadHits() ;
      // no loading on demand
      void loadHits( STRegionImp::ModuleContainer::const_iterator /*begin*/,
                     STRegionImp::ModuleContainer::const_iterator /*end*/) { loadHits() ; }

    private:
      std::string m_clusterlocation ;
      GaudiTool* m_parenttool ;
    } ;

    STDetector::STDetector(const DeSTDetector& stdetector, const std::string& clusterlocation,
                           GaudiTool& parent)
      : m_clusterlocation(clusterlocation), m_parenttool(&parent)
    {
      // copy the entire hierarchy
      size_t nummodules(0) ;
      for( DeSTDetector::Sectors::const_iterator isector = stdetector.sectors().begin() ;
           isector != stdetector.sectors().end(); ++isector ) {

        RegionID regionid( LHCb::STChannelID((*isector)->elementID()) ) ;
        STRegionImp* aregion = const_cast<STRegionImp*>(region(regionid)) ;
        if(!aregion) {
          aregion = new STRegionImp(regionid,*this) ;
          insert( aregion ) ;
        }
        aregion->insert( (*isector)->elementID().sector(), new HitCreatorGeom::STModule(**isector) ) ;
        ++nummodules ;
      }
    }

    inline bool compareHitX( const Tf::STHit* lhs, const Tf::STHit* rhs) { return lhs->xT() < rhs->xT() ; }

    void STDetector::loadHits()
    {
      if(!isLoaded()) {
        // retrieve clusters
        const LHCb::STLiteCluster::FastContainer* liteCont =
          m_parenttool->get<LHCb::STLiteCluster::FastContainer>(m_clusterlocation);

        // create hits clusters. don't assume anything about order
        HitCreatorGeom::STModule* cachedSector(0) ;
        unsigned int cachedSectorID(0xFFFFFFFF) ;

        for(  LHCb::STLiteCluster::FastContainer::const_iterator iclus = liteCont->begin();
              iclus != liteCont->end(); ++iclus) {
          const LHCb::STChannelID channelid = iclus->channelID() ;

          // find the sector ;
          if( cachedSector ==0 || channelid.uniqueSector() != cachedSectorID ) {
            RegionID regionid( channelid ) ;
            STRegionImp* aregion = region(regionid) ;
            cachedSector = aregion->modules()[channelid.sector()] ;
          }
          const DeSTSector& detelement = cachedSector->detelement() ;
          cachedSector->ownedhits().push_back( Tf::STHit( detelement, *iclus) ) ;
        }

        // now set up pointers for hit navigation. we don't want any hits
        // to change place. hits should be sorted now by (station, layer,
        // region, x)
        STDetector::HitContainer& thehits = hits() ;
        thehits.clear() ; // should not be necessary
        thehits.reserve(liteCont->size()) ; // to make sure things don't change place anymore.
        for( RegionContainer::iterator ireg = regions().begin(); ireg!=regions().end(); ++ireg)
          for( STRegionImp::ModuleContainer::iterator imodule = (*ireg)->modules().begin() ;
               imodule != (*ireg)->modules().end() ; ++imodule) {
            Detector<STRegionImp>::HitContainer::iterator begin = thehits.end() ;
            for( STModule::HitContainer::const_iterator ihit = (*imodule)->ownedhits().begin() ;
                 ihit != (*imodule)->ownedhits().end(); ++ihit)
              thehits.push_back( &(*ihit) ) ;
            std::sort( begin, thehits.end(),compareHitX) ;
            // now set the pointers from the module
            (*imodule)->setRange( begin, thehits.end() ) ;
          }
        setIsLoaded(true) ;
      }
    }
  }

  template<class Trait>
  STHitCreator<Trait>::STHitCreator(const std::string& type,
				    const std::string& name,
				    const IInterface* parent):
    GaudiTool(type, name, parent),
    m_detectordata(0)
  {
    declareInterface<typename Trait::ISTHitCreator>(this);
    declareProperty("ClusterLocation", m_clusterLocation=Trait::defaultClusterLocation()) ;
    declareProperty("DetectorLocation",m_detectorLocation=Trait::defaultDetectorLocation()) ;
  }

  template<class Trait>
  STHitCreator<Trait>::~STHitCreator() { }

  template<class Trait>
  StatusCode STHitCreator<Trait>::initialize()
  {

    const StatusCode sc = GaudiTool::initialize();
    if (sc.isFailure()) return Error("Failed to initialize",sc);

    // get geometry and copy the hierarchy y to navigate hits
    const DeSTDetector* itdet = getDet<DeSTDetector>(m_detectorLocation);
    m_detectordata = new HitCreatorGeom::STDetector(*itdet,m_clusterLocation,*this) ;

    // reset pointer to list of clusters at beginevent
    incSvc()->addListener(this, IncidentType::EndEvent);

    return sc;
  }

  template<class Trait>
  StatusCode STHitCreator<Trait>::finalize()
  {
    delete m_detectordata;
    return GaudiTool::finalize();
  }

  template<class Trait>
  void STHitCreator<Trait>::handle ( const Incident& incident )
  {
    if ( IncidentType::EndEvent == incident.type() ) 
    {
      if ( m_detectordata ) m_detectordata->clearEvent() ;
    }
  }

  template<class Trait>
  Tf::STHitRange STHitCreator<Trait>::hits(const typename Trait::StationID iStation,
					   const typename Trait::LayerID iLayer) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    return m_detectordata->stations()[iStation]->layers[iLayer]->hits() ;
  }
  
  template<class Trait>
  Tf::STHitRange STHitCreator<Trait>::hits(const typename Trait::StationID iStation,
					   const typename Trait::LayerID iLayer,
					   const typename Trait::RegionID iRegion) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    return m_detectordata->stations()[iStation]->layers[iLayer]->regions[iRegion]->hits() ;
  }

  template<class Trait>
  Tf::STHitRange STHitCreator<Trait>::hits() const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    return m_detectordata->hits() ;
  }
  
  template<class Trait>
  Tf::STHitRange STHitCreator<Trait>::hits( const typename Trait::StationID iStation,
					    const typename Trait::LayerID iLayer,
					    const typename Trait::RegionID iRegion,
					    const float xmin,
					    const float xmax ) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    const Tf::HitCreatorGeom::STRegionImp* region = m_detectordata->region(iStation,iLayer,iRegion) ;
    return region->hits(xmin,xmax) ;
  }
  
  template<class Trait>
  Tf::STHitRange STHitCreator<Trait>::hits( const typename Trait::StationID iStation,
					    const typename Trait::LayerID iLayer,
					    const typename Trait::RegionID iRegion,
					    const float xmin,
					    const float xmax,
					    const float ymin,
					    const float ymax) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    const Tf::HitCreatorGeom::STRegionImp* region = m_detectordata->region(iStation,iLayer,iRegion) ;
    return region->hits(xmin,xmax,ymin,ymax) ;
  }

  template<class Trait>
  const typename STHitCreator<Trait>::STRegion*
  STHitCreator<Trait>::region( const typename Trait::StationID iStation,
			       const typename Trait::LayerID iLayer,
			       const typename Trait::RegionID  iRegion ) const
  {
    return m_detectordata->region(iStation,iLayer,iRegion) ;
  }

  //====================================================================================
  // template instantiations
  //====================================================================================

  struct IT {
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("IT") ; }
    static std::string defaultClusterLocation() { return LHCb::STLiteClusterLocation::ITClusters ; }
    typedef IITHitCreator ISTHitCreator ;
    typedef TStationID StationID ;
    typedef TLayerID   LayerID ;
    typedef ITRegionID RegionID ;
  } ;
    
  typedef STHitCreator<IT> ITHitCreator ;
  DECLARE_TOOL_FACTORY( ITHitCreator );

  
  struct TT {
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("TT") ; }
    static std::string defaultClusterLocation() { return LHCb::STLiteClusterLocation::TTClusters ; }
    typedef ITTHitCreator ISTHitCreator ;
    typedef TTStationID StationID ;
    typedef TTLayerID   LayerID ;
    typedef TTRegionID RegionID ;
  } ;
  
  typedef STHitCreator<TT> TTHitCreator ;
  DECLARE_TOOL_FACTORY( TTHitCreator );
}
