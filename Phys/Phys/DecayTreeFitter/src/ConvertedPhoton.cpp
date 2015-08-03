#include "ConvertedPhoton.h"
#include "RecoTrack.h"
#include "FitParams.h"
#include "Event/Track.h"
#include "Event/StateParameters.h"
#include "TrackKernel/TrackTraj.h"

namespace DecayTreeFitter
{
  extern int vtxverbose ;

  namespace {
    
    double zFirstHit(const LHCb::Track& track)
    {
      double z = 0 ;
      const LHCb::State* s = track.stateAt( LHCb::State::FirstMeasurement ) ;
      if(s) 
	z = s->z() ;
      else if( track.type() == LHCb::Track::Downstream) 
	z = StateParameters::ZMidT ;
      else
	z = StateParameters::ZEndVelo ;
      return z ;
    }

    // bool computeZ( const LHCb::Track& track,
    // 		   double& zmin, double& zmax )
    // {
    //   // return true if succesful
    //   bool rc = false ;
    //   zmin = zmax = 0 ;
    //   const LHCb::State* s1 = track.stateAt( LHCb::State::ClosestToBeam ) ;
    //   const LHCb::State* s2 = track.stateAt( LHCb::State::FirstMeasurement ) ;
    //   if( s1 && s2 ) {
    // 	// probably a 'Long' track
    // 	zmin = s1->z() ;
    // 	zmax = s2->z() ;
    // 	if(zmin > zmax) zmin = zmax ;
    // 	rc = true ;
    //   } else if( s2 ) {
    // 	// probably a 'Downstream' track
    // 	zmin = 0 ;
    // 	zmax =  s2->z() ; s2->z() ;
    // 	rc = true ;
    //   }
    //   return rc ;
    // }
  }
 

  ConvertedPhoton::ConvertedPhoton(const LHCb::Particle& bc, const ParticleBase* mother, 
				   const Configuration& config) 
    : InternalParticle(bc,mother,config),
      m_conversionZ(0),
      m_conversionZCov(-1)
  {
    // we should really do this after we have done initialization. it
    // wouldn't be nice to set the constraint such that z is not smaller
    // than that of the mother vertex. alternative: only set 'zmax'
    // and use the mother vertex as the other part of the
    // constraint. that just makes it a bit less linear.

    // there should be exactly two daughter tracks.
    // double zmin(999999), zmax(999999) ;
    // for(const auto& daughter : daughters() ) {
    //   const RecoTrack* tr = dynamic_cast<const RecoTrack*>(daughter) ;
    //   if( tr ) {
    // 	double azmin,azmax ;
    // 	bool success = computeZ( tr->track(),azmin,azmax) ;
    // 	if(success) {
    // 	  zmin = std::min(zmin,azmin) ;
    // 	  zmax = std::min(zmax,azmax) ;
    // 	}
    //   }
    // }
    // //now do something
    // m_conversionZ = (zmin + zmax)/2 ;
    // m_conversionZCov = std::max( (zmax-zmin)*(zmax-zmin)/12.0, 1.0e4) ;
  }

  void
  ConvertedPhoton::updateCache(const FitParams& pars)
  {

    // we'll do this only once
    // get the position of the first hit
    double zfirsthit(99999) ;
    for(const auto& daughter : daughters() ) {
      const RecoTrack* tr = dynamic_cast<const RecoTrack*>(daughter) ;
      if( tr ) 
	zfirsthit = std::min(zfirsthit,zFirstHit( tr->track() ) ) ;
    }
    // get the position of the mother
    double zmother = -300 ; // begin velo?
    if( mother() ) zmother = pars.par()( mother()->posIndex()+3 ) ;
    m_conversionZ = 0.5*(zmother+zfirsthit) ;
    const double dz = zfirsthit - zmother ;
    m_conversionZCov = std::max(dz*dz/12.0, 1.0e2) ;

    // alternative: assume somebody has already determined the poca to
    // a reasonable level. now use the track curvature to constrain to
    // that point
    // m_conversionZ = std::min(pars.par()( posIndex()+3 ), zfirsthit) ;
    // double totalcurv2 = 0 ; // will have units of 1/mm^2
    // for(const auto& daughter : daughters() ) {
    //   const RecoTrack* tr = dynamic_cast<const RecoTrack*>(daughter) ;
    //   if( tr ) {
    // 	zfirsthit = std::min(zfirsthit,zFirstHit( tr->track() ) ) ;
    // 	std::cout << "RecoPhoton: " << tr->tracktraj() << std::endl ;
    // 	LHCb::TrackTraj::Vector curv = tr->tracktraj()->curvature( m_conversionZ ) ;
    // 	totalcurv2 += curv.x()*curv.x() + curv.y() * curv.y() ;
    //   }
    // }
    // m_conversionZCov = std::min(totalcurv2 > 0 ? 1/totalcurv2 : 1e04, 1e04) ;
    if(vtxverbose>=1) 
      std::cout << "DTF::ConvertedPhoton::updateCache: " 
		<< zfirsthit << " " << zmother << " "
		<< m_conversionZ << " +/- " 
		<< std::sqrt( m_conversionZCov ) << std::endl;
  }

