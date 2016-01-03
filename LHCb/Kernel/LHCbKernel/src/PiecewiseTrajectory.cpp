#include "Kernel/PiecewiseTrajectory.h"
#include <cassert>

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

std::unique_ptr<LHCb::Trajectory> LHCb::PiecewiseTrajectory::clone() const
{
  return std::unique_ptr<LHCb::Trajectory>(new LHCb::PiecewiseTrajectory(*this));
}

LHCb::PiecewiseTrajectory::PiecewiseTrajectory(const PiecewiseTrajectory& rhs)
  :LHCb::Trajectory(rhs)
{
  for ( const auto& i : rhs.m_traj ) append(i.first->clone().release());
}

void
LHCb::PiecewiseTrajectory::append(LHCb::Trajectory *t)
{
  m_traj.emplace_back(std::unique_ptr<LHCb::Trajectory>{t},endRange());
  setRange(beginRange(),endRange()+t->endRange()-t->beginRange());
}

void
LHCb::PiecewiseTrajectory::prepend(LHCb::Trajectory *t)
{
  auto newBeginRange = beginRange()-(t->endRange()-t->beginRange());
  m_traj.emplace_front(std::unique_ptr<LHCb::Trajectory>{t},newBeginRange);
  setRange(newBeginRange,endRange());
}

// pair of (local traj, local mu)
std::pair<const LHCb::Trajectory*,double>
LHCb::PiecewiseTrajectory::loc(double mu) const
{
  assert(!m_traj.empty());
  auto i = std::lower_bound(std::begin(m_traj),
                            std::end(m_traj),
                            mu, order2mu());
  if (i==m_traj.end()) --i;
  return {i->first.get(),i->first->beginRange()+(mu-i->second)};
}

LHCb::Trajectory::Point
LHCb::PiecewiseTrajectory::position(double s) const
{
  return local(s,[](const LHCb::Trajectory* traj, double t) { return traj->position(t); } );
}

LHCb::Trajectory::Vector
LHCb::PiecewiseTrajectory::direction(double s) const{
  return local(s,[](const LHCb::Trajectory* traj, double t) { return traj->direction(t); } );
}

LHCb::Trajectory::Vector
LHCb::PiecewiseTrajectory::curvature(double s) const
{
  return local(s,[](const LHCb::Trajectory* traj, double t) { return traj->curvature(t); } );
}

void
LHCb::PiecewiseTrajectory::expansion( double s,
                                      Point& p,
                                      Vector& dp,
                                      Vector& ddp ) const
{
  local(s, [&]( const LHCb::Trajectory* traj, double t ) {
      traj->expansion(t,p,dp,ddp);
  });
}

double
LHCb::PiecewiseTrajectory::muEstimate(const Point& p) const
{
  //TODO: replace by std::accumulate...
  return for_each(std::begin(m_traj),std::end(m_traj), dist2point(p) ).muEstimate();
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
  auto l = loc(s);
  auto dist = (l.first->*f)(l.second,tolerance,pathDirection);
  if ( (l.first!=m_traj.back().first.get()  && pathDirection>0) ||
       (l.first!=m_traj.front().first.get() && pathDirection<0) ) {  // we're moving amongst
                                                               // the internal pieces
    auto endOfLocalRange = ( pathDirection>0 ? l.first->endRange()
                               : l.first->beginRange() ) ;
    // make sure we don't run beyond the current piece...
    dist = std::min(dist, std::abs( endOfLocalRange - l.second ));
  }
  return dist;
}

std::ostream&
LHCb::PiecewiseTrajectory::print(std::ostream& os) const
{
  for (const auto& i : m_traj ) {
    auto pieceRange = i.first->endRange() - i.first->beginRange();
    os << "   BeginPoint[global/local] : " << i.first->beginPoint() << " [" << i.second << "/" << i.first->beginRange() << "]\n"
       << " ->  EndPoint[global/local] : " << i.first->endPoint() << " [" << i.second+pieceRange << "/" << i.first->endRange() << "]\n" << std::endl;
  }
  return os;

}
