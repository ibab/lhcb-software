#include <iomanip>
#include <algorithm>
#include <boost/foreach.hpp>

#include "Event/Particle.h"
#include "InternalParticle.h"
#include "MissingParticle.h"
#include "FitParams.h"
#include "RecoTrack.h"

#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"

namespace DecayTreeFitter
{

  extern int vtxverbose ;

  inline bool
  sortByType(const ParticleBase* lhs, const ParticleBase* rhs)
  {
    int lhstype = lhs->type() ;
    int rhstype = rhs->type() ;
    bool rc = false ;
    if( lhstype == rhstype  &&
        lhstype == ParticleBase::kRecoTrack )
      rc =  lhs->particle().pt() > rhs->particle().pt() ;
    else if( lhs->particle().daughters().size()>0 &&
             rhs->particle().daughters().size()>0 )
      rc = lhs->nFinalChargedCandidates() >
        rhs->nFinalChargedCandidates() ;
    else
      rc = lhstype < rhstype ;
    return rc ;
  }

  InternalParticle::InternalParticle(const LHCb::Particle& bc, const ParticleBase* mother,
                                     const Configuration& config)
    : ParticleBase(bc,mother),m_lifetimeconstraint(false)
  {
    BOOST_FOREACH( const LHCb::Particle* daughter, bc.daughters() )
      addDaughter(*daughter,config) ;
    // copy constraints
    m_lifetimeconstraint = false ; //bc && bc->constraint(BtaConstraint::Life) ;
    m_isconversion = daughters().size()==2 &&  bc.particleID().pid() == 22 ;
  }

  bool compTrkTransverseMomentum(const RecoTrack* lhs, const RecoTrack* rhs)
  {
    return lhs->particle().pt() > rhs->particle().pt() ;
  }

