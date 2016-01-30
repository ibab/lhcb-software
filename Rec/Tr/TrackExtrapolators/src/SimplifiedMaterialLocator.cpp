#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // IMaterialLocator::intersect" only partially overridden
  #pragma warning(push)
#endif
#include "SimplifiedMaterialLocator.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

#include <set>

#include "DetDesc/Material.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/DetDesc.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/LVolume.h"
#include "DetDesc/VolumeIntersectionIntervals.h"

//TODO:FIXME: once we get to C++14, use std::make_unique instead...
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
        return std::unique_ptr<T> ( new T( std::forward<Args>(args)... ) );
}

namespace MaterialLocatorUtils {
  
  class PVolumeWrapper
  {
  public:
    PVolumeWrapper( const IPVolume& volume )
      : m_volume( volume )
    {
      // get the global position of the center 
      m_zcenter = volume.toMother( { 0,0,0 } ).z() ;
      // determine the size in z
      const SolidTubs* solidtubs = dynamic_cast<const SolidTubs*>(solid()) ;
      const SolidBox*  solidbox  = dynamic_cast<const SolidBox*>(solid()) ;
      if( solidbox ) {
	m_halfzlength = solidbox->zHalfLength() ;
      } else if( solidtubs ) {
	m_halfzlength = solidtubs->zHalfLength() ;
      } else {
	throw GaudiException(std::string("TrackingGeometry cannot deal with solid named: ") + solid()->name(),
			     "TrackingGeometry.h",StatusCode::FAILURE) ;
      }
      m_hasdaughters = lvolume()->noPVolumes()>0 ;
    }
    
    virtual ~PVolumeWrapper() = default;
    
    const std::string& name() const { return m_volume.name() ; }
    inline unsigned int addintersections(const Gaudi::XYZPoint&  p,const Gaudi::XYZVector& v, ILVolume::Intersections& ) const ;
    const Material* material() const { return m_volume.lvolume()->material() ; }
    const ISolid* solid() const { return m_volume.lvolume()->solid() ; }
    const ILVolume* lvolume() const { return m_volume.lvolume() ; }

  private:
    const IPVolume& m_volume ;
    double m_zcenter ;
    double m_halfzlength ;
    bool m_hasdaughters ;
  } ;

  unsigned int PVolumeWrapper::addintersections(const Gaudi::XYZPoint&  p,
                                                const Gaudi::XYZVector& v,
                                                ILVolume::Intersections& intersections) const
  {
    int rc(0) ;
    // do a quick z-overlap test. the intersectionTicks calls
    // don't actually test for tick boundaries. here we use that
    // our ticks must be between 0 and 1.
    double z1 = p.z()-m_zcenter ;
    double z2 = z1 + v.z() ;
    if(z1>z2) std::swap(z1,z2) ;
    if(-m_halfzlength <= z2 && z1 <= m_halfzlength) {
      // now we are goign to use that we know that the toplevel transform is unity, except for a z-translation. so not:
      //   rc = m_volume.intersectLine(p,v,intersections,0,1,0) ;
      // but
      Gaudi::XYZPoint pprime = p ; 
      pprime.SetZ(p.z()-m_zcenter) ;
      if(m_hasdaughters) {
        static ILVolume::Intersections own ;
        own.clear() ;
        rc = m_volume.lvolume()->intersectLine(pprime,v,own,0,1,0) ;
        intersections.insert( std::end(intersections), std::begin(own), std::end(own) );
      } else {
        // half the volumes don't have daughters. if they don't, we do
        // this ourself because the intersectLine call above is just
        // too slow.
        static ISolid::Ticks ticks ;
        ticks.clear() ;
        solid()->intersectionTicks(pprime,v,ticks) ;
        if(ticks.size()%2==1) std::cout << "odd number of ticks!" << std::endl ;
        for(unsigned int itick=0; itick+1<ticks.size(); itick+=2) {
          double firsttick  = ticks[itick] ;
          double secondtick = ticks[itick+1] ;
          if( firsttick <= 1 && secondtick >= 0 ) {
            intersections.emplace_back( std::make_pair( std::max(0.,firsttick), std::min(1.,secondtick) ), material() );
            ++rc ;
          }
        }
      }
    }
    return rc ;
  }
}

DECLARE_TOOL_FACTORY( SimplifiedMaterialLocator )

SimplifiedMaterialLocator::SimplifiedMaterialLocator( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : MaterialLocatorBase(type, name, parent)
{ 
  declareInterface<IMaterialLocator>(this);
  declareProperty( "Geometry" , m_tgvolname = "/dd/TrackfitGeometry/Geometry/lvLHCb" );
}

StatusCode SimplifiedMaterialLocator::initialize()
{  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "SimplifiedMaterialLocator::initialize()" << endmsg ;
  StatusCode sc = MaterialLocatorBase::initialize();
  if( sc.isSuccess() ) {
    
    DetDesc::Services* services = DetDesc::services();
    IDataProviderSvc*  detsvc  = services->detSvc();
    SmartDataPtr<const ILVolume> tgvol(detsvc,m_tgvolname) ;
    if( !tgvol ) {
      error() << "Did not find TrackfitGeometry volume " << m_tgvolname << endmsg ;
      sc = StatusCode::FAILURE ;
    } else {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Found TrackfitGeometry volume with " << tgvol->pvolumes().size() << " daughters." << endmsg ;
      m_volumes.reserve( std::distance( tgvol->pvBegin(), tgvol->pvEnd() ) );
      std::transform( tgvol->pvBegin(), tgvol->pvEnd(),
                      std::back_inserter(m_volumes),
                      []( const IPVolume* i ) { return make_unique<MaterialLocatorUtils::PVolumeWrapper>(*i); } );
    }
  }
  return sc;
}

StatusCode SimplifiedMaterialLocator::finalize()
{  
  m_volumes.clear() ;
  return MaterialLocatorBase::finalize() ;
}

inline bool compareFirstTick( const ILVolume::Intersection& lhs, const ILVolume::Intersection& rhs )
{
  return lhs.first.first<rhs.first.first ;
}

size_t SimplifiedMaterialLocator::intersect( const Gaudi::XYZPoint& start, 
                                             const Gaudi::XYZVector& vect, 
                                             ILVolume::Intersections& intersepts ) const 
{ 
  // for now, no navigation
  intersepts.clear();
  for( auto&  i : m_volumes ) i->addintersections(start,vect,intersepts) ;
  std::sort( std::begin(intersepts), std::end(intersepts), compareFirstTick) ;
  return intersepts.size() ;
}
