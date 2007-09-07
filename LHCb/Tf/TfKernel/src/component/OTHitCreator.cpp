// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// ITDet

#include "Kernel/OTChannelID.h"
#include "Kernel/OTDataFunctor.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTModule.h"
#include "OTDet/DeOTQuarter.h"

#include "OTHitCreator.h"

#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>

#include "HitCreatorGeom.h"

//#include "TfKernel/RegionID.h"
#include "TfKernel/Region.h"
#include "TfKernel/OTHit.h"

namespace Tf
{
  namespace HitCreatorGeom {

    //////////////////////////////////////////////////////////////////////////////////////////
    /** @class OTModule */
    /////////////////////////////////////////////////////////////////////////////////////////

    class OTModule : public Envelope<DeOTModule>
    {
    public:
      // typedefs, basically for template traits
      typedef Tf::OTHit HitType ;
      typedef Tf::OTHitRange HitRangeType ;
      typedef std::vector<OTHit> HitContainer ;
      typedef std::vector<const OTHit*> SortedHitContainer ;
      // constructor
      OTModule(const DeOTModule& detelement ) ;
      const OTHitRange& hits() const { return m_hitrange ; }
      const DeOTModule& detelement() const { return *m_det ; }
      const HitContainer& ownedhits() const { return m_ownedhits ; }
      const SortedHitContainer& sortedhits() const { return m_sortedhits ; }
      bool isLoaded() const { return m_isloaded ; }
      void print(std::ostream& os) const ;

      // loading and clearing event
      size_t loadHits(const OTRawBankDecoder& decoder, float tmin, float tmax) ;
      void clearEvent() { m_ownedhits.clear() ;  m_sortedhits.clear() ; m_isloaded = false ;}
      void setRange( OTHits::const_iterator begin, OTHits::const_iterator end ) { m_hitrange = OTHitRange(begin,end) ; }
      static size_t moduleIndexInRegion( LHCb::OTChannelID id ) { return id.module() + (id.quarter()%2) * 9 ; }
     private:
      const DeOTModule* m_det ;
      mutable HitContainer m_ownedhits ;        // these are the hits which the module owns
      mutable SortedHitContainer m_sortedhits ; // hits sorted in x. I hope I can get rid of it.
      mutable OTHitRange m_hitrange ;           // these are pointers in the global list of hits, used for navigation
      mutable bool m_isloaded ;
    } ;

    OTModule::OTModule(const DeOTModule& detelement )
      : Envelope<DeOTModule>(detelement), m_det(&detelement), m_isloaded(false)
    {
      // we'll take this away at some point. it is just for understanding the profiles. FIXME.
      m_ownedhits.reserve(128) ;
      m_sortedhits.reserve(128) ;
    }

    inline bool compareHitX( const Tf::OTHit* lhs, const Tf::OTHit* rhs) { return lhs->xT() < rhs->xT() ; }

    /// Decode this module
    size_t OTModule::loadHits(const OTRawBankDecoder& decoder,
                              float tmin, float tmax)
    {
      if( !m_isloaded ) {
        // create the otlitetimes
        const DeOTModule& moduleelement = detelement() ;
        LHCb::OTLiteTimeRange otlitetimes = decoder.decodeModule( moduleelement.elementID() ) ;

        // create the hits
        size_t numhits = otlitetimes.size() ;
        m_ownedhits.reserve( numhits ) ;
        // put if-statement before loop to fork only once
        if( tmin < tmax ) {
          for( LHCb::OTLiteTimeRange::const_iterator ihit = otlitetimes.begin() ;
               ihit != otlitetimes.end(); ++ihit) {
            float calibTime = ihit->rawTime() - moduleelement.strawT0( ihit->channel().straw() ) ;
            if( tmin <= calibTime && calibTime <= tmax )
              m_ownedhits.push_back( Tf::OTHit(moduleelement,*ihit ) ) ;
          }
        } else {
          for( LHCb::OTLiteTimeRange::const_iterator ihit = otlitetimes.begin() ;
               ihit != otlitetimes.end(); ++ihit)
            m_ownedhits.push_back( Tf::OTHit(moduleelement,*ihit ) ) ;
        }

        // create the sorted hits
        m_sortedhits.reserve(m_ownedhits.size()) ;
        for(HitContainer::const_iterator it = m_ownedhits.begin();
            it !=  m_ownedhits.end(); ++it) m_sortedhits.push_back( &(*it) ) ;
        std::sort( m_sortedhits.begin(), m_sortedhits.end(), compareHitX) ;

        m_isloaded = true ;
      }
      return m_ownedhits.size() ;
    }