  ErrCode
  InternalParticle::initPar1(FitParams* fitparams)
  {
    // This is the most complicated part of the vertexer: an
    // initialization that always works.

    // There are two ways out: If the LHCb::Particle was vertexed
    // before, we can rely on the existing vertex (case A). If not, we
    // need to estimate the vertex position from the daughters; that
    // is very complicated (case B). The momentum is always
    // initialized from the sum of the daughter four-vectors. In the
    // end, it doesn't really matter.

    // FIX ME: Currently, this scheme does not work for B->K*D0, with
    // D0->pi0Ks, because the D0 is initialized before there is a B
    // vertex.

    if(vtxverbose>=3)
      std::cout << "InternalParticle::initPar: "
                << particle().particleID().pid() << " " << daughters().size() << " "
                << hasPosition() << " " << posIndex() << std::endl ;

    ErrCode status ;
    int posindex = posIndex() ;

    // logic check: we do not want to call this routine for resonances.
    assert( hasPosition() ) ;

    // Start with origin
    for(int row=1; row<=3; ++row) fitparams->par()(row+posindex) = 0 ;

    // Step 1: pre-initialization of all daughters
    for(daucontainer::const_iterator it = begin() ; it != end() ; ++it)
      status |= (*it)->initPar1(fitparams) ;

    // Step 2: initialize the vertex. if we are lucky, we had a
    // 'btaresonant daughter, and we are already done.
    if( fitparams->par()(posindex+1)==0 && fitparams->par()(posindex+2)==0 &&
        fitparams->par()(posindex+3)==0 ) {

      const LHCb::Vertex* vtx = particle().endVertex() ;
      if( vtx 
	  && vtx->position() != Gaudi::XYZPoint(0,0,0)
	  // && vtx->technique() ==
	  ) {
        // we found an existing valid vertex. that's fine as well ...
        Gaudi::XYZPoint point = vtx->position() ;
        fitparams->par(posindex+1) = point.x() ;
        fitparams->par(posindex+2) = point.y() ;
        fitparams->par(posindex+3) = point.z() ;
        if(vtxverbose>=2)
          std::cout << "using existing vertex: " << point << std::endl ;

      } else {
        // Case B: the hard way ... use the daughters to estimate the
        // vertex. First we check if there are sufficient tracks
        // attached to this vertex. If so, estimate the poca of the
        // two tracks with the highest momentum. This will work for
        // the majority of the cases. If there are not sufficient
        // tracks, add the composites and take the two with the best
        // doca.

        // create a vector with all daughters that constitute a
        // 'trajectory' (ie tracks, composites and daughters of
        // resonances.)
        daucontainer alldaughters ;
        collectVertexDaughters( alldaughters, posindex ) ;
	if( vtxverbose>=2) {
	  std::cout << "number of daughters for initializing vertex: "
		    << name() << " " << alldaughters.size() << std::endl ;
	  if(vtx) std::cout << "Original vertex pos: " << vtx->position() << std::endl ;
	}
	
        // select daughters that are either charged, or have an initialized vertex
        daucontainer vtxdaughters ;
        std::vector<RecoTrack*> trkdaughters ;
        for(daucontainer::const_iterator it = alldaughters.begin() ;
            it != alldaughters.end() ; ++it) {
          if( (*it)->type()==ParticleBase::kRecoTrack ) {
            trkdaughters.push_back( static_cast<RecoTrack*>(*it) )  ;
          } else if( (*it)->hasPosition() && fitparams->par((*it)->posIndex()+1)!=0 ) {
            vtxdaughters.push_back( *it ) ;
          }
        }

        if( trkdaughters.size() >=2 ) {
          // sort in pT. not very efficient, but it works.
          if( trkdaughters.size()>2 )
            std::sort(trkdaughters.begin(),trkdaughters.end(),compTrkTransverseMomentum) ;
          // now, just take the first two ...
          RecoTrack* dau1 = trkdaughters[0] ;
          RecoTrack* dau2 = trkdaughters[1] ;

          // get the poca of the two statevectors
          const LHCb::State& state1 = dau1->state() ;
          const LHCb::State& state2 = dau2->state() ;
          Gaudi::Math::Line<Gaudi::XYZPoint, Gaudi::XYZVector> line1(state1.position(),
                                                                     state1.slopes()) ;
          Gaudi::Math::Line<Gaudi::XYZPoint, Gaudi::XYZVector> line2(state2.position(),
                                                                     state2.slopes()) ;
          double mu1(0),mu2(0) ;
          Gaudi::Math::closestPointParams(line1,line2,mu1,mu2) ;
          Gaudi::XYZPoint p1 = line1.position(mu1) ;
          Gaudi::XYZPoint p2 = line2.position(mu2) ;
          fitparams->par()(posindex+1) = 0.5*(p1.x()+p2.x()) ;
          fitparams->par()(posindex+2) = 0.5*(p1.y()+p2.y()) ;
          fitparams->par()(posindex+3) = 0.5*(p1.z()+p2.z()) ;
          dau1->setFlightLength( mu1 ) ;
          dau2->setFlightLength( mu2 ) ;

        } else if(trkdaughters.size()+vtxdaughters.size()>=2)  {
          std::cout << "VtkInternalParticle: To be implemented!!"<< std::endl ;
          /*

          // that's unfortunate: no enough charged tracks from this
          // vertex. need all daughters. create trajectories and use
          // normal TrkPoca.

          std::vector<LHCb::StateZTraj> trajectories ;
          for(vector<RecoTrack*>::const_iterator it = trkdaughters.begin() ;
          it != trkdaughters.end() ; ++it)
          trajectories.push_back( (*it)->traj() ) ;

          trajectories.push_back(&((*it)->particle().trkAbsFit()->traj())) ;

          std::vector<TrkLineTraj> linetrajectories ; // store trajectories of composites
          linetrajectories.reserve(  vtxdaughters.size() ) ;
          for(daucontainer::const_iterator it = vtxdaughters.begin() ;
          it != vtxdaughters.end() ; ++it) {
          //std::cout << (*it)->particle().pdtEntry()->name() << std::endl ;
          int dauposindex = (*it)->posIndex() ;
          int daumomindex = (*it)->momIndex() ;
          Gaudi::XYZPoint point(fitparams->par()(dauposindex+1),
          fitparams->par()(dauposindex+2),
          fitparams->par()(dauposindex+3)) ;
          Hep3Vector direction(fitparams->par()(daumomindex+1),
          fitparams->par()(daumomindex+2),
          fitparams->par()(daumomindex+3)) ;
          linetrajectories.push_back(TrkLineTraj(point,direction,1) ) ;
          trajectories.push_back(&(linetrajectories.back())) ;
          //daupoint = point ;
          }

          // we select the two trajectories with the best poca
          double docabest(99999);
          TrkErrCode pocastatus ;
          for( std::vector<const Trajectory*>::iterator it1 = trajectories.begin() ;
          it1 != trajectories.end(); ++it1 )
          for( std::vector<const Trajectory*>::iterator it2 = trajectories.begin() ;
          it2 != it1; ++it2 ) {
          TrkPoca poca(**it1,0.,**it2, 0.);
          Hep3Vector dir1 = (*it1)->direction(poca.flt1());
          Hep3Vector dir2 = (*it2)->direction(poca.flt2());
          double doca = poca.doca() ;
          if(fabs(doca)<fabs(docabest)) {
          Gaudi::XYZPoint pnt1 = (*it1)->position(poca.flt1());
          Gaudi::XYZPoint pnt2 = (*it2)->position(poca.flt2());
          fitparams->par()(posindex+1) = 0.5*(pnt1.x()+pnt2.x()) ;
          fitparams->par()(posindex+2) = 0.5*(pnt1.y()+pnt2.y()) ;
          fitparams->par()(posindex+3) = 0.5*(pnt1.z()+pnt2.z()) ;
          docabest = doca ;
          pocastatus = poca.status() ;
          }
          }
          */
        } else if( mother() && mother()->posIndex()>=0 ) {

          // let's hope the mother was initialized
          int posindexmother = mother()->posIndex() ;
          for(int ipos=1; ipos<=3; ++ipos) {
            fitparams->par()(posindex+ipos) =
              fitparams->par()(posindexmother+ipos) ;
          }
        } else {
          // something is wrong!
          std::cout << "There are not sufficient geometric constraints to fit "
                    << "this decay tree. Perhaps you should add a beam constraint. "
                    << std::endl ;
          //<< particle().constraint(BtaConstraint::Beam)
          //   << std::endl
          //   << treeprinter.print(*bc()) << endmsg ;
          status |= ErrCode::badsetup ;
        }
      }
    }

    // step 3: do the post initialization step of all daughters
    for(daucontainer::const_iterator it = daughters().begin() ;
        it != daughters().end() ; ++it)
      (*it)->initPar2(fitparams) ;

    // step 4: initialize the momentum by adding up the daughter 4-vectors
    initMom(fitparams) ;

    if(vtxverbose>=3)
      std::cout << "End of initpar: "
                << name() << " ("
                << fitparams->par()(posindex+1) << ","
                << fitparams->par()(posindex+2) << ","
                << fitparams->par()(posindex+3) << ")" << std::endl ;

    return status ;
  }

