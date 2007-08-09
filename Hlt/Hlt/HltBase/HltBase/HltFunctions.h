// $Id: HltFunctions.h,v 1.9 2007-08-09 13:58:06 hernando Exp $
#ifndef HLTBASE_HLTFUNCTIONS_H 
#define HLTBASE_HLTFUNCTIONS_H 1

// Include files

/** @file 
 *  Common HLT functions and filters that operate on tracks and vertices
 *
 *  @author Jose Angel Hernando Morata
 *          based on LoKi ideas by I. Belyaev
 *  @date   2006-07-18
 */

#include "HltBase/HltTypes.h"
#include "HltBase/HltUtils.h"
#include "HltBase/ITrackMatch.h"

namespace Hlt {  
 
  
  /* SmartFunction:    
   *   returns the info of the object given a key, if not apply the function
   */
  template <class T>
  class SmartFunction : public Estd::function<T> 
  {
  public:
    typedef Estd::function<T> Function;
    explicit SmartFunction(int ikey, const Function& f):key(ikey) 
    {fun = f.clone();}
    virtual ~SmartFunction() {delete fun;}
    double operator() (const T& t) const {
      double v = t.info(key,-1.e6); if (v != 1.e-6) v = (*fun)(t);
      return v;
    }
    Function* clone() const {return new SmartFunction(key,*fun);}
    int key;
    Function* fun;
  };
  
  /* Info:    
   *   returns the info of the object given a key
   *   ie. if T is a track. track.info(keyPT) return the IP of the track
   */
  template <class T>
    class Info : public Estd::function<T> 
    {
    public:
      typedef Estd::function<T> Function;
      explicit Info(int ikey) : key(ikey) { }
      double operator() (const T& t) const 
	{return t.info(key,-1);}
      Function* clone() const {return new Info<T>(key);}
      int key; 
    };
  
  /* AbsInfo:    
   *   returns the abs info of the object given a key
   *   ie. if T is a track. track.info(keyPT) return the abs IP of the track
   */
  template <class T>
    class AbsInfo : public Estd::function<T> 
    {
    public:
      typedef Estd::function<T> Function;
      explicit AbsInfo(int ikey) : key(ikey) { }
      double operator() (const T& t) const 
	{return fabs(t.info(key,0.));}
      Function* clone() const {return new AbsInfo<T>(key);}
      int key;
    };  


  /* AbsInfo:    
   *   returns the abs info of the object given a key
   *   ie. if T is a track. track.info(keyPT) return the abs IP of the track
   */
  template <class T>
    class AbsFun : public Estd::function<T> 
    {
    public:
      typedef Estd::function<T> Function;
      explicit AbsFun(const Function& fun) : _fun(fun.clone()) { }
      virtual ~AbsFun() {delete _fun;}
      double operator() (const T& t) const 
      {return fabs((*_fun)(t));}
      Function* clone() const {return new AbsFun<T>(*_fun);}
    public:
      Function* _fun;
    };  


  /* AbsInfo:    
   *   returns the results of a member function
   */
  template <class T>
  class MemberFunction : public Estd::function<T> 
  {
  public:
    typedef Estd::function<T> Function;
    typedef double (T::* ptr_menfun) () const;
    explicit MemberFunction(ptr_menfun pmf):m_pmf(pmf) {}
    double operator() (const T& t) const
    {return ((&t)->*m_pmf)();}
    Function* clone() const 
    {return new MemberFunction<T>(m_pmf);}
  public:
    ptr_menfun m_pmf;
  };

  /* Return the key of the best match
   *
   */
  template <class T1, class T2>
  class BinderKey : public Estd::function<T1> 
  {
  public:
    typedef Estd::function<T1> Function;
    typedef Estd::binder_function<T1,T2> Binder;
    typedef typename std::vector<T2*>::iterator Iterator;
    explicit BinderKey(const Binder& bin)
    {m_binder = dynamic_cast<Binder*>(bin.clone());}
    virtual ~BinderKey() {delete m_binder;}
    double operator() (const T1& t1) const {
      std::pair<double,Iterator> pair = m_binder->pair(t1);
      return (double) (*(pair.second))->key();
    }
    Function* clone() const 
    {return new BinderKey<T1,T2>(*m_binder);}
  public:
    Binder* m_binder;
  };