    class OTDetector ;
    typedef RegionOfModules<OTModule,OTDetector> OTRegionImp ;


    class OTDetector : public Detector<OTRegionImp>
    {
    public:
      OTDetector(const OTHitCreator& parent, const DeOTDetector& otdet) ;
      void loadHits( OTRegionImp::ModuleContainer::const_iterator begin, OTRegionImp::ModuleContainer::const_iterator end) ;
      void loadHits() ;
    private:
      const OTHitCreator* m_parent ;
      std::vector<OTModule*> m_modules ; // sorted modules for fast access
    } ;

    OTDetector::OTDetector(const OTHitCreator& parent, const DeOTDetector& otdet)
      : m_parent(&parent)
    {
      size_t nummodules(0) ;
      for( DeOTDetector::Stations::const_iterator istation = otdet.stations().begin() ;
           istation != otdet.stations().end() ; ++istation)
        for( DeOTStation::Layers::const_iterator ilayer = (*istation)->layers().begin() ;
             ilayer != (*istation)->layers().end() ; ++ilayer)
          for( DeOTLayer::Quarters::const_iterator iquadrant = (*ilayer)->quarters().begin() ;
               iquadrant != (*ilayer)->quarters().end() ; ++iquadrant)
            for( DeOTQuarter::Modules::const_iterator imodule = (*iquadrant)->modules().begin() ;
                 imodule != (*iquadrant)->modules().end() ; ++imodule) {
              // create the region (if it doesn't exist)
              RegionID regionid( LHCb::OTChannelID((*imodule)->elementID()) ) ;
              OTRegionImp* aregion = const_cast<OTRegionImp*>(region(regionid)) ;
              if(!aregion) {
                aregion = new OTRegionImp(regionid,*this) ;
                insert( aregion ) ;
              }
	      size_t moduleindex = OTModule::moduleIndexInRegion((*imodule)->elementID()) ;
              aregion->insert( moduleindex, new HitCreatorGeom::OTModule(**imodule) ) ;
              ++nummodules ;
            }
      for(RegionContainer::iterator ireg = regions().begin() ; ireg != regions().end() ; ++ireg)
        for(OTRegionImp::ModuleContainer::iterator it = (*ireg)->modules().begin() ; it != (*ireg)->modules().end() ; ++it)
          m_modules.push_back( *it ) ;

    }


    void OTDetector::loadHits( OTRegionImp::ModuleContainer::const_iterator begin,
                               OTRegionImp::ModuleContainer::const_iterator end)
    {
      if(!isLoaded()) {
        unsigned int numhits(0) ;
        for(OTRegionImp::ModuleContainer::const_iterator imodule=begin ;
            imodule!= end; ++imodule)
          if(!(*imodule)->isLoaded()) {
            OTModule* module = const_cast<OTModule*>(*imodule) ;
            numhits += module->loadHits(*(m_parent->decoder()),m_parent->tmin(),m_parent->tmax()) ;
          }

        // FIXME. This invalidates all pointers to hits ranges,
        // including those set in 'other' modules.
        HitContainer& thehits = hits() ;
        thehits.resize(numhits,0) ;
        HitContainer::const_iterator hitsbegin = thehits.begin() ;
        size_t lasthit(0) ;
        for(OTRegionImp::ModuleContainer::const_iterator imodule = begin ; imodule!= end; ++imodule) {
          size_t firsthit = lasthit ;
          for(OTModule::SortedHitContainer::const_iterator ihit = (*imodule)->sortedhits().begin() ;
              ihit != (*imodule)->sortedhits().end(); ++ihit)
            thehits[lasthit++] = *ihit ;
          (const_cast<OTModule*>(*imodule))->setRange( hitsbegin + firsthit, hitsbegin + lasthit ) ;
        }
      }
    }

