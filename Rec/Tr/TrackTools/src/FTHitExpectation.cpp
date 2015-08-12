// from GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"

//#include "Kernel/ISTChannelIDSelector.h"

// Tsa
#include "TsaKernel/Line.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line3D.h"
//#include "TsaKernel/IITExpectedHits.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

#include "LoKi/select.h"

#include "LHCbMath/GeomFun.h"
#include "FTDet/DeFTDetector.h"
#include "FTDet/DeFTLayer.h"
#include "FTDet/DeFTFibreMat.h"

// local
#include "FTHitExpectation.h"

#include <algorithm>
#include <utility>

#include "LHCbMath/GeomFun.h"

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( FTHitExpectation )


//=============================================================================
// 
//=============================================================================
FTHitExpectation::FTHitExpectation(const std::string& type,
				   const std::string& name,
				   const IInterface* parent
				   ): 
THitExpectation(type, name, parent),
  m_ftDet( NULL ), 
  m_layers(),
  m_fibremats()
  //  m_selector(NULL)
{ 
  // constructor
  //declareProperty( "SelectorType", m_selectorType = "STSelectChannelIDByElement" );
  //declareProperty( "SelectorName", m_selectorName = "ALL" );
  //declareProperty( "allStrips", m_allStrips = false);
}

//=============================================================================
// 
//=============================================================================
FTHitExpectation::~FTHitExpectation(){
  // destructor
}

//=============================================================================
// 
//=============================================================================
StatusCode FTHitExpectation::initialize()
{

  StatusCode sc = THitExpectation::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  //m_expectedITHits = tool<Tf::Tsa::IITExpectedHits>("Tf::Tsa::ITExpectedHits");

  // need to know the z of T stations....
  m_ftDet = getDet<DeFTDetector>(DeFTDetectorLocation::Default);
  if ( m_ftDet -> version() != 20 ){
    return Error("FTDetector version not implemented (different than v2)",StatusCode::FAILURE);
  }
 
  m_layers    = m_ftDet -> layers();
  m_fibremats = m_ftDet -> fibremats();

  /*
  // (selector
  if (m_selectorName != "ALL"){
    info() << "Selector name " << m_selectorName << endmsg;
    m_selector  = tool< ISTChannelIDSelector >( m_selectorType,m_selectorName );
  }
  */
  return StatusCode::SUCCESS;
}


IHitExpectation::Info FTHitExpectation::expectation(const LHCb::Track& aTrack) const{

  IHitExpectation::Info info;
  info.likelihood = 0.0;
  info.nFound = 0;
  info.nExpected = 0;;


  const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
  std::vector<LHCb::LHCbID> ftHits; ftHits.reserve(ids.size());
  LoKi::select(ids.begin(), ids.end(), std::back_inserter(ftHits), bind(&LHCbID::isFT,_1));

  for ( std::vector<DeFTLayer*>::const_iterator iterL = m_layers.begin(); 
	iterL != m_layers.end() ; 
	++iterL )
    {      
      LHCb::FTChannelID elemID( (unsigned int)std::distance( m_layers.begin(), iterL ), 0u, 0u, 0u, 0u );
      double layerZ = ((*iterL)->geometry()->toGlobal( Gaudi::XYZPoint(0. ,0., 0.) )).z();

      Tf::Tsa::Line     line    = yLine    ( aTrack, layerZ );
      Tf::Tsa::Parabola aParab  = xParabola( aTrack, layerZ );
      Tf::Tsa::Line tanLine     = aParab.tangent( layerZ );
      Tf::Tsa::Line3D aLine3D   = Tf::Tsa::createLine3D( tanLine, line, layerZ );
      std::vector<std::pair<LHCb::FTChannelID, double> > vectFTPairs;

      collectHits( aLine3D, elemID, vectFTPairs );

      int old = -1;
      for ( std::vector<std::pair<LHCb::FTChannelID, double> >::iterator iter = vectFTPairs.begin(); 
	    iter != vectFTPairs.end(); 
	    ++iter )
	{ 
	  if ( int(iter->second) != old) {
	    old = int(iter->second);
	    if ( iter->first ) {
	      ++(info.nExpected);
	      std::vector<LHCb::LHCbID>::iterator itIter = std::find( ftHits.begin(), ftHits.end(), iter->first); 
	      if (itIter != ftHits.end() ) ++(info.nFound);
	    }
	  }
	}  // pairs      
    }//layers
  return info;
}
 