  /* AbsInfo:    
   *   returns the Pt of a track
   */
  class PT : public Hlt::TrackFunction {
  public:
    explicit PT() {}
    double operator() (const LHCb::Track& t) const {return t.pt();}
    Estd::function<LHCb::Track>* clone() const {return new PT();}
  };  
  
  /* CheckFlag:    
   *   returns true if a a given flag it set on the track
   */
  class CheckFlag  : public Hlt::TrackFilter {
  public:
    explicit CheckFlag(LHCb::Track::Flags f = LHCb::Track::Backward)
    {flag = f;}
    bool operator()(const LHCb::Track& t) const {return t.checkFlag(flag);}
    Estd::filter<LHCb::Track>* clone() const {return new CheckFlag(flag);}
    LHCb::Track::Flags flag;      
  };

  
  class Charge : public Hlt::TrackFunction {
  public:
    explicit Charge() {}
    double operator() (const LHCb::Track& t) const {return t.charge();}
    Estd::function<LHCb::Track>* clone() const {return new Charge();}
  };
  
  class P : public Hlt::TrackFunction {
  public:
    explicit P() {}
    double operator() (const LHCb::Track& t) const {return t.p();}
    Estd::function<LHCb::Track>* clone() const {return new P();}
  };
  
  class DeltaE : public Hlt::TrackFunction
  {
  public:
    explicit DeltaE() {}
    double operator() (const LHCb::Track& t) const;    
    Estd::function<LHCb::Track>* clone() const {return new DeltaE();}
  };

  class DeltaP : public Hlt::TrackBiFunction
  {
  public:
    explicit DeltaP() {}
    double operator() (const LHCb::Track& t1, const LHCb::Track& t2) const;    
    Hlt::TrackBiFunction* clone() const {return new DeltaP();}
  };

  class minPT : public Hlt::VertexFunction {
  public:
    explicit minPT() {}
    double operator() (const LHCb::RecVertex& v) const {
      return HltUtils::minPT(v);}
    Hlt::VertexFunction* clone() const {return new minPT();}
  };

  class maxPT : public Hlt::VertexFunction {
  public:
    explicit maxPT() {}
    double operator() (const LHCb::RecVertex& v) const {
      return HltUtils::maxPT(v);}
    Hlt::VertexFunction* clone() const {return new maxPT();}
  };

  class TrackMatch : public Hlt::TrackBiFunction {
  public:
    explicit TrackMatch(ITrackMatch& tool) {_tool = &tool;}
    double operator() (const LHCb::Track& t1, const LHCb::Track& t2) const
    {double chi = 0.;_tool->match(t1,t2,chi);return chi;}
    Hlt::TrackBiFunction* clone() const {return new Hlt::TrackMatch(*_tool);}
    ITrackMatch* _tool;
  };


  /* rIP:
   *   return the radial impact parameter between a track and a vertex
   *   track needs to be of Velo RZ type!
   */
  class rIP : public Hlt::TrackVertexBiFunction {
  public:
    explicit rIP(){}
    double operator() (const LHCb::Track& track, 
                       const LHCb::RecVertex& vertex) const
    {return HltUtils::rImpactParameter(vertex, track);}
    Estd::bifunction<LHCb::Track,LHCb::RecVertex>* clone() const
    {return new rIP();}
  };

  /* IP:
   *   return the impact parameter between a track and a vertex
   */
  class IP : public Hlt::TrackVertexBiFunction {
  public:
    explicit IP(){}
    double operator() (const LHCb::Track& track, 
                       const LHCb::RecVertex& vertex) const
    {return HltUtils::impactParameter(vertex, track);}
    Estd::bifunction<LHCb::Track,LHCb::RecVertex>* clone() const
    {return new IP();}
  };

