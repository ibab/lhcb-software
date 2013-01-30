//-----------------------------------------------------------------------------
/** @file STHitCreator.cpp
 *
 *  Implementation file for class : Tf::STHitCreator
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
#include "Event/STLiteCluster.h"

#include "TfKernel/RegionID.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/ITTHitCreator.h"
#include "TfKernel/IUTHitCreator.h"

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
      void clearEvent() {
	m_clusters = 0 ;
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
      : m_clusterlocation(clusterlocation), m_parenttool(&parent), m_clusters(NULL)
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

    struct compareHitX {
      bool operator()(const Tf::STHit* lhs, const Tf::STHit* rhs) const { return lhs->xT() < rhs->xT() ; }
    };

    void STDetector::loadHits()
    {
      if(!isLoaded()) {
        // retrieve clusters
        const LHCb::STLiteCluster::FastContainer* liteCont = clusters() ;
	
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
            std::sort( begin, thehits.end(),compareHitX()) ;
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
    m_stdet(0), m_detectordata(0)
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
    if (m_detectordata) m_detectordata->clearEvent();
    delete m_detectordata;
    return GaudiTool::finalize();
  }

  template<class Trait>
  StatusCode STHitCreator<Trait>::updateGeometry()
  {
    if(msgLevel(MSG::DEBUG)) debug() << "In STHitCreator::updateGeometry()" << endreq ;
    if(m_detectordata) {
      m_detectordata->clearEvent();
      delete m_detectordata ;
    }
    m_detectordata = new HitCreatorGeom::STDetector(*m_stdet,m_clusterLocation,*this) ;
    return StatusCode::SUCCESS ;
  }

  template<class Trait>
  void STHitCreator<Trait>::handle ( const Incident& incident )
  {
    if ( IncidentType::BeginEvent == incident.type() ) 
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
					    const double xmin,
					    const double xmax ) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    const Tf::HitCreatorGeom::STRegionImp* region = m_detectordata->region(iStation,iLayer,iRegion) ;
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
    const Tf::HitCreatorGeom::STRegionImp* region = m_detectordata->region(iStation,iLayer,iRegion) ;
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

  template<class Trait>
  Tf::STHit STHitCreator<Trait>::hit(LHCb::STChannelID stid) const
  {
    const DeSTSector* sector = m_stdet->findSector( stid ) ;
    const LHCb::STLiteCluster::FastContainer* clusters = m_detectordata->clusters() ;
    LHCb::STLiteCluster::FastContainer::const_iterator iclus =  
      clusters->find< LHCb::STLiteCluster::findPolicy >( stid )  ;
    if( iclus == clusters->end() )
      throw GaudiException("STHitCreator::hit cannot find cluster", "STHitCreatorException" , StatusCode::FAILURE ) ;
    return Tf::STHit(*sector, *iclus ) ;
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
  void STHitCreator<Trait>::resetUsedFlagOfHits() const
   {
     Tf::STHitRange hits = m_detectordata->hits() ;
   
     for( Tf::STHits::const_iterator it = hits.begin(); it != hits.end() ; ++it){
       const Tf::STHit* hit = (*it);
       hit->resetUsedFlag();
     }
   }
}