  ErrCode
  InternalParticle::initPar2(FitParams* fitparams)
  {
    // FIX ME: in the unfortunate case (the B-->D0K*- above) that our
    // vertex is still the origin, we copy the mother vertex.
    int posindex = posIndex() ;
    if( hasPosition() && mother() && fitparams->par(posindex+1)==0 &&
        fitparams->par(posindex+2)==0 && fitparams->par(posindex+3)==0 ) {
      int posindexmom = mother()->posIndex() ;
      for(int irow=1; irow<=3; ++irow)
        fitparams->par(posindex+irow) = fitparams->par(posindexmom+irow) ;
    }
    // step 5: initialize the lifetime
    return initTau(fitparams) ;
  }

  ErrCode
  InternalParticle::initMom( FitParams* fitparams ) const
  {
    int momindex = momIndex() ;
    // reset
    for( int irow=1; irow<=4; ++irow)
      fitparams->par(momindex+irow) = 0 ;

    // now add daughter momenta
    for(daucontainer::const_iterator it = begin() ; it != end() ; ++it) {
      int daumomindex = (*it)->momIndex() ;
      double e2(0) ;
      //int maxrow = (*it)->hasEnergy() && !(*it)->hasMassConstraint() ? 4 : 3 ; 
      int maxrow = (*it)->hasEnergy() ? 4 : 3 ;
      for(int irow=1; irow<=maxrow; ++irow) {
        double px = fitparams->par()(daumomindex+irow) ;
        e2 += px*px ;
        fitparams->par(momindex+irow) += px ;
      }
      if(maxrow==3) {
        double mass = (*it)->pdtMass() ;
        fitparams->par(momindex+4) += std::sqrt(e2+mass*mass) ;
      }
    }

    // if there is a mass constraint, ignore what we have just
    // computed for the energy, and just insert the mass
    if( hasMassConstraint()  ) {
      double mass = pdtMass() ;
      double p2 = 0 ;
      for( int irow=1; irow<=3; ++irow) {
	double px = fitparams->par(momindex+irow) ;
	p2 += px*px ;
      }
      fitparams->par(momindex+4) = std::sqrt(p2+mass*mass) ;
    }

    return ErrCode::success ;
  }

