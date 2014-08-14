// Include files

// from std
#include <string>
#include <vector>
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/AlgFactory.h"

// from TrackEvent
#include "Event/Track.h"
#include "DetDesc/DetectorElement.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"

#include "IGetElementsToBeAligned.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/ITTHitCreator.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/Line.h"

#include <boost/foreach.hpp>

namespace Al {
  class Equations;
  class Residuals ;
}
class IHistogram1D ;
class IHistogram2D ;

/** @class AlignAlgorithm AlignAlgorithm.h
*
*
*  @author Wouter Hulsbergen
*  @date   2008-09-07
*/

struct ElementHistos
{
  IHistogram1D* resh1 ;
  IHistogram1D* resfineh1 ;
  IHistogram1D* hotresh1 ;
  IHistogram1D* hotresfineh1 ;
  IHistogram1D* respullh1 ;
} ;

class AlRobustAlignAlg : public GaudiHistoAlg
{

public:
  /// Some handy typedefs
  typedef IGetElementsToBeAligned::Elements     Elements;

  /// Standard constructor
  AlRobustAlignAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AlRobustAlignAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution

private:
  bool printDebug()   const {return msgLevel(MSG::DEBUG);};
  bool printVerbose() const {return msgLevel(MSG::VERBOSE);};
  ElementHistos* findElement(const DetectorElement& elem) ;
  template<class HitContainer>
  size_t accumulate( const HitContainer& hits, 
		     const LHCb::State& refstate,
		     const LHCb::Track& track) ;
  const DetectorElement& element (const Tf::STHit& hit) ;
  const DetectorElement& element (const Tf::OTHit& hit) ;
private:
  std::vector<ElementHistos*> m_histos ;

  ToolHandle<IGetElementsToBeAligned> m_elementtool ; 
  ToolHandle<Tf::IITHitCreator> m_ithitcreator ;
  ToolHandle<Tf::ITTHitCreator> m_tthitcreator ;
  ToolHandle<Tf::IOTHitCreator> m_othitcreator ;

  std::string m_trackLocation ;
  double m_maxDistance ;
  double m_maxFineDistance ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlRobustAlignAlg )

AlRobustAlignAlg::AlRobustAlignAlg( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_elementtool("GetElementsToBeAligned"),
    m_ithitcreator("Tf::STHitCreator<Tf::IT>/Tf::ITHitCreator"),
    m_tthitcreator("Tf::STHitCreator<Tf::TT>/Tf::TTHitCreator"),
    m_othitcreator("Tf::OTHitCreator"),
    m_maxDistance(50.),
    m_maxFineDistance(10.)
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  );
}

AlRobustAlignAlg::~AlRobustAlignAlg() {}


