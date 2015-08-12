//-----------------------------------------------------------------------------
/** @file STHitCreator.cpp
 *
 *  Implementation file for class : Tf::STHitCreator
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/IIncidentSvc.h"
#include "Kernel/STChannelID.h"
#include "Kernel/STDataFunctor.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "Event/STLiteCluster.h"

#include "TfKernel/RegionID.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/ITTHitCreator.h"
#include "TfKernel/IUTHitCreator.h"

#include "STHitCreator.h"
#include "HitCreatorGeom.h"

namespace Tf
{
  namespace HitCreatorGeom
  {

    class STModule final : public Envelope<DeSTSector>
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

    class STDetector final : public Detector<STRegionImp>
    {
    public:
      STDetector(const DeSTDetector& stdetector, const std::string& clusterlocation, GaudiTool& parent) ;
      ~STDetector() { clearEvent(); }
      void loadHits() ;
      // no loading on demand
      void loadHits( STRegionImp::ModuleContainer::const_iterator /*begin*/,
                     STRegionImp::ModuleContainer::const_iterator /*end*/) { loadHits() ; }
      void clearEvent() {
	m_clusters = nullptr ;
	Detector<STRegionImp>::clearEvent() ;
      }
      const LHCb::STLiteCluster::FastContainer* clusters() {
	return m_clusters ? m_clusters :
	  (m_clusters = m_parenttool->get<LHCb::STLiteCluster::FastContainer>(m_clusterlocation) ) ;
      }
    private:
      std::string m_clusterlocation ;
      GaudiTool* m_parenttool ;
      const LHCb::STLiteCluster::FastContainer* m_clusters ;
    } ;

    STDetector::STDetector(const DeSTDetector& stdetector, const std::string& clusterlocation,
                           GaudiTool& parent)
      : m_clusterlocation(clusterlocation), m_parenttool(&parent), m_clusters{nullptr}
    {
      // copy the entire hierarchy
      for( const auto& sector : stdetector.sectors() ) {
        RegionID regionid{ LHCb::STChannelID(sector->elementID()) } ;
        STRegionImp* aregion = const_cast<STRegionImp*>(region(regionid)) ;
        if(!aregion) {
          aregion = new STRegionImp(regionid,*this) ;
          insert( aregion ) ;
        }
        aregion->insert( sector->elementID().sector(), new HitCreatorGeom::STModule(*sector) ) ;
      }
    }

    struct compareHitX {
      bool operator()(const Tf::STHit* lhs, const Tf::STHit* rhs) const { return lhs->xT() < rhs->xT() ; }
    };

    void STDetector::loadHits()
    {
      if(!isLoaded()) {
        // retrieve clusters
        const auto* liteCont = clusters() ;
	
        // create hits clusters. don't assume anything about order
        HitCreatorGeom::STModule* cachedSector{nullptr} ;
        unsigned int cachedSectorID(0xFFFFFFFF) ;

        for( const auto& clus : *liteCont ) {
          const LHCb::STChannelID channelid = clus.channelID() ;
          // find the sector ;
          if( !cachedSector || channelid.uniqueSector() != cachedSectorID ) {
            STRegionImp* aregion = region( RegionID{ channelid } ) ;
            cachedSector = aregion->modules()[channelid.sector()] ;
          }
          cachedSector->ownedhits().emplace_back( cachedSector->detelement(), clus ) ;
        }

        // now set up pointers for hit navigation. we don't want any hits
        // to change place. hits should be sorted now by (station, layer,
        // region, x)
        auto& thehits = hits() ;
        thehits.clear() ; // should not be necessary
        thehits.reserve(liteCont->size()) ; // to make sure things don't change place anymore.
        for(const auto& reg : regions() ) for( const auto& module : reg->modules() ) {
            auto begin = std::end(thehits) ;
            for( auto& hit : module->ownedhits() ) thehits.push_back( &hit ) ;
            auto end = std::end(thehits);
            std::sort( begin, end, compareHitX()) ;
            // now set the pointers from the module
            module->setRange( begin, end ) ;
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
    m_stdet{nullptr}, m_detectordata{nullptr}
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

    // get geometry and copy the hierarchy to navigate hits
    m_stdet = getDet<DeSTDetector>(m_detectorLocation); 
    // we may need to register to the conditions of all modules instead
    updMgrSvc()->registerCondition( this,  const_cast<IGeometryInfo*>(m_stdet->geometry()),
				    &STHitCreator<Trait>::updateGeometry );
    
    // reset pointer to list of clusters at beginevent
    incSvc()->addListener(this, IncidentType::BeginEvent);

    return sc;
  }

  template<class Trait>
  StatusCode STHitCreator<Trait>::finalize()
  {
    m_detectordata.reset(nullptr);
    return GaudiTool::finalize();
  }

  template<class Trait>
  StatusCode STHitCreator<Trait>::updateGeometry()
  {
    if(msgLevel(MSG::DEBUG)) debug() << "In STHitCreator::updateGeometry()" << endreq ;
    m_detectordata.reset(new HitCreatorGeom::STDetector(*m_stdet,m_clusterLocation,*this)) ;
    return StatusCode::SUCCESS ;
  }

  template<class Trait>
  void STHitCreator<Trait>::handle ( const Incident& incident )
  {
    if ( IncidentType::BeginEvent == incident.type() && m_detectordata) m_detectordata->clearEvent() ;
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
					    const double xmin,
					    const double xmax ) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    const auto* region = m_detectordata->region(iStation,iLayer,iRegion) ;
    return region->hits(xmin,xmax) ;
  }
  
  template<class Trait>
  Tf::STHitRange STHitCreator<Trait>::hitsLocalXRange( const typename Trait::StationID iStation,
						       const typename Trait::LayerID iLayer,
						       const typename Trait::RegionID iRegion,
						       const double xmin,
						       const double xmax ) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    const auto* region = m_detectordata->region(iStation,iLayer,iRegion) ;
    return region->hitsLocalXRange(xmin,xmax) ;
  }
  
  template<class Trait>
  Tf::STHitRange STHitCreator<Trait>::hits( const typename Trait::StationID iStation,
					    const typename Trait::LayerID iLayer,
					    const typename Trait::RegionID iRegion,
					    const double xmin,
					    const double xmax,
					    const double ymin,
					    const double ymax) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    const auto* region = m_detectordata->region(iStation,iLayer,iRegion) ;
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

  template<class Trait>
  Tf::STHit STHitCreator<Trait>::hit(LHCb::STChannelID stid) const
  {
    const LHCb::STLiteCluster::FastContainer* clusters = m_detectordata->clusters() ;
    auto iclus =  clusters->find< LHCb::STLiteCluster::findPolicy >( stid )  ;
    if( iclus == clusters->end() )
      throw GaudiException("STHitCreator::hit cannot find cluster", "STHitCreatorException" , StatusCode::FAILURE ) ;
    const DeSTSector* sector = m_stdet->findSector( stid ) ;
    return { *sector, *iclus } ;
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
  DECLARE_TOOL_FACTORY( ITHitCreator )
  
  struct TT {
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("TT") ; }
    static std::string defaultClusterLocation() { return LHCb::STLiteClusterLocation::TTClusters ; }
    typedef ITTHitCreator ISTHitCreator ;
    typedef TTStationID StationID ;
    typedef TTLayerID   LayerID ;
    typedef TTRegionID RegionID ;
  } ;
  
  typedef STHitCreator<TT> TTHitCreator ;
  DECLARE_TOOL_FACTORY( TTHitCreator )

  struct UT {
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("UT") ; }
    static std::string defaultClusterLocation() { return LHCb::STLiteClusterLocation::UTClusters ; }
    typedef IUTHitCreator ISTHitCreator ;
    typedef UTStationID StationID ;
    typedef UTLayerID   LayerID ;
    typedef UTRegionID RegionID ;
  } ;
  
  typedef STHitCreator<UT> UTHitCreator ;
  DECLARE_TOOL_FACTORY( UTHitCreator )


  // RestUsed flag for all OT hits 
  template<class Trait>
  void STHitCreator<Trait>::resetUsedFlagOfHits() const {
     auto hits = m_detectordata->hits() ;
     std::for_each( std::begin(hits), std::end(hits), [](const Tf::STHit* hit) {
         hit->resetUsedFlag();
     });
  }
}
