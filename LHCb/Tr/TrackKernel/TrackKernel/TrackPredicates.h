#include "Event/Track.h"

namespace TrackPredicates
{
  struct Type
  {
    int m_type ;
    Type(int atype) : m_type(atype) {} 
    bool operator()(const LHCb::Track* track) const { return track->type()==m_type ; }
  } ;

  struct Flag
  {
    LHCb::Track::Flags m_flag ;
    bool m_positive ;
    Flag(LHCb::Track::Flags flag, bool positive=true) : m_flag(flag)
								    , m_positive(positive) {} 
    bool operator()(const LHCb::Track* track) const { 
      return m_positive ? track->checkFlag(m_flag) : !track->checkFlag(m_flag) ; }
  } ;

  struct VeloSide
  {
    int m_sign ;
    VeloSide(int asign) : m_sign(asign) {}
    bool operator()(const LHCb::Track* track) const { 
      return track->firstState().tx()*m_sign*(track->checkFlag(LHCb::Track::Backward)? -1 : 1 )>0 ; }
  } ;

  struct MaxChisqPerDoF
  {
    double m_maxchisq ;
    MaxChisqPerDoF(double maxChisqPerDof) : m_maxchisq(maxChisqPerDof) {}
    bool operator()(const LHCb::Track* track) const { 
      return track->chi2PerDoF() <m_maxchisq  ; }
  } ;
}