  ErrCode
  InternalParticle::projectKineConstraint(const FitParams& fitparams,
                                          Projection& p) const
  {
    // these are in fact four independent constraints. i'll filter
    // them as one, making the code simpler at the expense of a bit of
    // CPU.

    // first add the mother
    int momindex = momIndex() ;
    for(int imom=1; imom<=4; ++imom) {
      p.r(imom)               = fitparams.par()(momindex+imom) ;
      p.H(imom,momindex+imom) = 1 ;
    }

    // now add the daughters
    for(daucontainer::const_iterator it = daughters().begin() ;
        it != daughters().end() ; ++it) {
      int daulenindex = (*it)->lenIndex() ;
      int daumomindex = (*it)->momIndex() ;
      double mass = (*it)->pdtMass() ;
      double e2=mass*mass ;
      int maxrow = (*it)->hasEnergy() ? 4 : 3 ;
      for(int imom=1; imom<=maxrow; ++imom) {
        double px = fitparams.par()(daumomindex+imom) ;
        e2 += px*px ;
        p.r(imom) += -px ;
        p.H(imom,daumomindex+imom) = -1 ;
      }

      if(maxrow==3) {
        // treat the energy for particles that are parameterized with p3
        double energy = sqrt(e2) ;
        p.r(4) += -energy ;
        for(int jmom=1; jmom<=3; ++jmom) {
          double px = fitparams.par()(daumomindex+jmom) ;
          p.H(4,daumomindex+jmom) = -px/energy ;
        }
      } else if(false && daulenindex>=0 && (*it)->charge()!=0) {

        double tau =  fitparams.par()(daulenindex+1) ;
        double lambda = bFieldOverC() * (*it)->charge() ;
        double px0 = fitparams.par()(daumomindex+1) ;
        double py0 = fitparams.par()(daumomindex+2) ;
        double pt0 = sqrt(px0*px0+py0*py0) ;
        const double posprecision = 1e-4 ; // 1mu
        if( fabs(pt0*lambda*tau*tau) > posprecision ) {
          double sinlt = sin(lambda*tau) ;
          double coslt = cos(lambda*tau) ;
          double px = px0*coslt - py0*sinlt ;
          double py = py0*coslt + px0*sinlt ;
          p.r(1) += px0 - px ;
          p.r(2) += py0 - py ;
          p.H(1,daumomindex+1) +=  1 - coslt ;
          p.H(1,daumomindex+2) +=      sinlt ;
          p.H(1,daulenindex+1) +=  lambda*py ;
          p.H(2,daumomindex+1) +=     -sinlt ;
          p.H(2,daumomindex+2) +=  1 - coslt ;
          p.H(2,daulenindex+1) += -lambda*px ;
        }
      }
    }
    return ErrCode::success ;
  }

  ErrCode
  InternalParticle::projectLifeTimeConstraint(const FitParams&,
                                              Projection&) const
  {
    std::cout << "Not yet implemented lifetime constraint!" << std::endl ;
    // int lenindex = lenIndex() ;
    //     assert(lenindex>=0) ;
    //     double tau = pdtTau() ;
    //     p.r(1)            = fitparams.par()(lenindex+1) - tau ;
    //     p.Vfast(1,1)      = tau*tau ;
    //     p.H(1,lenindex+1) = 1 ;
    return ErrCode::success ;
  }

