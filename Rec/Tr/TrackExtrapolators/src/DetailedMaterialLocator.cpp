
#include "GaudiKernel/SystemOfUnits.h"

#include "DetDesc/Material.h"
#include "DetDesc/IDetectorElement.h"

#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // IMaterialLocator::intersect" only partially overridden
  #pragma warning(push)
#endif
#include "DetailedMaterialLocator.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

#include <boost/lambda/lambda.hpp>
using namespace boost::lambda ;

DECLARE_TOOL_FACTORY( DetailedMaterialLocator )

DetailedMaterialLocator::DetailedMaterialLocator( const std::string& type,
							    const std::string& name,
							    const IInterface* parent )
  : 
  MaterialLocatorBase(type, name, parent),
  m_minRadThickness(1e-4),
  m_geometrypath( "/dd/Structure/LHCb" ),
  m_transportSvc( "TransportSvc", name )
{ 
  declareInterface<IMaterialLocator>(this);
  declareProperty( "MinRadThickness" , m_minRadThickness);
  declareProperty( "Geometry", m_geometrypath ) ;
}

StatusCode DetailedMaterialLocator::initialize()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "DetailedMaterialLocator::initialize() " << m_minRadThickness << endmsg ;
  StatusCode sc = MaterialLocatorBase::initialize();
  if( !sc.isSuccess() ) return Error("Failed to initialize base class",sc) ;
  
  sc = m_transportSvc.retrieve() ;
  if( !sc.isSuccess() ) return Error("Failed to get transport service",sc) ;

  // if it is zero, the transport services uses the 'standard' geometry
  auto mainvolume =  !m_geometrypath.empty() ? getDet<IDetectorElement>(m_geometrypath) : nullptr;
  m_geometry = mainvolume ? mainvolume->geometry() : nullptr ;

  return sc;
}


size_t DetailedMaterialLocator::intersect( const Gaudi::XYZPoint& start, const Gaudi::XYZVector& vect, 
						ILVolume::Intersections& intersepts ) const 
{
  // check if transport is within LHCb
  const double m_25m = 25*Gaudi::Units::m ;
  size_t rc = 0 ;
  intersepts.clear() ;
  
  if (fabs(start.x()) > m_25m || fabs(start.y()) > m_25m ||
      fabs(start.z()) > m_25m ||
      fabs(start.x()+vect.x()) > m_25m ||
      fabs(start.y()+vect.y()) > m_25m ||
      fabs(start.z()+vect.z()) > m_25m ) {
    Warning("No transport possible since destination is outside LHCb ").ignore();
    
    if(msgLevel(MSG::DEBUG)) debug() << "No transport between z= " << start.z()
                                     << " and " << start.z() + vect.z() 
                                     << ", since it reaches outside LHCb" 
                                     << "start = " << start 
                                     << " vect= " << vect << endmsg ;
  } else {
    try {
      static const std::string chronotag = "DetailedMaterialLocator" ;
      chronoSvc()->chronoStart(chronotag);
      // rather painfull: the 'intersections' call is not const
      ITransportSvc* nonconsttransportservice = const_cast<ITransportSvc*>(&(*m_transportSvc)) ;
      const double mintick = 0 ;
      const double maxtick = 1 ;
      rc = nonconsttransportservice->intersections( start, vect, mintick, maxtick, intersepts, 
						    m_minRadThickness, m_geometry);
      chronoSvc()->chronoStop(chronotag);  
    } catch (GaudiException& exception) {
      error() << "caught transportservice exception " << exception << std::endl
	      << "propagating pos/vec: "
	      << start << " / " << vect << endmsg ;
      throw exception ;
    }
  }
  return rc ;
}
