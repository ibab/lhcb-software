#include "OTDet/DeOTDetector.h"
#include "Event/Node.h"
//#include "TfKernel/OTHit.h"
//#include "LHCbMath/GeomFun.h"
#include "StrawHit.h"
#include "Event/OTMeasurement.h"
#include "Event/FitNode.h"
#include "TrackInterfaces/ITrackProjector.h"


namespace OTCal
{    

  unsigned int StrawHit::uniqueLayer() const {
    return (m_channelid.station() - 1) * 4 + m_channelid.layer() ;
  }

  unsigned int StrawHit::uniqueQuarter() const {
    return uniqueLayer() * 4 + m_channelid.quarter() ;
  }

  unsigned int StrawHit::uniqueModule() const {
    return uniqueQuarter()*9 + (m_channelid.module()-1) ;
  }

  unsigned int StrawHit::uniqueOtis() const {
    return uniqueModule()*4 + (m_channelid.straw()-1)/32 ;
  }

  inline double sqr(double x) { return x*x ; }
  
  StrawHit::StrawHit(const ITrackProjector& projector, const LHCb::Node& node ) 
    : 
    m_isOutlier(node.type()==LHCb::Node::Outlier),
    m_trkdistance(0),
    m_trkcov(0),
    m_residual(0),
    m_residualcov(0),
    m_hitcov(0),
    m_trkDriftTime(0),
    m_driftTime(0),
    m_driftRadius(0),
    m_t0estimate(0)
  {
    // Set some things to identify the straw
    const LHCb::OTMeasurement* otmeas = dynamic_cast<const LHCb::OTMeasurement*>(&(node.measurement())) ;
    m_channelid = otmeas->channel() ;
    m_module = &(otmeas->module()) ;
    const OTDet::RtRelation& rtrelation = m_module->rtRelation() ;

    // currently, the only fool proof way to obtain an unbiased
    // distance is to unbias the state and call the projector.
    const LHCb::FitNode* fitnode = dynamic_cast<const LHCb::FitNode*>(&node) ;
    LHCb::State ubstate ;
    if( fitnode->type()==LHCb::Node::HitOnTrack ) {
      ubstate = fitnode->unbiasedState() ;
      // project it:
      LHCb::FitNode unbiasednode(*fitnode) ;
      unbiasednode.setState( ubstate ) ;
      unbiasednode.setRefVector( ubstate.stateVector() ) ;
      projector.projectReference(unbiasednode) ;
      // extract the distance
      m_trkdistance = unbiasednode.pocaVector().Dot( ubstate.position() - fitnode->measurement().trajectory().beginPoint() ) ;

      // for the errors we just take the original node, which was biased (!)
      m_residual    = fitnode->unbiasedResidual() ; // note: could be [ns!]
      m_residualcov = sqr(fitnode->errUnbiasedResidual()) ;
    } else {
      // this was an outlier, hence already unbiased
      ubstate = fitnode->state() ;
      m_trkdistance = fitnode->pocaVector().Dot( ubstate.position() - fitnode->measurement().trajectory().beginPoint() ) ;
      m_residual    = fitnode->residual() ;
      m_residualcov = sqr(fitnode->errResidual()) ;
    }
    
    m_hitcov      = fitnode->errMeasure2() ;
    m_trkcov      = m_residualcov - m_hitcov; // note: this could be [ns]!
    m_trkDriftTime = rtrelation.drifttime( std::abs( m_trkdistance ) ) ;
    if( m_trkcov < 0 ) {
      std::cout << "PROBLEM: " << isOutlier() << " " << m_residualcov << " " << m_hitcov << std::endl ;
    }
    
    // now get the unbiased time
    m_driftTime = otmeas->driftTimeFromY(ubstate.y()) ;
    m_driftRadius = otmeas->driftRadiusWithErrorFromY(ubstate.y()).val ;
    
    // the t0 estimate is the current t0 plus the drifttime residual
    double t0 = otmeas->module().strawT0(  straw() ) ;
    m_t0estimate = t0 + m_driftTime - m_trkDriftTime ;
  }

//   StrawHit::StrawHit(const DeOTDetector& otdet, const LHCb::Node& node )
//   { 
//     const LHCb::OTMeasurement* otmeas = dynamic_cast<const LHCb::OTMeasurement*>(&(node.measurement())) ;
//     m_channelid = otmeas->channel() ;
//     m_module = &(otmeas->module()) ;

//     // now we need all the calibration stuff. the new Tf code is best
//     // for that, I think. Let's recreate the hit:
    
//     // There is a problem here: The residual is
//     //
//     //   res =  lr-sign * measured-radius  -  fitted-distance
//     //
//     // For the calibration we need the biased or unbiased
//     // fitted-distance.  However, the node does not contain it. The
//     // node does also not contain the y-corrected
//     // measured-distance. So, we don't have a clue what the right
//     // distance is. 

//     // How do we solve this? The best thing to do is to extend the
//     // node class. The next best thing to do is to entirely recompute
//     // the residual. Because we still need to be able to unbias, we'll
//     // will still use the residual-covariance from the node.

//     // Compute distance between straw wire and track. We could use
//     // trkpoca but I find that way to expensive. So, we just assume
//     // straight lines.

//     //     Tf::OTHit        othit( *m_module, otmeas->ottime()  ) ;
//     //     const LHCb::State& trkstate = node.state() ;
//     //     Gaudi::XYZPointF  trkpos(trkstate.x(), trkstate.y(), trkstate.z()) ;
//     //     Gaudi::XYZVectorF trkdir(trkstate.tx(), trkstate.ty(), 1) ;
//     //     Gaudi::XYZVectorF projvector = trkdir.Cross( othit.direction() ).Unit() ;
//     //     double distance       = (othit.position(0) - trkpos).Dot( projvector ) ;
    
//     //     // To get the reference y-position we need to know the coordinate
//     //     // along the hit trajectory. This is the expensive solution:
//     //     //Gaudi::XYZLineF   trkline(trkpos,trkdir) ;
//     //     //double mu0(0), mu1(0) ;
//     //     //Gaudi::Math::closestPointParams(trkline, othit, mu0, mu1) ;
//     //     double yref(0) ;
//     //     {
//     //       const Gaudi::XYZPointF&   p0 = trkpos ;
//     //       const Gaudi::XYZVectorF&  v0 = trkdir ;
//     //       const Gaudi::XYZPointF&   p1 = othit.beginPoint();
//     //       const Gaudi::XYZVectorF&  v1 = othit.direction();
    
//     //       const double v0Dotv1 = v0.Dot(v1);
//     //       const double v0mag2  = v0.Mag2();
//     //       const double v1mag2  = v1.Mag2();
//     //       const double dnom = gsl_pow_2(v0Dotv1) -  (v0mag2*v1mag2);
//     //       const Gaudi::XYZVectorF p0_p1 = (p0 - p1);
//     //       //mu0 = ( v1mag2*p0_p1.Dot(v0)      - v0Dotv1*( p0_p1.Dot(v1) ) ) / dnom;
//     //       double mu1 = ( v0Dotv1*( p0_p1.Dot(v0) ) -   v0mag2*p0_p1.Dot(v1)  )   / dnom;
//     //       yref = othit.position(mu1).y() ;
//     //       //std::cout << "yref: " << yref << " " << trkstate.y() << std::endl ;
//     //     }
    
//     // this drifttime does not have the tof correction, so we should not use it ...
//     const OTDet::RtRelation& rtrelation = m_module->rtRelation() ;

//     double yref = node.state().y() ;
//     double biasedresidual = node.residual() ;
//     m_driftTime   = otmeas->calibratedTime() - otmeas->deltaTimeOfFlight() - otmeas->propagationTimeFromY(yref) ; 
//     m_driftRadius = otmeas->driftRadiusWithErrorFromY(yref).val ;
//     int lrsign = otmeas->ambiguity() ;
//     //double distance = - biasedresidual + lrsign * m_driftRadius ;
    
//     // now calculate the residual
//     //int lrsign   = distance > 0 ? 1 : -1 ;
//     //double biasedresidual = lrsign * m_driftRadius - distance ;
    
//     // Compare to what we had in the node:
//     //std::cout << "StrawHit: " << lrsign << " " << otmeas->ambiguity() << " " 
//     //<< biasedresidual << " " << node.residual() << std::endl ;
//     // FIXME: This test actually fails ... We need to find out why!

//     // This we'll need in any case
//     //double biasedresidual    = node.residual() ;
//     m_hitdistance = lrsign * m_driftRadius ;
//     m_hitcov      = node.errMeasure2()    ;

//     double biasedresidualcov   =  node.errResidual2 () ;
//     double unbiasedresidual    = biasedresidual * m_hitcov / biasedresidualcov ;
//     double unbiasedresidualcov = m_hitcov * m_hitcov / biasedresidualcov ;

//     // this comes from the node
//     m_trkdistance = m_hitdistance - unbiasedresidual ;
//     m_trkcov      = unbiasedresidualcov - m_hitcov ;
//     // it is possible that after 'debiasing' we need to change the lrsign
//     if( m_trkdistance * m_hitdistance < 0 ) m_hitdistance *= -1 ;
    
//     //std::cout << "StrawHit: " << m_hitdistance << " " << m_trkdistance << std::endl ;
    
//     // How do we now know that this distanc eis correct? At least,
//     // we'll once need to take the difference between state and straw.

//     m_trkDriftTime = rtrelation.drifttime( std::abs( m_trkdistance ) ) ;

//     double rawtime = otmeas->calibratedTime() + otmeas->module().strawT0(  m_straw ) ;
//     m_t0estimate = rawtime -  otmeas->deltaTimeOfFlight() - otmeas->propagationTimeFromY(yref) - m_trkDriftTime ;
    
//     //std::cout << "drifttime: " << m_trkDriftTime << std::endl ;
//     //std::cout << m_driftTime << " " << rtrelation.drifttime(m_driftRadius) << " " << std::endl ;
//     //std::cout << m_driftRadius << " " << rtrelation.radius( m_driftTime ) << std::endl ;


//   }
}
