// GaudiKernel
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IDetDataSvc.h"

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
#include <cmath>

#include "HitCreatorGeom.h"

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
      OTModule(const DeOTModule& detelement, const OTDet::RtRelation* rtrel ) ;
      const OTHitRange& hits() const { return m_hitrange ; }
      const DeOTModule& detelement() const { return *m_det ; }
      const HitContainer& ownedhits() const { return m_ownedhits ; }
      const SortedHitContainer& sortedhits() const { return m_sortedhits ; }
      bool isLoaded() const { return m_isloaded ; }
      void print(std::ostream& os) const ;

      // loading and clearing event
      size_t loadHits(const IOTRawBankDecoder& decoder, double tmin, double tmax) ;
      void clearEvent() { m_ownedhits.clear() ;  m_sortedhits.clear() ; m_isloaded = false ;}
      void setRange( OTHits::const_iterator begin, OTHits::const_iterator end ) { m_hitrange = OTHitRange(begin,end) ; }
      static size_t moduleIndexInRegion( LHCb::OTChannelID id ) { return id.module() + (id.quarter()%2) * 9 ; }
    private:
      const DeOTModule* m_det ;
      mutable HitContainer m_ownedhits ;        // these are the hits which the module owns
      mutable SortedHitContainer m_sortedhits ; // hits sorted in x. I hope I can get rid of it.
      mutable OTHitRange m_hitrange ;           // these are pointers in the global list of hits, used for navigation
      mutable bool m_isloaded ;
      const OTDet::RtRelation* m_rtrel;  // pointer to custom rt relation (if non-null)
    } ;

    OTModule::OTModule(const DeOTModule& detelement,
                       const OTDet::RtRelation* rtrel )
      : Envelope<DeOTModule>(detelement), m_det(&detelement),
        m_isloaded(false), m_rtrel(rtrel)
    {
      // we'll take this away at some point. it is just for understanding the profiles. FIXME.
      m_ownedhits.reserve(128) ;
      m_sortedhits.reserve(128) ;
    }

    struct compareHitX {
      bool operator()(const Tf::OTHit* lhs, const Tf::OTHit* rhs) const { return lhs->xT() < rhs->xT() ; }
    };

    /// Decode this module
    size_t OTModule::loadHits(const IOTRawBankDecoder& decoder,
                              double tmin, double tmax)
    {
      if( !m_isloaded ) {
        // create the otlitetimes
        const DeOTModule& moduleelement = detelement() ;
        LHCb::OTLiteTimeRange otlitetimes = decoder.decodeModule( moduleelement.elementID() ) ;

        // create the hits
        m_ownedhits.reserve(  otlitetimes.size() );
        // put if-statement before loop to fork only once
        if ( !m_rtrel) {
          // don't use custom rt relation - drift times are on
          if( tmin < tmax ) {
            std::for_each( std::begin(otlitetimes), std::end(otlitetimes), [this,tmin,tmax,&moduleelement](const LHCb::OTLiteTime& hit) {
              auto calibTime = hit.calibratedTime() ;
              if( tmin <= calibTime && calibTime <= tmax ) this->m_ownedhits.emplace_back( moduleelement,hit ) ;
            } );
          } else {
            for( const auto& hit : otlitetimes ) m_ownedhits.emplace_back( moduleelement,hit ) ;
          }
        } else {
          // use custom rt relation - drift times are off
          const OTDet::RtRelation& rtrel = *m_rtrel;
          if( tmin < tmax ) {
            std::for_each( std::begin(otlitetimes), std::end(otlitetimes), [this,tmin,tmax,&moduleelement,&rtrel](const LHCb::OTLiteTime& hit) {
              auto calibTime = hit.calibratedTime() ;
              if( tmin <= calibTime && calibTime <= tmax ) this->m_ownedhits.emplace_back( moduleelement,hit,rtrel);
            } );
          } else {
            for( const auto& hit : otlitetimes ) m_ownedhits.emplace_back( moduleelement,hit,rtrel ) ;
          }
        }

        // create the sorted hits
        m_sortedhits.reserve(m_ownedhits.size()) ;
        std::transform( std::begin(m_ownedhits), std::end(m_ownedhits), std::back_inserter(m_sortedhits), std::addressof<OTHit> );
        std::sort( m_sortedhits.begin(), m_sortedhits.end(), compareHitX() ) ;

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
      ~OTDetector();
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
      for( const auto& station : otdet.stations() ) 
        for( const auto& layer :  station->layers() ) 
          for( const auto& quadrant : layer->quarters() ) 
            for( const auto& module : quadrant->modules() ) {
              // create the region (if it doesn't exist)
              RegionID regionid( LHCb::OTChannelID(module->elementID()) ) ;
              OTRegionImp* aregion = const_cast<OTRegionImp*>(region(regionid)) ;
              if(!aregion) {
                aregion = new OTRegionImp(regionid,*this) ;
                insert( aregion ) ;
              }
              aregion->insert( OTModule::moduleIndexInRegion(module->elementID()),
                               new HitCreatorGeom::OTModule(*module, m_parent->getRtRelation()) ) ;
              ++nummodules ;
            }
      for(const auto& reg : regions() ) for(auto& mod : reg->modules() ) 
          m_modules.push_back( mod ) ;

    }

    OTDetector::~OTDetector() { clearEvent(); }


    void OTDetector::loadHits( OTRegionImp::ModuleContainer::const_iterator begin,
                               OTRegionImp::ModuleContainer::const_iterator end)
    {
      if(!isLoaded()) {
        unsigned int numhits(0) ;
        std::for_each( begin, end, [this,&numhits](const OTModule* module) { 
          if (!module->isLoaded()) {
            const_cast<OTModule*>(module)->loadHits(*(this->m_parent->decoder()),this->m_parent->tmin(),this->m_parent->tmax()) ;
          }
          numhits += module->sortedhits().size() ;
        } );

        // FIXME. This invalidates all pointers to hits ranges,
        // including those set in 'other' modules.
        HitContainer& thehits = hits() ;
        thehits.resize(numhits,0) ;
        auto lasthit  = std::begin(thehits);
        std::for_each( begin, end, [&lasthit]( const OTModule* module ) { 
          auto firsthit = lasthit ;
          lasthit = std::copy( std::begin(  module->sortedhits() ), std::end( module->sortedhits()),  firsthit  );
          (const_cast<OTModule*>(module))->setRange( firsthit, lasthit ) ;
        } );
      }
    }

    void OTDetector::loadHits()
    {
      loadHits(m_modules.begin(), m_modules.end() ) ;
      setIsLoaded(true) ;
    }
  }

  DECLARE_TOOL_FACTORY( OTHitCreator )


  OTHitCreator::OTHitCreator(const std::string& type,
                             const std::string& name,
                             const IInterface* parent):
    GaudiTool(type, name, parent),
    m_otdecoder{"OTRawBankDecoder"},
    m_rejectOutOfTime{false},
    m_tmin(-8*Gaudi::Units::ns),
    m_tmax(56*Gaudi::Units::ns)
  {
    // interfaces
    declareInterface<IOTHitCreator>(this);
    declareProperty( "RejectOutOfTime", m_rejectOutOfTime) ;
    declareProperty( "TMin",m_tmin ) ;
    declareProperty( "TMax",m_tmax ) ;
    declareProperty( "NoDriftTimes", m_noDriftTimes = false );
    declareProperty( "ForceDriftRadius",
                     m_forceDriftRadius = 0. * Gaudi::Units::mm );
    declareProperty( "ForceResolution",
                     m_forceResolution = 5. * Gaudi::Units::mm / std::sqrt(12.) );
    declareProperty("RawBankDecoder",m_otdecoder) ;
  }


  StatusCode OTHitCreator::initialize()
  {
    StatusCode sc = GaudiTool::initialize();
    if (sc.isFailure()){
      return Error("Failed to initialize",sc);
    }

    // reset pointer to list of clusters at beginevent
    incSvc()->addListener(this, IncidentType::BeginEvent);

    // tool handle to the otlitetimedecoder
    m_otdecoder.retrieve().ignore() ;

    // reset the limits if we don't cut on the time
    if(!m_rejectOutOfTime) {
      m_tmin = 1 ;
      m_tmax = 0 ;
    }

    // get geometry
    m_otdetector = getDet<DeOTDetector>(DeOTDetectorLocation::Default);

    // set up things for use without drift time (if desired)
    if (m_noDriftTimes) {
      // (nearly) constant drift radii are obtained by assuming a tiny
      // drift velocity
      // this drift velocity was chosen such that it does not upset the
      // Newton-Raphson method in RtRelation (which returns -1 as drift radius
      // if it thinks it is not converging), so there is a slight variation
      // in the drift radii reported (on the order of 25 micron)
      // making the drift velocity significantly smaller is not an option,
      // because we then trigger non-convergence of the Newton-Raphson method
      // in RtRelation...
      const double vdrift = m_otdetector->modules().front()->rtRelation().rmax() /
        (4e-6 * Gaudi::Units::s);
      // need a vector to construct polynomial approximation to the tweaked
      // rt relation; rtrelpoly: t(r) = rtrelpoly[0] + rtrelpoly[1] * r + ...
      // a linear approximation does the job
      std::vector<double> rtrelpoly{ - m_forceDriftRadius / vdrift, 1. / vdrift };
      // construct new r-t relation - all drift times are mapped to
      // m_forceDriftRadius and the resolution is set to m_forceResolution
      m_rtrel.reset(new OTDet::RtRelation(m_otdetector->modules().front()->rtRelation().rmax(),
                                          rtrelpoly, m_forceResolution) );
      /* if we run without drift times, this should appear in the log */
      info() << "Drift times are not used, drift radius set to " <<
        m_forceDriftRadius / Gaudi::Units::mm <<
        " mm, resolution set to " << m_forceResolution / Gaudi::Units::mm
             << " mm." << endmsg;
      info() << "rt relation gives: r(2ns) = " <<
        m_rtrel->radius(2.) / Gaudi::Units::mm << " mm, r(40ns) " <<
        m_rtrel->radius(40.) / Gaudi::Units::mm << " mm." << endmsg;
    } else {
      /* we want a message if we run in debugging mode, just to make
       * sure we're aware of what we're doing */
      if(msgLevel(MSG::DEBUG)) debug() << "Drift times are used." << endmsg;
    }

    // we may need to register to the conditions of all modules instead
    updMgrSvc()->registerCondition( this, const_cast<IGeometryInfo*>(m_otdetector->geometry()),
                                    &OTHitCreator::updateGeometry );
    sc = updateGeometry();
    return sc ;
  }

  StatusCode OTHitCreator::finalize()
  {
    m_detectordata.reset( nullptr );
    m_rtrel.reset( nullptr );
    m_otdecoder.release().ignore() ;
    return GaudiTool::finalize() ;
  }

  StatusCode OTHitCreator::updateGeometry()
  {
    m_detectordata.reset( new HitCreatorGeom::OTDetector(*this,*m_otdetector) );
    return StatusCode::SUCCESS ;
  }

  void OTHitCreator::handle ( const Incident& incident )
  {
    if ( IncidentType::BeginEvent == incident.type() && m_detectordata )
       m_detectordata->clearEvent() ;
  }

  Tf::OTHitRange OTHitCreator::hits(const TStationID iStation,
                                    const TLayerID iLayer ) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    return m_detectordata->stations()[iStation]->layers[iLayer]->hits() ;
  }

  Tf::OTHitRange OTHitCreator::hits(const TStationID iStation) const
  {
    if( !m_detectordata->isLoaded() ) m_detectordata->loadHits() ;
    return m_detectordata->stations()[iStation]->hits() ;
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
                                     const double xmin,
                                     const double xmax ) const
  {
    const Tf::HitCreatorGeom::OTRegionImp* region = m_detectordata->region(iStation,iLayer,iRegion) ;
    return region->hits(xmin,xmax) ;
  }

  Tf::OTHitRange OTHitCreator::hitsLocalXRange( const TStationID iStation,
                                                const TLayerID iLayer,
                                                const OTRegionID iRegion,
                                                const double xmin,
                                                const double xmax ) const
  {
    const Tf::HitCreatorGeom::OTRegionImp* region = m_detectordata->region(iStation,iLayer,iRegion) ;
    return region->hitsLocalXRange(xmin,xmax) ;
  }

  Tf::OTHitRange OTHitCreator::hits( const TStationID iStation,
                                     const TLayerID iLayer,
                                     const OTRegionID iRegion,
                                     const double xmin,
                                     const double xmax,
                                     const double ymin,
                                     const double ymax ) const
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
    return thisregion->modules()[modindex] ;
  }

  Tf::OTHit OTHitCreator::hit( const LHCb::OTChannelID id ) const
  {
    // we could as well use OTDet here .. we only need the element.
    const DeOTModule& thismodule = module(id)->detelement() ;
    if ( m_rtrel )
      return Tf::OTHit( thismodule, m_otdecoder->time( id ), *m_rtrel ) ;
    else
      return Tf::OTHit( thismodule, m_otdecoder->time( id )  ) ;
  }

  // return a pointer to an rt relation for switching drift times off
  // or null if that is not desired
  const OTDet::RtRelation* OTHitCreator::getRtRelation() const
  { return m_rtrel.get(); }


  // RestUsed flag for all OT hits
  void OTHitCreator::resetUsedFlagOfHits() const
  {
    std::for_each( std::begin( m_detectordata->hits() ), std::end( m_detectordata->hits() ),
                   [](const OTHit *hit) { hit->resetUsedFlag() ; } );
  }
}