  ErrCode
  InternalParticle::projectConversionConstraint(const FitParams& fitparams,
                                                Projection& p) const
  {
    // only works if there are two daughters. constraint those to be parallel:
    // p1.in(p2) - |p1||p2|=0
    assert(m_isconversion) ;
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
  InternalParticle::projectConstraint(Constraint::Type type,
                                      const FitParams& fitparams,
                                      Projection& p) const
  {
    ErrCode status ;
    switch(type) {
    case Constraint::mass:
    case Constraint::massEnergy:
      //       if( m_daughters.size()==2 &&
      //    !m_daughters.front()->hasEnergy() &&
      //    !m_daughters.back()->hasEnergy() )
      //  status |= projectMassConstraintTwoBody(fitparams,p) ;
      ///      else
      status |= projectMassConstraint(fitparams,p) ;
      //chisq = filterMassConstraintOnDaughters(fitpar) ;
      break ;
    case Constraint::geometric:
      status |= projectGeoConstraint(fitparams,p) ;
      break ;
    case Constraint::kinematic:
      status |= projectKineConstraint(fitparams,p) ;
      break ;
    case Constraint::lifetime:
      status |= projectLifeTimeConstraint(fitparams,p) ;
      break ;
    case Constraint::conversion:
      status |= projectConversionConstraint(fitparams,p) ;
      break ;
    default:
      status |= ParticleBase::projectConstraint(type,fitparams,p) ;
    }
    return status ;
  }

  ErrCode InternalParticle::projectMassConstraintTwoBody(const FitParams& fitparams,
                                                         Projection& p) const
  {
    // we can also apply the constraint to the daughters. that may
    // work better if the opening angle is small.

    // m^2 = ma^1 + mb^2 + 2 * (Ea*Eb - pxa*pxb - pya*pyb - pza*pzb )

    ParticleBase* d1 = daughters()[0] ;
    ParticleBase* d2 = daughters()[1] ;

    assert(d1->hasEnergy()==false && d2->hasEnergy()==false ) ;

    double mass = pdtMass() ;
    double m1   = d1->pdtMass() ;
    double m2   = d2->pdtMass() ;

    int momindex1 = d1->momIndex() ;
    int momindex2 = d2->momIndex() ;

    // initialize the value
    double px1 = fitparams.par()(momindex1+1) ;
    double py1 = fitparams.par()(momindex1+2) ;
    double pz1 = fitparams.par()(momindex1+3) ;

    double px2 = fitparams.par()(momindex2+1) ;
    double py2 = fitparams.par()(momindex2+2) ;
    double pz2 = fitparams.par()(momindex2+3) ;

    double E1  = std::sqrt( m1*m1 + px1*px1 + py1*py1 + pz1*pz1 ) ;
    double E2  = std::sqrt( m2*m2 + px2*px2 + py2*py2 + pz2*pz2 ) ;

    p.r(1) = m1*m1 + m2*m2 + 2 * (E1*E2 - px1*px2 - py1*py2 - pz1*pz2 ) - mass* mass ;

    // calculate the projection matrix
    p.H(1,momindex1+1) = 2 * (E2 * px1/E1 - px2 ) ;
    p.H(1,momindex1+2) = 2 * (E2 * py1/E1 - py2 ) ;
    p.H(1,momindex1+3) = 2 * (E2 * pz1/E1 - pz2 ) ;
    p.H(1,momindex2+1) = 2 * (E1 * px2/E2 - px1 ) ;
    p.H(1,momindex2+2) = 2 * (E1 * py2/E2 - py1 ) ;
    p.H(1,momindex2+3) = 2 * (E1 * pz2/E2 - pz1 ) ;

    // set the variance in the residual
    double width = pdtWidth() ;
    p.Vfast(1,1) = 4*mass*mass*width*width ;

    return ErrCode::success ;
  }

  void InternalParticle::addToConstraintList(constraintlist& alist,
                                             int depth) const
  {
    // first the daughters
    for(daucontainer::const_iterator it = daughters().begin() ;
        it != daughters().end() ; ++it)
      (*it)->addToConstraintList(alist,depth-1) ;

    //double geoprecision  = 1e-5 ; // 1mu
    //double massprecision = 4*pdtMass()*pdtMass()*1e-5 ; // 0.01 MeV

    // the lifetime constraint
    if(lenIndex()>=0 && m_lifetimeconstraint)
      alist.push_back(Constraint(this,Constraint::lifetime,depth,1)) ;
    // the kinematic constraint
    if(momIndex()>=0)
      alist.push_back(Constraint(this,Constraint::kinematic,depth,4)) ;
    // the geometric constraint
    if(mother() && lenIndex()>=0)
      alist.push_back(Constraint(this,Constraint::geometric,depth,3,3)) ;
    // the mass constraint. FIXME: move to ParticleBase
    if(hasMassConstraint()) {
      if( !m_isconversion )
        alist.push_back(Constraint(this,Constraint::mass,depth,1,10)) ;
      else
        alist.push_back(Constraint(this,Constraint::conversion,depth,1,3)) ;
    }
  }


  std::string InternalParticle::parname(int thisindex) const
  {
    int id = thisindex ;
    // skip the lifetime parameter if there is no mother
    if(!mother() && id>=3) ++id ;
    return ParticleBase::parname(id) ;
  }

  //   struct printfunctor : public unary_function<ParticleBase*,void>
  //   {
  //     printfunctor(const FitParams* fitpar) : _arg(fitpar)  {}
  //     void operator() (const ParticleBase* x) { x->print(_arg) ; }
  //     const FitParams* _arg ;
  //   };


  //   bool InternalParticle::swapMotherDaughter(FitParams* fitparams,
  //          const ParticleBase* newmother)
  //   {
  //     // routine that switches momentum vectors in a vertex, used for
  //     // reconstructing the tagging vertex.
  //     assert((newmother->type()==kBtaComposite||newmother->type()==kBtaResonance)) ;
  //     daucontainer::iterator it = std::find(m_daughters.begin(),m_daughters.end(),newmother) ;
  //     assert( it != m_daughters.end() ) ;

  //     // now start substituting
  //     // 1. assign the missing particle
  //     // 2.
  //     // 3. swap the momenta around

  //     int dummy = newmother->index() ;
  //     ParticleBase* missingparticle = new MissingParticle(0,this) ;
  //     missingparticle->updateIndex(dummy) ;

  //     // invert tau
  //     if( newmother->lenIndex()>=0 && tauIndex()>=0) {
  //       double tau = fitparams->par()(newmother->lenIndex()+1) ;
  //       fitparams->par()(lenIndex()+1) = -tau ;
  //     }

  //     // set the btacandidate
  //     setCandidate( newmother->bc() ) ;

  //     // do the momentum
  //     int momindex = momIndex() ;
  //     int momindexmother = newmother->momIndex() ;
  //     int momindexmissing = missingparticle->momIndex() ;

  //     int maxrow = newmother->hasEnergy() && hasEnergy() ? 4 : 3 ;
  //     double energy2(0) ;
  //     for( int row=1; row<=maxrow; ++row) {
  //       double pxin  = fitparams->par()(momindexmother+row) ;
  //       double pxout = fitparams->par()(momindex      +row) ;
  //       // the new missing particle
  //       fitparams->par()(momindexmissing+row) = 2*pxin - pxout ;
  //       fitparams->par()(momindex+row) = pxin ;
  //       energy2 += pxin*pxin ;
  //     }

  //     if( newmother->hasEnergy() && hasEnergy() ) {
  //       double mass = newmother->pdtMass() ;
  //       double Ein  = sqrt(energy2+mass*mass) ;
  //       double Eout = fitparams->par()(momindex+4) ;
  //       fitparams->par()(momindexmissing+4) = 2*Ein - Eout ;
  //       fitparams->par()(momindex+4)        = Ein ;
  //     }

  //     ParticleBase* newmothercopy = *it ;
  //     *it = missingparticle ;
  //     delete newmothercopy ;

  //     return true ;
  //   }

}
