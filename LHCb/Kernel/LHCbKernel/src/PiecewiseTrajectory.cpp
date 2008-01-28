#include "Kernel/PiecewiseTrajectory.h"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/construct.hpp"
#include "boost/lambda/core.hpp"

using boost::lambda::_1;
using boost::lambda::_2;
using boost::lambda::bind;
using boost::lambda::var;
using boost::lambda::delete_ptr;

namespace {
    class dist2point {
        public:
            dist2point (const LHCb::Trajectory::Point& p) : m_p(p) {;} 
            template <typename T> double operator()(const T& t) 
            { return (t.first->position( t.first->muEstimate(m_p) )-m_p).Mag2();}
            template <typename T> bool operator()(const T& t, const T& s)  
            { return (*this)(t)<(*this)(s); }
        private:
            const LHCb::Trajectory::Point& m_p;
    };
    struct order2mu { //TODO: check!
        template <typename T> bool operator()(double s, const T& t)  
        { return s < t.second+t.first->beginRange(); }
        template <typename T> bool operator()(const T& t, double s)  
        { return t.second+t.first->endRange() < s; }
    };
};

std::auto_ptr<LHCb::Trajectory> LHCb::PiecewiseTrajectory::clone() const
{
  return std::auto_ptr<LHCb::Trajectory>(new LHCb::PiecewiseTrajectory(*this));
}

LHCb::PiecewiseTrajectory::PiecewiseTrajectory(const PiecewiseTrajectory& lhs):LHCb::Trajectory(lhs) {

  Trajs::const_iterator iter = lhs.m_traj.begin();
  for(; iter != lhs.m_traj.end() ; ++iter){
    std::auto_ptr<LHCb::Trajectory> traj = iter->first->clone();
    append(traj.release());
  } // iter
}

LHCb::PiecewiseTrajectory::~PiecewiseTrajectory()
{
   std::for_each(m_traj.begin(),
                 m_traj.end(),
                 bind(delete_ptr(),bind(&Trajs::value_type::first,_1)));
}


template <typename RETURN_TYPE, typename FUNCTOR>
RETURN_TYPE LHCb::PiecewiseTrajectory::local(double s, FUNCTOR f) const
{
   // TODO: check!!
   Trajs::const_iterator i = lower_bound(m_traj.begin(),
                                         m_traj.end(),
                                         s,
                                         order2mu());
   const Elem& j = (i==m_traj.end()) ? m_traj.back() : *i;
   double x = s-j.second;
   return f(j.first,x);
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
    Trajs::const_iterator i = min_element(m_traj.begin(),
                                          m_traj.end(),
                                          dist2point(p));
    return i->second+i->first->muEstimate(p);
}

double 
LHCb::PiecewiseTrajectory::distTo1stError( double s,
                                           double tolerance, 
                                           int pathDirection) const 
{
  return local<double>(s, bind( &Trajectory::distTo1stError,
                                _1,_2,tolerance,pathDirection) );
}

double 
LHCb::PiecewiseTrajectory::distTo2ndError( double s,
                                           double tolerance, 
                                           int pathDirection) const 
{
  return local<double>(s, bind( &Trajectory::distTo2ndError,
                                _1,_2,tolerance,pathDirection) );
}

void 
LHCb::PiecewiseTrajectory::append(LHCb::Trajectory *t) 
{
    m_traj.push_back(std::make_pair(t,endRange()-t->beginRange()));
    Range tRange = range();
    setRange(tRange.first, tRange.second + t->endRange()-t->beginRange());
}
// FIXME: prepend needs a bit more thinking -- it's friday afternoon & 
//        time to enjoy the sunshine ;-)
//void 
//PiecewiseTrajectory::prepend(Trajectory *t) 
//{
//    range().first -= t->length();
//    m_traj.push_front(make_pair(t,t->beginRange()+length()));
//}