void FTHitExpectation::collectHits( Tf::Tsa::Line3D aLine3D,
				    LHCb::FTChannelID chan,
				    std::vector<std::pair<LHCb::FTChannelID, double> >& vectFTPairs 
				    ) const{

  for ( std::vector<DeFTFibreMat*>::const_iterator iterF = m_fibremats.begin() ; 
	iterF != m_fibremats.end(); 
	++iterF )
    {
      if( (unsigned int)((*iterF) -> layer()) != chan.layer() ) continue;
      /*
      unsigned int quarter ;
      int module =  (*iterF) -> module();
      bool left  =  module<=4 || module==10;
      bool bottom = (*iterF) -> isBottom();
      if(  left &&  bottom ) quarter = 0;
      if( !left &&  bottom ) quarter = 1;
      if(  left && !bottom ) quarter = 2;
      if( !left && !bottom ) quarter = 3;
      */
      // propagate to z of sector
      Gaudi::Plane3D entryplane, exitplane;
      EntryExitPlanes( *(*iterF), entryplane, exitplane );

      Gaudi::XYZPoint globalEntry = intersection( aLine3D, entryplane );
      Gaudi::XYZPoint globalExit  = intersection( aLine3D, exitplane );

      LHCb::MCHit* hit = new LHCb::MCHit();
      hit -> setEntry( globalEntry );
      hit -> setDisplacement ( globalExit-globalEntry  );
      hit -> setEnergy( 1.0 );
      if( !((*iterF) -> isInside( hit -> midPoint() ) ) ) continue;
      (*iterF) -> calculateHits( hit, vectFTPairs );      /// FIXME: this is an unchecked StatusCode
      delete hit;
    }
}


void FTHitExpectation::EntryExitPlanes( DeFTFibreMat& mat,  Gaudi::Plane3D& entry,  Gaudi::Plane3D& exit ) const
{
  double xmin = mat.fibreMatMinX();
  double ymin = mat.fibreMatMinY();
  double xmax = mat.fibreMatMaxX();
  double ymax = mat.fibreMatMaxY();
  
  double xLower = -0.5*(xmax-xmin);
  double xUpper =  0.5*(xmax-xmin);
  double yLower = -0.5*(ymax-ymin);
  double yUpper =  0.5*(ymax-ymin);
  
  const Gaudi::XYZPoint g1 = mat.geometry() -> toGlobal( Gaudi::XYZPoint( xLower, yLower, 0. ) );
  const Gaudi::XYZPoint g2 = mat.geometry() -> toGlobal( Gaudi::XYZPoint( xLower, yUpper, 0. ) );
  // trajectory of middle  
  //const Gaudi::XYZPoint g3 = mat.geometry()->toGlobal(xLower, 0., 0.);
  const Gaudi::XYZPoint g4 = mat.geometry()->toGlobal(Gaudi::XYZPoint( xUpper, 0., 0.) );
  // plane
  Gaudi::Plane3D plane(g1,g2,g4);
  
  double thickness = 1.2;
  entry = Gaudi::Plane3D(plane.Normal(), mat.geometry()->toGlobal( Gaudi::XYZPoint(0.,0.,-0.5*thickness)));
  exit  = Gaudi::Plane3D(plane.Normal(), mat.geometry()->toGlobal( Gaudi::XYZPoint(0.,0., 0.5*thickness))); 
}


void FTHitExpectation::collect(const LHCb::Track& aTrack ,std::vector<LHCb::LHCbID>& ids) const{

 for ( std::vector<DeFTLayer*>::const_iterator iterL = m_layers.begin(); 
	iterL != m_layers.end() ; 
	++iterL )
    {      
      LHCb::FTChannelID elemID( (unsigned int)std::distance( m_layers.begin(), iterL ), 0u, 0u, 0u, 0u );
      double layerZ = ((*iterL)->geometry()->toGlobal( Gaudi::XYZPoint(0. ,0., 0.) )).z();

      Tf::Tsa::Line     line    = yLine    ( aTrack, layerZ );
      Tf::Tsa::Parabola aParab  = xParabola( aTrack, layerZ );
      Tf::Tsa::Line tanLine     = aParab.tangent( layerZ );
      Tf::Tsa::Line3D aLine3D   = Tf::Tsa::createLine3D( tanLine, line, layerZ );
      std::vector<std::pair<LHCb::FTChannelID, double> > vectFTPairs;

      collectHits( aLine3D, elemID, vectFTPairs );


      int old = -1;
      ids.reserve(vectFTPairs.size());
      for ( std::vector<std::pair<LHCb::FTChannelID, double> >::iterator iter = vectFTPairs.begin(); 
	    iter != vectFTPairs.end(); 
	    ++iter )
	{ 
	  if ( int(iter->second) != old ) {
	    old = int(iter->second);
	    if ( iter->first) ids.push_back( LHCb::LHCbID(iter->first) );
	  }
	}  // pairs      
    }//layers
}



unsigned int FTHitExpectation::nExpected(const LHCb::Track& aTrack) const{

  return expectation(aTrack).nExpected;
}  

Gaudi::XYZPoint FTHitExpectation::intersection(const Tf::Tsa::Line3D& line,
					       const Gaudi::Plane3D& aPlane) const{
  
  // make a plane
  Gaudi::XYZPoint inter;
  double mu = 0;
  Gaudi::Math::intersection( line, aPlane, inter, mu );
  return inter;
}
