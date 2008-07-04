// $Id: HltFunctions.h,v 1.5 2008-07-04 08:07:41 graven Exp $
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
#include "TrackInterfaces/IFunctionTool.h"
#include "HltBase/IBiFunctionTool.h"

namespace Hlt {  

  
  /* SmartFunction:    
   *   returns the info of the object given a key, if not apply the function
   */
  template <class T>
  class SmartFunction : public zen::function<T> 
  {
  public:
    typedef zen::function<T> Function;
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
  class Info : public zen::function<T> 
  {
  public:
    typedef zen::function<T> Function;
    explicit Info(int ikey) : key(ikey) { }
    double operator() (const T& t) const 
    {return t.info(key,-1);}
    Function* clone() const {return new Info<T>(key);}
    int key; 
  };

  
  /* PT:    
   *   returns the Pt of a track
   */
  class PT : public Hlt::TrackFunction {
  public:
    explicit PT() {}
    double operator() (const LHCb::Track& t) const {return t.pt();}
    zen::function<LHCb::Track>* clone() const {return new PT();}
  };

  /* CheckFlag:    
   *   returns 1. is the track has this flag
   */
  class TrackFlag  : public Hlt::TrackFunction {
  public:
    explicit TrackFlag(LHCb::Track::Flags f = LHCb::Track::Backward)
    {flag = f;}
    double operator()(const LHCb::Track& t) const{
      return (double) t.checkFlag(flag);
    }
    Hlt::TrackFunction* clone() const {return new TrackFlag(flag);}
    LHCb::Track::Flags flag;      
  };
  
  
  class Charge : public Hlt::TrackFunction {
  public:
    explicit Charge() {}
    double operator() (const LHCb::Track& t) const {return t.charge();}
    zen::function<LHCb::Track>* clone() const {return new Charge();}
  };
  
  class P : public Hlt::TrackFunction {
  public:
    explicit P() {}
    double operator() (const LHCb::Track& t) const {return t.p();}
    zen::function<LHCb::Track>* clone() const {return new P();}
  };
  

  class ETCalo : public Hlt::TrackFunction
  {
  public:
    explicit ETCalo() {}
    double operator() (const LHCb::Track& t) const;    
    zen::function<LHCb::Track>* clone() const {return new ETCalo();}
  };

  class DeltaE : public Hlt::TrackFunction
  {
  public:
    explicit DeltaE() {}
    double operator() (const LHCb::Track& t) const;    
    zen::function<LHCb::Track>* clone() const {return new DeltaE();}
  };
  
  class DeltaP : public Hlt::TrackBiFunction
  {
  public:
    explicit DeltaP() {}
    double operator() (const LHCb::Track& t1, const LHCb::Track& t2) const;    
    Hlt::TrackBiFunction* clone() const {return new DeltaP();}
  };

  class VertexMinPT : public Hlt::VertexFunction {
  public:
    explicit VertexMinPT() {}
    double operator() (const LHCb::RecVertex& v) const {
      return HltUtils::VertexMinPT(v);}
    Hlt::VertexFunction* clone() const {return new VertexMinPT();}
  };

  class VertexMaxPT : public Hlt::VertexFunction {
  public:
    explicit VertexMaxPT() {}
    double operator() (const LHCb::RecVertex& v) const {
      return HltUtils::VertexMaxPT(v);}
    Hlt::VertexFunction* clone() const {return new VertexMaxPT();}
  };

  class VertexMatchIDsFraction : public Hlt::VertexBiFunction {
  public:
    explicit VertexMatchIDsFraction() {}
    double operator() (const LHCb::RecVertex& v,
                       const LHCb::RecVertex& vref
                       ) const {
      return HltUtils::vertexMatchIDsFraction(vref,v);
      
    }
    zen::bifunction<LHCb::RecVertex, LHCb::RecVertex>* clone() const
    {return new VertexMatchIDsFraction();}
  };
  

  class VertexMinIP: public Hlt::VertexBiFunction {
  public:
    explicit VertexMinIP(){}
    double operator() (const LHCb::RecVertex& v,
                       const LHCb::RecVertex& pv) const{
      double ip0 = HltUtils::impactParameter(pv,*(v.tracks()[0]));
      double ip1 = HltUtils::impactParameter(pv,*(v.tracks()[1]));
      return (fabs(ip1)<fabs(ip0)) ? ip1 : ip0;
    }
    Hlt::VertexBiFunction* clone() const {return new VertexMinIP();}
  };

