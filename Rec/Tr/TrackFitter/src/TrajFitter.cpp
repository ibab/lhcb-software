// $Id: TrajFitter.cpp,v 1.1 2007-04-17 15:22:13 graven Exp $
// Include files 
// -------------
#include <algorithm>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/ToolFactory.h"
// From LHCbMath
#include "LHCbMath/MatrixManip.h"


#include "TrajFitter.h"
#include "TrackInterfaces/ITrackProjectorSelector.h"
#include "TrackInterfaces/ITrackProjector.h"
#include "Kernel/ITrajPoca.h"
#include "Kernel/DifTraj.h"
#include "Event/State.h"
#include "Event/Measurement.h"

#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/construct.hpp"

using namespace boost;
using namespace boost::lambda;
using namespace std;
using namespace Gaudi::Math;

DECLARE_TOOL_FACTORY( TrajFitter );
//=========================================================================
// Standard Constructor, initializes variables
//=========================================================================
TrajFitter::TrajFitter( const string& type,
                        const string& name,
                        const IInterface* parent)
  : GaudiTool( type, name, parent),
    m_poca(0),m_proj(0)
{
  declareInterface<ITrajFitter>( this );

  declareProperty("MaxIterations", m_maxIter = 5 );
  declareProperty("MaxDeltaChiSq", m_deltaChiSq = 0.01 );
}

//=========================================================================
// Destructor
//=========================================================================
TrajFitter::~TrajFitter() {
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode TrajFitter::initialize() 
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  m_poca = tool<ITrajPoca>("TrajPoca","Poca",this);
  m_proj = tool<ITrackProjectorSelector>( "TrackProjectorSelector",
                                          "Projector", this );
  return StatusCode::SUCCESS;
}


// small adaptor to go from traj parameterization to the 'state'
// one needed by the projectors -- that prevents a lot of code duplication
// (thanks to Wouter for the suggestion ... it was so obvious I didn't realize 
// it myself ;-)
namespace {
    template <typename TRAJ>
    class Residual {
    public:
        typedef typename ITrajFitter::Types<TRAJ::kSize>::Derivative    Derivative;
        typedef typename ITrajFitter::Types<TRAJ::kSize>::ResDerivative Result;

        Residual(LHCb::Measurement* meas,/*const*/ ITrajPoca* poca,const ITrackProjectorSelector* proj ) 
            : m_meas(meas),m_poca(poca),m_proj(proj->projector(*meas)) {};

        Result project(const TRAJ& traj) const {
            // go from traj parameters to a state at the right point
            double s1(0),s2(0);
            static Gaudi::XYZPoint pos;
            static Gaudi::XYZVector dist, dir;
            m_poca->minimize(traj,s1,m_meas->trajectory(),s2,dist,0.005*Gaudi::Units::mm);
            // note: we don't need a complete state -- no need for errors...
            // also, since we are at poca, and thus _create_ a state at s=0,
            // only need the position (i.e. x and y), and neither slopes nor momentum as the 
            // derivatives wrt. slopes & momentum are proportional to s and s^2 resp.
            pos = traj.position(s1);
            dir = traj.direction(s1);
            LHCb::State s; s.setState(pos.x(),pos.y(),pos.z(),dir.x()/dir.z(),dir.y()/dir.z(),0);
            m_proj->project(s,*m_meas);
            // convert state derivatives to traj derivates (long live the chain rule!)
            const Gaudi::TrackProjectionMatrix& d0 = m_proj->projectionMatrix();
            typename TRAJ::Derivative d1 = traj.derivative(s1);
            Derivative d;
            for (int i=0;i<2;++i) for (int j=0;j<TRAJ::kSize;++j) d(0,j) += d0(0,i)*d1(i,j);
            // and normalize by the measurement error
            double scale = m_proj->errResidual();
            return make_pair( m_proj->residual()/scale, d/scale );
        }
        private:
            LHCb::Measurement     *m_meas;
            /*const*/ ITrajPoca   *m_poca;
            ITrackProjector       *m_proj;
    };

    // make a step toward the minimimum, and return the delta-chisquared achieved.
    template <typename TRAJ>
    double stepTowardMinimum(TRAJ& traj, const vector<Residual<TRAJ> >& residuals)
    {
      typedef vector<Residual<TRAJ> >                                 Residuals;
      typedef typename ITrajFitter::Types<TRAJ::kSize>::ResDerivative Result;
      typedef typename TRAJ::Parameters                               Vector;
      typedef ROOT::Math::SMatrix<double,TRAJ::kSize,TRAJ::kSize>     Matrix; // FIXME: symmetric!!!

      double chisq(0); Vector vec; Matrix mat;
      for (typename Residuals::const_iterator r = residuals.begin(); r != residuals.end(); ++r) {
            Result result = r->project(traj);
            vec   += result.first*result.second.Row(0);
            mat   += Transpose(result.second)*result.second;
            chisq += result.first*result.first;
      }
      mat.Invert(); //FIXME: brute force solution of the linear system...
                    // should try to find a better way of doing this (eg. use some GSL routine) 
      traj+=mat*vec; // update the traj with this step.
      double deltachisq = Similarity(mat,vec);
      return deltachisq;
    };
}

//=========================================================================
// Fit that traj!
//=========================================================================

template <typename TRAJ,typename RESIDS>
StatusCode 
TrajFitter::fit(TRAJ* traj, 
                RESIDS* resids, 
                const LHCb::Measurement::Vector& measurements) const 
{
  if (traj==0) return StatusCode::FAILURE;
  // transform input data to something more appropriate...
  typedef Residual<TRAJ> Resid;
  vector<Resid> r;
  transform(measurements.begin(), measurements.end(), 
            back_inserter(r), 
            bind(constructor<Resid>(),_1,m_poca,m_proj));
  bool converged(false);
  for (unsigned n = 0; !converged && n<m_maxIter; ++n) {
     double deltaChiSq = stepTowardMinimum(*traj,r);
     converged = (deltaChiSq<m_deltaChiSq);
  }
  if (resids!=0) {
      Assert(resids->empty(),"Non-empty resid vector???");
      for (typename vector<Resid>::iterator i=r.begin();i!=r.end();++i) 
         resids->push_back( i->project(*traj) );
  }
  return converged ? StatusCode::SUCCESS 
                   : Warning("Minimization did not converge");
}

StatusCode TrajFitter::Nfit(ITrajFitter::NDifTraj traj, 
                            ITrajFitter::NResiduals resids,
                            const LHCb::Measurement::Vector& measurements) const 
{
    // dispatch to templated member functions
    // note: this also instantiates the actual code...
    switch( traj.n() ) {
    case 0: return StatusCode::FAILURE;
#define FIT(N) case N: return fit(traj.get<N>(),resids.get<N>(),measurements)
           ;FIT( 2);FIT( 3);FIT( 4);FIT( 5);FIT( 6);FIT( 7);FIT( 8);FIT( 9);FIT(10);
    FIT(11);FIT(12);FIT(13);FIT(14);FIT(15);FIT(16);FIT(17);FIT(18);FIT(19);FIT(20);
#undef FIT
    }
    error() << "Can not (yet) fit a DifTraj<" 
            << traj.n() << ">: please update TrajFitter::Nfit" 
            << " to support this number of parameters..." 
            << endmsg;
    return Error("The # of parameters of the input DifTraj is not (yet) supported");
}