  /* DZ:
   *  computes the distance in z between 2 vertices: v1-v2
   */
  class DZ : public Hlt::VertexBiFunction {
  public:
    explicit DZ() {}
    double operator() (const LHCb::RecVertex& vertex1, 
                       const LHCb::RecVertex& vertex2) const {
      double dz = vertex1.position().z() - vertex2.position().z();
      return dz;
    }
    Estd::bifunction<LHCb::RecVertex,LHCb::RecVertex>* clone() const
    {return new DZ();}
  };
  
  /* FC:
   *  computes the poiting of the vertex1 with respect vertex2 (primary)
   */
  class FC : public Hlt::VertexBiFunction {
  public:
    explicit FC() {}
    double operator() (const LHCb::RecVertex& vertex1, 
                       const LHCb::RecVertex& vertex2) const {
      return HltUtils::FC(vertex1,vertex2);
    }
    Estd::bifunction<LHCb::RecVertex,LHCb::RecVertex>* clone() const
    {return new FC();}
  };

  /* FC2:
   *  computes the poiting of the vertex1 with respect vertex2 (primary)
   */
  class FC2 : public Hlt::VertexBiFunction {
  public:
    explicit FC2() {}
    double operator() (const LHCb::RecVertex& vertex1, 
                       const LHCb::RecVertex& vertex2) const {
      return HltUtils::FC2(vertex1,vertex2);
    }
    Estd::bifunction<LHCb::RecVertex,LHCb::RecVertex>* clone() const
    {return new FC2();}
  };

  /* DOCA
   *   compute the distance of closest approach between two tracks
   */
  class DOCA : public Hlt::TrackBiFunction {
  public:
    explicit DOCA() {}
    double operator() (const LHCb::Track& track1, 
                       const LHCb::Track& track2) const {
      return HltUtils::closestDistanceMod(track1,track2);
    }
    Estd::bifunction<LHCb::Track,LHCb::Track>* clone() const
    {return new DOCA();}
  };  


  /* matchIDsFraction
   */
  class MatchIDsFraction : public Hlt::TrackBiFunction {
  public:
    explicit MatchIDsFraction() {}
    double operator() (const LHCb::Track& track1, 
                       const LHCb::Track& track2) const {
      return HltUtils::matchIDsFraction(track1,track2);
    }
    Hlt::TrackBiFunction* clone() const {return new MatchIDsFraction();}
  };

  /* DOCA
   *   compute the distance of closest approach between two tracks
   */
  class DeltaAngle : public Hlt::TrackBiFunction {
  public:
    explicit DeltaAngle() {}
    double operator() (const LHCb::Track& track1, 
                       const LHCb::Track& track2) const {
      return HltUtils::deltaAngle(track1,track2);
    }
    Hlt::TrackBiFunction* clone() const {return new DeltaAngle();}
  };  
 
  /* It fills the vertex using the 2 tracks
   *
   */
  class VertexCreator 
  {
  public:
    explicit VertexCreator() {}
    void operator() (const LHCb::Track& track1,
                     const LHCb::Track& track2,
                     LHCb::RecVertex& ver) const;
  };
  
  /* Functor: to order tracks by decresing order of PT
   *
   */
  class SortTrackByPt {
  public:
    bool operator() (const LHCb::Track* iniTrack, 
                     const LHCb::Track* endTrack ) const;
  };

  typedef Estd::binder_cbifunction<LHCb::RecVertex,LHCb::RecVertex> VVFunct;
  class TFC : public VVFunct { 
    TFC(): VVFunct(&HltUtils::FC) {}
  };


  // class TDOCA : public VVFunct { 
  //  TDOCA(): VVFunct(&HltUtils::FC) {}
  // };


};
#endif // HLTBASE_HLTOPER_H
