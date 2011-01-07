#include "Kernel/PiecewiseTrajectory.h"
// Boost
#ifdef __INTEL_COMPILER        // Disable ICC remark from Boost
  #pragma warning(disable:177) // variable was declared but never referenced
  #pragma warning(disable:193) // zero used for undefined preprocessing identifier
#endif
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/construct.hpp"
#include "boost/lambda/core.hpp"
#include <cassert>

using boost::lambda::_1;
using boost::lambda::_2;
using boost::lambda::bind;
using boost::lambda::var;
using boost::lambda::delete_ptr;

namespace {
    class dist2point {
        public:
            dist2point (const LHCb::Trajectory::Point& p) : m_p(p), m_dist2(-1),m_mu(0) {;} 
            template <typename T> void operator()(const T& t) 
            { 
              // make sure mu isn't beyond the local traj valid range...
              double mu = std::min(std::max(t.first->beginRange(),
                                            t.first->muEstimate(m_p)),
                                            t.first->endRange());
              double dist2 = ( t.first->position(mu) - m_p ).Mag2();
              if ( m_dist2 < 0 || dist2 < m_dist2) {
                m_dist2 = dist2; 
                // m_mu should be wrt. global traj, not the local piece
                m_mu    = t.second+(mu-t.first->beginRange());
              }
            }
            double muEstimate() const { return m_mu; }
        private:
            const LHCb::Trajectory::Point& m_p;
            double m_dist2,m_mu;
    };
    struct order2mu {
        template <typename T> bool operator()(double s, const T& t)  
        { return s < t.second; }
        template <typename T> bool operator()(const T& t, double s)  
        { return t.second < s; }
    };
}

std::auto_ptr<LHCb::Trajectory> LHCb::PiecewiseTrajectory::clone() const
{
  return std::auto_ptr<LHCb::Trajectory>(new LHCb::PiecewiseTrajectory(*this));
}

LHCb::PiecewiseTrajectory::PiecewiseTrajectory(const PiecewiseTrajectory& lhs)
    :LHCb::Trajectory(lhs) 
{
  Trajs::const_iterator iter = lhs.m_traj.begin();
  for(; iter != lhs.m_traj.end() ; ++iter) {
    append(iter->first->clone().release());
  }
}

LHCb::PiecewiseTrajectory::~PiecewiseTrajectory()
{
   std::for_each(m_traj.begin(),
                 m_traj.end(),
                 bind<void>(delete_ptr(),bind(&Trajs::value_type::first,_1)));
}

void 
LHCb::PiecewiseTrajectory::append(LHCb::Trajectory *t) 
{
   m_traj.push_back(std::make_pair(t,endRange()));
   setRange(beginRange(),endRange()+t->endRange()-t->beginRange());
}

void 
LHCb::PiecewiseTrajectory::prepend(LHCb::Trajectory *t) 
{
    double newBeginRange = beginRange()-(t->endRange()-t->beginRange());
    m_traj.push_front(std::make_pair(t,newBeginRange));
    setRange(newBeginRange,endRange());
}

// pair of (local traj, local mu)
std::pair<const LHCb::Trajectory*,double>
LHCb::PiecewiseTrajectory::loc(double mu) const
{
   assert(!m_traj.empty());
   Trajs::const_iterator i = std::lower_bound(m_traj.begin(),
                                              m_traj.end(),
                                              mu,
                                              order2mu());
   if (i==m_traj.end()) --i;
   return std::make_pair(i->first,i->first->beginRange()+(mu-i->second));
}

LHCb::Trajectory::Point
LHCb::PiecewiseTrajectory::position(double s) const
{
   return local<Point>(s,bind(&Trajectory::position,_1,_2));
}

LHCb::Trajectory::Vector 
LHCb::PiecewiseTrajectory::direction(double s) const{
   return local<Vector>(s,bind(&Trajectory::direction,_1,_2));
}
 
LHCb::Trajectory::Vector
LHCb::PiecewiseTrajectory::curvature(double s) const
{
   return local<Vector>(s,bind(&Trajectory::curvature,_1,_2));
}

void 
LHCb::PiecewiseTrajectory::expansion( double s,
                                      Point& p,
                                      Vector& dp,
                                      Vector& ddp ) const
{
   local<void>(s, bind(&Trajectory::expansion,
                       _1,_2,var(p),var(dp),var(ddp)));
}

double
LHCb::PiecewiseTrajectory::muEstimate(const Point& p) const 
{
    return for_each(m_traj.begin(),m_traj.end(), dist2point(p) ).muEstimate();
}

double 
LHCb::PiecewiseTrajectory::distTo1stError( double s,
                                           double tolerance, 
                                           int pathDirection) const 
{
    return distToError(s,tolerance,pathDirection,&Trajectory::distTo1stError);
}

double 
LHCb::PiecewiseTrajectory::distTo2ndError( double s,
                                           double tolerance, 
                                           int pathDirection) const 
{
    return distToError(s,tolerance,pathDirection,&Trajectory::distTo2ndError);
}
  
double 
LHCb::PiecewiseTrajectory::distToError( double s,
                                        double tolerance, 
                                        int pathDirection,
                                        distFun f) const 
{
  std::pair<const Trajectory*,double> l = loc(s);
  double dist = (l.first->*f)(l.second,tolerance,pathDirection);
  if ( (l.first!=m_traj.back().first  && pathDirection>0) || 
       (l.first!=m_traj.front().first && pathDirection<0) ) {  // we're moving amongst 
                                                               // the internal pieces
      double endOfLocalRange = ( pathDirection>0 ? l.first->endRange() 
                                                 : l.first->beginRange() ) ;
      // make sure we don't run beyond the current piece...
      dist = std::min(dist, std::abs( endOfLocalRange - l.second ));
  }
  return dist;
}

std::ostream& 
LHCb::PiecewiseTrajectory::print(std::ostream& os) const
{
   for (Trajs::const_iterator i=m_traj.begin();i != m_traj.end();++i) {
        double pieceRange = i->first->endRange() - i->first->beginRange();
        os << "   BeginPoint[global/local] : " << i->first->beginPoint() << " [" << i->second << "/" << i->first->beginRange() << "]\n"
           << " ->  EndPoint[global/local] : " << i->first->endPoint() << " [" << i->second+pieceRange << "/" << i->first->endRange() << "]\n" << std::endl;
   }
   return os;

}