  template <class T, class ITOOL>
  class FunctionTool : public zen::function<T> {
  public:
    explicit FunctionTool() {_tool = 0;}
    explicit FunctionTool(ITOOL& tool) {_tool = &tool;}
    double operator() (const T& t1) const {
      if (!_tool) throw GaudiException(" null tool pointer","",StatusCode::FAILURE );
      return _tool->function(t1);
    }
    zen::function<T>* clone() const {
      if (_tool) return new Hlt::FunctionTool<T,ITOOL>(*_tool);
      return new Hlt::FunctionTool<T,ITOOL>();
    }
    ITOOL* _tool;
  };

  template <class T, class T2, class ITOOL>
  class BiFunctionTool : public zen::bifunction<T,T2> {
  public:
    explicit BiFunctionTool() {_tool = 0;}
    explicit BiFunctionTool(ITOOL& tool) {_tool = &tool;}
    double operator() (const T& t1, const T2& t2) const {
      if (!_tool) throw GaudiException(" null tool pointer","",StatusCode::FAILURE );
      double value = _tool->function(t1,t2);return value;
    }
    zen::bifunction<T,T2>* clone() const {
      if (_tool) return new Hlt::BiFunctionTool<T,T2,ITOOL>(*_tool);
      return new Hlt::BiFunctionTool<T,T2,ITOOL>();
    }
    ITOOL* _tool;
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
    zen::bifunction<LHCb::Track,LHCb::RecVertex>* clone() const
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
    zen::bifunction<LHCb::Track,LHCb::RecVertex>* clone() const
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
    zen::bifunction<LHCb::RecVertex,LHCb::RecVertex>* clone() const
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
    zen::bifunction<LHCb::RecVertex,LHCb::RecVertex>* clone() const
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
    zen::bifunction<LHCb::RecVertex,LHCb::RecVertex>* clone() const
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
    zen::bifunction<LHCb::Track,LHCb::Track>* clone() const
    {return new DOCA();}
  };

  class DimuonMass : public Hlt::TrackBiFunction {
  public:
    explicit DimuonMass() {}
    double operator() (const LHCb::Track& track1, 
                       const LHCb::Track& track2) const {
      return HltUtils::invariantMass(track1,track2,
                                     105.658369,105.658369);
    }
    zen::bifunction<LHCb::Track,LHCb::Track>* clone() const
    {return new DimuonMass();}
  };

  class VertexDimuonMass : public Hlt::VertexFunction {
  public:
    explicit VertexDimuonMass() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      const LHCb::Track& t1 = *(vertex.tracks()[0]);
      const LHCb::Track& t2 = *(vertex.tracks()[1]);
      return Hlt::DimuonMass()(t1,t2);
    }
    zen::function<LHCb::RecVertex>* clone() const
    {return new VertexDimuonMass();}
  };

  
  class SumPT : public Hlt::TrackBiFunction {
  public:
    explicit SumPT() {}
    double operator() (const LHCb::Track& track1,
                       const LHCb::Track& track2) const {
      return track1.pt()+track2.pt();
    }
    zen::bifunction<LHCb::Track,LHCb::Track>* clone() const
    {return new SumPT();}
  };

  class VertexSumPT : public Hlt::VertexFunction {
  public:
    explicit VertexSumPT() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      Hlt::SumPT fun;
      const LHCb::Track& t1 = *(vertex.tracks()[0]);
      const LHCb::Track& t2 = *(vertex.tracks()[1]);
      return Hlt::SumPT()(t1,t2);
    }
    zen::function<LHCb::RecVertex>* clone() const
    {return new VertexSumPT();}
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


  /*
   * returns 1. if the two tracks has the same M3 hit
   */
  class DoShareM3 : public Hlt::TrackBiFunction {
  public:
    explicit DoShareM3() {}
    double operator() (const LHCb::Track& track1, 
                       const LHCb::Track& track2) const {
      return (double) HltUtils::doShareM3(track1,track2);
    }
    zen::bifunction<LHCb::Track,LHCb::Track>* clone() const
    {return new DoShareM3();}
  };


  /* DeltaAngle
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

  //---------- extra utilities -----------------------------
  
  template <class T>
  zen::filter<T>* makeFilter(const zen::function<T>& fun,
                              const std::string& mode, 
                             double x0, double xf) {
    if (mode == "<") return (fun < x0).clone();
    else if (mode == ">") return (fun > x0).clone();
    else if (mode == "=") return (fun == x0).clone();
    else if (mode == "[]") return ((fun > x0) && (fun < xf)).clone();
    else if (mode == "||>") {
      zen::abs_function<T> afun(fun);
      return (afun > x0).clone();
    } else if ( mode == "||<") {
      zen::abs_function<T> afun(fun);
      return (afun < x0).clone();
    } else if (mode == "||[]") {
      zen::abs_function<T> afun(fun);
      return ((afun > x0) && (afun < xf)).clone();
    } 
    std::cout << " not mode " << mode << std::endl;
    return 0;
  }
  
};
#endif // HLTBASE_HLTOPER_H
