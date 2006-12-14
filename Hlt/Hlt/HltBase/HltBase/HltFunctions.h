// $Id: HltFunctions.h,v 1.3 2006-12-14 11:21:35 hernando Exp $
#ifndef HLTBASE_HLTFUNCTIONS_H 
#define HLTBASE_HLTFUNCTIONS_H 1

// Include files

/** common HLT functions and filters that operates in tracks, and vertices
 *
 *  @author Jose Angel Hernando Morata
 *          based on LoKi ideas by I. Belyaev
 *  @date   2006-07-18
 */

#include "HltBase/EFunctions.h"
#include "HltBase/HltContainers.h"
#include "HltBase/HltUtils.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"

namespace Hlt {
  
  
  /* definition of track and vertex functions and filters
   */
  typedef Estd::function<LHCb::Track>     track_function;  
  typedef Estd::function<LHCb::RecVertex> vertex_function;
  typedef Estd::filter<LHCb::Track>       track_filter;
  typedef Estd::filter<LHCb::RecVertex>   vertex_filter;
  
  typedef Estd::bifunction<LHCb::Track, LHCb::Track>         track_bifunction;  
  typedef Estd::bifunction<LHCb::RecVertex, LHCb::RecVertex> vertex_bifunction;
  typedef Estd::bifunction<LHCb::Track, LHCb::RecVertex>     trackvertex_bifunction;
  
  typedef Estd::bifilter<LHCb::Track,LHCb::Track>          track_bifilter;  
  typedef Estd::bifilter<LHCb::RecVertex, LHCb::RecVertex> vertex_bifilter;
  
  
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
   *   returns the Pt of a track
   */
  class PT : public track_function {
  public:
    explicit PT() {}
    double operator() (const LHCb::Track& t) const {return t.pt();}
    Estd::function<LHCb::Track>* clone() const {return new PT();}
  };  

  /* CheckFlag:    
   *   returns true if a a given flag it set on the track
   */
  class CheckFlag  : public track_filter {
  public:
    explicit CheckFlag(LHCb::Track::Flags f = LHCb::Track::Backward)
    {flag = f;}
    bool operator()(const LHCb::Track& t) const {return t.checkFlag(flag);}
    Estd::filter<LHCb::Track>* clone() const {return new CheckFlag(flag);}
    LHCb::Track::Flags flag;      
  };

  
  class Charge : public track_function {
  public:
    explicit Charge() {}
    double operator() (const LHCb::Track& t) const {return t.charge();}
    Estd::function<LHCb::Track>* clone() const {return new Charge();}
  };
  
  class P : public track_function {
  public:
    explicit P() {}
    double operator() (const LHCb::Track& t) const {return t.p();}
    Estd::function<LHCb::Track>* clone() const {return new P();}
  };

  /* rIP:
   *   return the radial impact parameter between a track and a vertex
   *   track needs to be of Velo RZ type!
   */
  class rIP : public trackvertex_bifunction {
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
  class IP : public trackvertex_bifunction {
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
  class DZ : public vertex_bifunction {
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
  class FC : public vertex_bifunction {
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
  class FC2 : public vertex_bifunction {
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
  class DOCA : public track_bifunction {
  public:
    explicit DOCA() {}
    double operator() (const LHCb::Track& track1, 
                       const LHCb::Track& track2) const {
      return HltUtils::closestDistanceMod(track1,track2);
    }
    Estd::bifunction<LHCb::Track,LHCb::Track>* clone() const
    {return new DOCA();}
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


  template <class CON>
  class minDZ : public Estd::binder_function<LHCb::RecVertex,LHCb::RecVertex,CON> {
  public:
    explicit minDZ(CON& con): 
      Estd::binder_function<LHCb::RecVertex,LHCb::RecVertex,CON>
    ( Hlt::DZ(),con,Estd::abs_min() ) {}
  };


  template <class CON>
  class minFC : public Estd::binder_function<LHCb::RecVertex,LHCb::RecVertex,CON> {
  public:
    explicit minFC(CON& con): 
      Estd::binder_function<LHCb::RecVertex,LHCb::RecVertex,CON>
    ( Hlt::DZ(), con, Estd::abs_min() ) {}
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