    void OTDetector::loadHits()
    {
      loadHits(m_modules.begin(), m_modules.end() ) ;
      setIsLoaded(true) ;
    }

  }

  DECLARE_TOOL_FACTORY( OTHitCreator );


  OTHitCreator::OTHitCreator(const std::string& type,
                             const std::string& name,
                             const IInterface* parent):
    GaudiTool(type, name, parent),
    m_otdecoder("OTRawBankDecoder"),
    m_rejectOutOfTime(false),
    m_tmin(-8*Gaudi::Units::ns),
    m_tmax(56*Gaudi::Units::ns),
    m_detectordata(0)
  {
    // interfaces
    declareInterface<IOTHitCreator>(this);
    declareProperty( "RejectOutOfTime", m_rejectOutOfTime) ;
    declareProperty( "TMin",m_tmin ) ;
    declareProperty( "TMax",m_tmax ) ;
  }

  OTHitCreator::~OTHitCreator()
  {
    // destructer
  }

  StatusCode OTHitCreator::initialize()
  {
    StatusCode sc = GaudiTool::initialize();
    if (sc.isFailure()){
      return Error("Failed to initialize",sc);
    }

    // reset pointer to list of clusters at beginevent
    incSvc()->addListener(this, IncidentType::EndEvent);

    // tool handle to the otlitetimedecoder
    sc = m_otdecoder.retrieve() ;

    // get geometry and copy the hierarchy y to navigate hits. do we
    // want to delay this till the first event?
    const DeOTDetector* otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
    m_detectordata = new HitCreatorGeom::OTDetector(*this,*otdet) ;

    // reset the limits if we don't cut on the time
    if(!m_rejectOutOfTime) {
      m_tmin = 1 ;
      m_tmax = 0 ;
    }

    return sc ;
  }

  StatusCode OTHitCreator::finalize()
  {
    if(m_detectordata) {
      delete m_detectordata ;
      m_detectordata = 0 ;
    }
    return GaudiTool::finalize() ;
  }

  void OTHitCreator::handle ( const Incident& incident )
  {
    if ( IncidentType::EndEvent == incident.type() ) m_detectordata->clearEvent() ;
  }