StatusCode AlRobustAlignAlg::initialize() {
  if ( !GaudiHistoAlg::initialize().isSuccess() ) return StatusCode::FAILURE ;
    
  if(!m_elementtool.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);
  
  if(!m_ithitcreator.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve IT hit creator",StatusCode::FAILURE);
  
  if(!m_tthitcreator.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve TT hit creator",StatusCode::FAILURE);
  
  if(!m_othitcreator.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve OT hit creator",StatusCode::FAILURE);

  m_histos.resize( m_elementtool->elements().size() ) ;

  return StatusCode::SUCCESS;
}

StatusCode AlRobustAlignAlg::finalize() 
{
  m_elementtool.release() ;
  m_ithitcreator.release() ;
  m_tthitcreator.release() ;
  m_othitcreator.release() ;
  return GaudiHistoAlg::finalize();
}

//=============================================================================
// Main execution
//=============================================================================


// template<class HitContainer>
// class DetConvertor
// {
//   const DetectorElement& element (const typename HitContainer::value_type& hit) ;
// } ;

const DetectorElement&
AlRobustAlignAlg::element(const Tf::STHit& hit)
{
  return hit.sector() ;
}

const DetectorElement&
AlRobustAlignAlg::element(const Tf::OTHit& hit)
{
  return hit.module() ;
}

template<class HitContainer>
size_t AlRobustAlignAlg::accumulate( const HitContainer& hits,
				     const LHCb::State& refstate,
				     const LHCb::Track& track)
{
  size_t numcompatible(0) ;   
  Gaudi::XYZPointF trkpoint(refstate.x(),refstate.y(),refstate.z()) ;
  Gaudi::XYZVectorF trkdir(refstate.tx(),refstate.ty(),1) ;
  //Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> trkline( refstate.position(), refstate.slopes() ) ;
  
  for( typename HitContainer::const_iterator ihit = hits.begin() ; ihit != hits.end(); ++ihit) {
    double ytrk = refstate.y() + ( (*ihit)->zMid() - refstate.z() ) * refstate.ty() ;
    if( (*ihit)->isYCompatible(ytrk,5) ) {
      // Juan still has something odd here, so I'll just compute it myself then ...
      //double distance = Gaudi::Math::distance( trkline, **ihit ) ;
      Gaudi::XYZPointF hitpoint = (*ihit)->beginPoint() ;
      Gaudi::XYZVectorF hitdir = (*ihit)->direction() ;
      Gaudi::XYZVectorF projectiondir = hitdir.Cross( trkdir ).Unit() ;
      Gaudi::XYZVectorF delta = hitpoint-trkpoint ;
      double distance = delta.Dot(projectiondir) ;
      if ( std::abs(distance) < m_maxDistance ) {
	ElementHistos* histos = findElement(element(**ihit)) ;
	if (histos) {
	  int sign = (*ihit)->yBegin() < (*ihit)->yEnd() ? 1 : -1 ;
	  histos->resh1->fill( sign * distance ) ;
	  histos->resfineh1->fill( sign * distance ) ;
	  if(track.isOnTrack( (**ihit).lhcbID() ) ) {
	    histos->hotresh1->fill( sign * distance ) ;
	    histos->hotresfineh1->fill( sign * distance ) ;
	  }
	}
      }
      ++numcompatible ; 
    }
  }
  return numcompatible ;
}

StatusCode AlRobustAlignAlg::execute() 
{
  // Get tracks.
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackLocation);

  // Get the TT hits
  Tf::STHitRange tthits = m_tthitcreator->hits() ;
  Tf::STHitRange ithits = m_ithitcreator->hits() ;
  Tf::OTHitRange othits = m_othitcreator->hits() ;
  
  debug() << "Number of hits in TT, IT: "
	  << tthits.size() << " " << ithits.size() << endreq ;

  // now loop over all tracks, create a line from the last state, loop
  // over all hits, etc
  BOOST_FOREACH( const LHCb::Track* track, tracks) {
    if( track->nStates() > 0 ) {
      //
      const LHCb::State* laststate = track->states().back() ;
      // create a 'line' object
      // now do the combinatorics
      size_t numcompatibleTT = accumulate( tthits, *laststate,*track ) ;
      size_t numcompatibleIT = accumulate( ithits, *laststate,*track ) ;
      size_t numcompatibleOT = accumulate( othits, *laststate,*track ) ;
      debug() << "Number of compatible hits TT, IT: " << numcompatibleTT << " " << numcompatibleIT << " "
	      << numcompatibleOT << endreq ;
    } else {
      warning() << "Track has no states" << endreq ;
    }
  }
  
  return StatusCode::SUCCESS ;
}

ElementHistos*
AlRobustAlignAlg::findElement(const DetectorElement& elem)
{
  ElementHistos* rc(0) ;
  const AlignmentElement* alignelem = m_elementtool->findElement( elem ) ;
  if( alignelem ) {
    rc = m_histos[ alignelem->index() ] ;
    if ( rc==0 ) {
      rc = new ElementHistos() ;
      m_histos[ alignelem->index() ] = rc ;
      //std::string dirname =  "element" + boost::lexical_cast<std::string>( alignelem->index() ) + "/"; //name + "/" ;
      std::string dirname =  alignelem->name() + "/" ;
      rc->resh1 = book1D(dirname + "resh1", 
			 "Residual for " + alignelem->name(),-m_maxDistance,m_maxDistance, 100);
      rc->resfineh1 = book1D(dirname + "resfineh1", 
			     "Residual for " + alignelem->name(),-m_maxFineDistance,m_maxFineDistance, 100);
      rc->hotresh1 = book1D(dirname + "hotresh1", 
			    "Residual for " + alignelem->name(),-m_maxDistance,m_maxDistance, 100);
      rc->hotresfineh1 = book1D(dirname + "hotresfineh1", 
			     "Residual for " + alignelem->name(),-m_maxFineDistance,m_maxFineDistance, 100);
    }
  }
  return rc ;
}