  ErrCode
  ConvertedPhoton::initCov( FitParams* fitpars) const
  {
    ErrCode status = ParticleBase::initCov( fitpars ) ;
    // do the daughters first
    // ErrCode status ;
    // int momindex = posIndex() ;
    // for(daucontainer::const_iterator it = m_daughters.begin() ;
    //     it != m_daughters.end() ; ++it) {
    //   status |= (*it)->initCov(fitparams) ;
    //   // for the momenta, add up the daughters 
    //   int daumomindex = (*it)->momIndex() ;
    //   for(unsigned int i=1; i<=3; ++i)
    // 	fitpars.cov().fast(momindex+i,momindex+i) += fitpars.cov().fast((*it)->

    // then, for the position, do this:
    int posindex = posIndex() ;
    if( m_conversionZCov<0 )
      (const_cast<ConvertedPhoton*>(this))->updateCache(*fitpars) ;
    fitpars->cov().fast(posindex+1,posindex+1) = 1 * m_conversionZCov ;
    fitpars->cov().fast(posindex+2,posindex+2) = 1 * m_conversionZCov ;
    fitpars->cov().fast(posindex+3,posindex+3) = 100 * m_conversionZCov ;
    int lenindex = lenIndex() ;
    if( lenindex >=0 )
      fitpars->cov().fast(lenindex+1,lenindex+1) = fitpars->cov().fast(posindex+3,posindex+3) ;
    return status ;
  }

  ErrCode
  ConvertedPhoton::projectConversionMassConstraint(const FitParams& fitparams,
					       Projection& p) const
  {
    // only works if there are two daughters. constraint those to be parallel:
    // p1.in(p2) - |p1||p2|=0
    const ParticleBase* dauA = daughters()[0] ;
    const ParticleBase* dauB = daughters()[1] ;
    int daumomindexA = dauA->momIndex() ;
    int daumomindexB = dauB->momIndex() ;

    // first calculate the total momenta
    double momA2(0),momB2(0) ;
    for(int irow=1; irow<=3; ++irow) {
      double pxA =  fitparams.par(daumomindexA+irow) ;
      momA2 += pxA*pxA ;
      double pxB =  fitparams.par(daumomindexB+irow) ;
      momB2 += pxB*pxB ;
    }
    double momA(sqrt(momA2)), momB(sqrt(momB2)) ;

    // now fill r and H
    p.r(1) = -momA*momB ;
    for(int irow=1; irow<=3; ++irow) {
      double pxA =  fitparams.par(daumomindexA+irow) ;
      double pxB =  fitparams.par(daumomindexB+irow) ;
      p.r(1) += pxA*pxB ;
      p.H(1,daumomindexA+irow) = pxB - pxA/momA * momB ;
      p.H(1,daumomindexB+irow) = pxA - pxB/momB * momA ;
    }

    return ErrCode::success ;
  }
  
  ErrCode
  ConvertedPhoton::projectConversionPositionConstraint(const FitParams& fitparams,
						       Projection& p) const
  {
    if( m_conversionZCov<0 )
      (const_cast<ConvertedPhoton*>(this))->updateCache(fitparams) ;
    
    int posindex = posIndex() ;
    p.r(1) = fitparams.par()(posindex+3) - m_conversionZ ;
    p.Vfast(1,1) = m_conversionZCov ;
    p.H(1,posindex+3) = 1 ;
    return ErrCode::success ;
  }

  ErrCode
  ConvertedPhoton::projectConstraint(Constraint::Type type,
				     const FitParams& fitparams,
				     Projection& p) const
  {
    ErrCode status ;
    switch(type) {
    case Constraint::convertedphotonposition:
      status |= projectConversionPositionConstraint(fitparams,p) ;
      break ;
    case Constraint::mass:
      status |= projectConversionMassConstraint(fitparams,p) ;
      break ;
    default:
      status |= InternalParticle::projectConstraint(type,fitparams,p) ;
    }
    return status ;
  }

  void ConvertedPhoton::addToConstraintList(constraintlist& alist,
					    int depth) const
  {
    InternalParticle::addToConstraintList(alist,depth) ;
    alist.push_back(Constraint(this,Constraint::convertedphotonposition,depth,1));
  }
}