  Tf::OTHitRange OTHitCreator::hits(const TStationID iStation,
                                    const TLayerID iLayer ) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    return m_detectordata->stations()[iStation]->layers[iLayer]->hits() ;
  }

  Tf::OTHitRange OTHitCreator::hits(const TStationID iStation,
                                    const TLayerID iLayer,
                                    const OTRegionID iRegion ) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    return m_detectordata->stations()[iStation]->layers[iLayer]->regions[iRegion]->hits() ;
  }

  Tf::OTHitRange OTHitCreator::hits() const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    return m_detectordata->hits() ;
  }

  Tf::OTHitRange OTHitCreator::hits( const TStationID iStation,
                                     const TLayerID iLayer,
                                     const OTRegionID iRegion,
                                     const float xmin,
                                     const float xmax ) const
  {
    const Tf::HitCreatorGeom::OTRegionImp* region = m_detectordata->region(iStation,iLayer,iRegion) ;
    return region->hits(xmin,xmax) ;
  }

  Tf::OTHitRange OTHitCreator::hits( const TStationID iStation,
                                     const TLayerID iLayer,
                                     const OTRegionID iRegion,
                                     const float xmin,
                                     const float xmax,
                                     const float ymin,
                                     const float ymax ) const
  {
    const Tf::HitCreatorGeom::OTRegionImp* region = m_detectordata->region(iStation,iLayer,iRegion) ;
    return region->hits(xmin,xmax,ymin,ymax) ;
  }

  const Tf::IOTHitCreator::OTRegion*
  OTHitCreator::region( const TStationID iStation,
                        const TLayerID iLayer,
                        const OTRegionID iRegion ) const
  {
    return m_detectordata->region(iStation,iLayer,iRegion) ;
  }


  // Retrieve an OTModule
  const Tf::HitCreatorGeom::OTModule* OTHitCreator::module( const LHCb::OTChannelID id ) const 
  {
    Tf::RegionID regionid(id) ;
    const Tf::HitCreatorGeom::OTRegionImp* thisregion = 
      m_detectordata->region(regionid.station(),regionid.layer(),regionid.region()) ;
    size_t modindex = Tf::HitCreatorGeom::OTModule::moduleIndexInRegion( id ) ;
    const Tf::HitCreatorGeom::OTModule* thismodule = thisregion->modules()[modindex] ;
    assert( thismodule->detelement().elementID() == id ) ;
    return thismodule ;
  }

  Tf::OTHit OTHitCreator::hit( const LHCb::OTChannelID id ) const
  {
    // we could as well use OTDet here .. we only need the element.
    const Tf::HitCreatorGeom::OTModule* thismodule = module(id) ;
    return Tf::OTHit( thismodule->detelement(), LHCb::OTLiteTime(id)) ;
  }
  
  //   void OTHitCreator::insertHits(const std::vector<size_t>& modules) const
  //   {
  //     // this call invalidates all pointers in modules that we don't
  //     // currently need. the problem is that it is far too expensive to
  //     // fill all of them.


  //     size_t totalnuminsertedhits(0) ;
  //     for(  std::vector<unsigned int>::const_iterator imodindex = modules.begin() ;
  //    imodindex != modules.end(); ++imodindex ) {

  //       HitCreatorGeom::OTDetector::ModuleContainer::iterator imodule = m_detectordata->modules().begin() + *imodindex ;
  //       HitCreatorGeom::OTModule& module = **imodule ;
  //       if( ! module.isLoaded() ) {

  //  // create the hits, sort etc.
  //  unsigned int numinsertedhits = loadHits(module) ;

  //  if( numinsertedhits > 0 ) {
  //    totalnuminsertedhits += numinsertedhits ;

  //    // I still need to figure out if the insertion is faster or
  //    // slower than just resetting all pointers.
  //    if( tryinsertion ) {

  //      // insert in the master hit list
  //      HitCreatorGeom::OTDetector::HitContainer& hits = m_detectordata->hits() ;
  //      HitCreatorGeom::OTDetector::HitContainer::iterator oldbegin = hits.begin() ;

  //      // this is just a const-cast!
  //      HitCreatorGeom::OTDetector::HitContainer::iterator modulebegin = oldbegin + (module.hits().begin()-oldbegin) ;

  //      hits.insert(modulebegin , module.sortedhits().begin(), module.sortedhits().end() ) ;

  //      // now correct all pointers from modules into master hit list
  //      bool afterinsertedmodule = false ;
  //      HitCreatorGeom::OTDetector::HitContainer::iterator newbegin ;
  //      for( HitCreatorGeom::OTDetector::ModuleContainer::iterator jmodule =  m_detectordata->modules().begin() ;
  //    jmodule != m_detectordata->modules().end() ; ++jmodule ) {
  //        // this is the pointer to the first hit in this module
  //        newbegin = hits.begin() + ((*jmodule)->hits().begin() - oldbegin) ;
  //        // correct for the number of inserted hits if needed
  //        if( afterinsertedmodule ) newbegin += numinsertedhits ;
  //        // update the range
  //        (*jmodule)->setRange( newbegin, newbegin + (*jmodule)->ownedhits().size() ) ;
  //        if(jmodule == imodule) afterinsertedmodule = true ;
  //      }
  //      // check that everything adds up
  //      assert( m_detectordata->modules().back()->hits().end() == hits.end() ) ;
  //    }
  //  }
  //       }
  //     }
  //     if(!tryinsertion && totalnuminsertedhits>0) fillMasterHitList(m_detectordata->hits().size() + totalnuminsertedhits) ;
  //   }


}
