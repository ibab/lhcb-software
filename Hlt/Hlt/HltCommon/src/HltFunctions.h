// $Id: HltFunctions.h,v 1.32 2010-04-21 13:56:22 gligorov Exp $
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

#include "boost/foreach.hpp"
#include "HltTypes.h"
#include "HltBase/HltUtils.h"
#include "TrackInterfaces/IFunctionTool.h"
#include "HltBase/IBiFunctionTool.h"
#include "Event/Node.h"
#include "Event/TrackFunctor.h"
#include "GaudiAlg/GaudiTool.h"

#include <Event/ProtoParticle.h>
#include <Event/Particle.h>
#include <Kernel/ParticleID.h>
#include <LoKi/ParticleCuts.h>
#include <Event/State.h>

#include "GaudiKernel/Vector4DTypes.h"

#include "EFunctions.h"
#include <memory>
#include <math.h>

namespace Hlt {  

  
  /* SmartFunction:    
   *   returns the info of the object given a key, if not apply the function
   */
  template <class T>
  class SmartFunction : public zen::function<T> 
  {
  public:
    typedef zen::function<T> Function;
    explicit SmartFunction(int ikey, const Function& f):m_key(ikey) , m_fun(f.clone())
    {}
    virtual ~SmartFunction() {}
    double operator() (const T& t) const {
        return ( t.hasInfo(m_key) ? t.info(m_key,-1e6) : (*m_fun)(t) );
    }
    SmartFunction* clone() const {return new SmartFunction(m_key,*m_fun);}
  private:
    int m_key;
    std::auto_ptr<Function> m_fun;
  };
  
  /* Info:    
   *   returns the info of the object given a key
   *   ie. if T is a track. track.info(keyPT) return the IP of the track
   */
  template <class T>
  class Info : public zen::function<T> 
  {
  public:
    explicit Info(int ikey) : key(ikey) { }
    double operator() (const T& t) const 
    {return t.info(key,-1);}
    Info<T>* clone() const {return new Info<T>(key);}
    int key; 
  };

  
  /* PT:    
   *   returns the Pt of a track
   */
  class PT : public Hlt::TrackFunction {
  public:
    explicit PT() {}
    double operator() (const LHCb::Track& t) const {return t.pt();}
    PT* clone() const {return new PT();}
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
    TrackFlag* clone() const {return new TrackFlag(flag);}
    LHCb::Track::Flags flag;      
  };

  /* Check a VELO tracks number of missed hits
   * returns a double with the number Expected - found hits (could be negative)
   */
  class MissedVeloHits : public Hlt::TrackFunction{
  public:
    explicit MissedVeloHits(){};
    double operator()( const LHCb::Track& t ) const{
      // should cut on LHCb::Track::nPRVelo3DExpect, 
      // however HLT overwrites the 3D info with the RZ info 
      // so if that is available double it then use that
      if( t.hasInfo(LHCb::Track::nPRVelo3DExpect) ){	
	return (t.info(LHCb::Track::nPRVelo3DExpect,-1.))
	  - static_cast<double>(t.nLHCbIDs());
      }else if( t.hasInfo(LHCb::Track::nPRVeloRZExpect) ){
	return (2.*t.info(LHCb::Track::nPRVeloRZExpect,-1.))
	  - static_cast<double>(t.nLHCbIDs());
      }else{
	return -1.;
      }
    }
    MissedVeloHits* clone() const { return new MissedVeloHits(); }
  };

  class NumberOfASideVeloHits : public Hlt::TrackFunction {
  public:
    explicit NumberOfASideVeloHits(){}
    double operator()(const LHCb::Track& t) const{
      int nLeft=0;
      int nRight=0;
      for( std::vector<LHCb::LHCbID>::const_iterator iId = t.lhcbIDs().begin(); t.lhcbIDs().end() != iId; ++iId ) {
        unsigned int sensor = (*iId).veloID().sensor();
        if(0==sensor%2) nLeft++;
        if(1==sensor%2) nRight++;
      }
      return (double) nLeft;
    }
    NumberOfASideVeloHits* clone() const {return new NumberOfASideVeloHits();}
  };

  class NumberOfCSideVeloHits : public Hlt::TrackFunction {
  public:
    explicit NumberOfCSideVeloHits(){}
    double operator()(const LHCb::Track& t) const{
      int nLeft=0;
      int nRight=0;
      for( std::vector<LHCb::LHCbID>::const_iterator iId = t.lhcbIDs().begin(); t.lhcbIDs().end() != iId; ++iId ) {
        unsigned int sensor = (*iId).veloID().sensor();
        if(0==sensor%2) nLeft++;
        if(1==sensor%2) nRight++;
      }
      return (double) nRight;
    }
    NumberOfCSideVeloHits* clone() const {return new NumberOfCSideVeloHits();}
  };
  
  class Charge : public Hlt::TrackFunction {
  public:
    explicit Charge() {}
    double operator() (const LHCb::Track& t) const {return t.charge();}
    Charge* clone() const {return new Charge();}
  };
  
  class P : public Hlt::TrackFunction {
  public:
    explicit P() {}
    double operator() (const LHCb::Track& t) const {return t.p();}
    P* clone() const {return new P();}
  };
  

  class ETCalo : public Hlt::TrackFunction
  {
  public:
    explicit ETCalo() {}
    double operator() (const LHCb::Track& t) const;    
    ETCalo* clone() const {return new ETCalo();}
  };

  class DeltaE : public Hlt::TrackFunction
  {
  public:
    explicit DeltaE() {}
    double operator() (const LHCb::Track& t) const;    
    DeltaE* clone() const {return new DeltaE();}
  };
  
  class DeltaP : public Hlt::TrackBiFunction
  {
  public:
    explicit DeltaP() {}
    double operator() (const LHCb::Track& t1, const LHCb::Track& t2) const;    
    DeltaP* clone() const {return new DeltaP();}
  };

  class VertexMinPT : public Hlt::VertexFunction {
  public:
    explicit VertexMinPT() {}
    double operator() (const LHCb::RecVertex& v) const {
      return HltUtils::VertexMinPT(v);}
    VertexMinPT* clone() const {return new VertexMinPT();}
  };

  class VertexMaxPT : public Hlt::VertexFunction {
  public:
    explicit VertexMaxPT() {}
    double operator() (const LHCb::RecVertex& v) const {
      return HltUtils::VertexMaxPT(v);}
    VertexMaxPT* clone() const {return new VertexMaxPT();}
  };

  class VertexMinP : public Hlt::VertexFunction {
  public:
    explicit VertexMinP() {}
    double operator() (const LHCb::RecVertex& v) const {
      return HltUtils::VertexMinP(v);}
    VertexMinP* clone() const {return new VertexMinP();}
  };

  class VertexMaxP : public Hlt::VertexFunction {
  public:
    explicit VertexMaxP() {}
    double operator() (const LHCb::RecVertex& v) const {
      return HltUtils::VertexMaxP(v);}
    VertexMaxP* clone() const {return new VertexMaxP();}
  };

  class VertexTracksMatchIDsFraction : public Hlt::VertexFunction {
  public:
    explicit VertexTracksMatchIDsFraction() {}
    double operator() (const LHCb::RecVertex& v) const {
        //The order matters! 
        return HltUtils::matchIDsFraction(*(v.tracks()[1]),*(v.tracks()[0]));
    } 
    VertexTracksMatchIDsFraction* clone() const {return new VertexTracksMatchIDsFraction();}
  };

  class VertexMatchIDsFraction : public Hlt::VertexBiFunction {
  public:
    explicit VertexMatchIDsFraction() {}
    double operator() (const LHCb::RecVertex& v,
                       const LHCb::RecVertex& vref
                       ) const {
      return HltUtils::vertexMatchIDsFraction(vref,v);
      
    }
    VertexMatchIDsFraction* clone() const
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
    VertexMinIP* clone() const {return new VertexMinIP();}
  };

  class VertexTrack1IP: public Hlt::VertexBiFunction {
  public:
    explicit VertexTrack1IP(){}
    double operator() (const LHCb::RecVertex& v,
                       const LHCb::RecVertex& pv) const{
      const LHCb::Track& track1 = *(v.tracks()[0]);
      return HltUtils::impactParameter(pv,track1);
    }
    VertexTrack1IP* clone() const {return new VertexTrack1IP();}
  };

  class VertexTrack2IP: public Hlt::VertexBiFunction {
  public:
    explicit VertexTrack2IP(){}
    double operator() (const LHCb::RecVertex& v,
                       const LHCb::RecVertex& pv) const{
      const LHCb::Track& track2 = *(v.tracks()[1]);
      return HltUtils::impactParameter(pv,track2);
    }
    VertexTrack2IP* clone() const {return new VertexTrack2IP();}
  };
  
  class VertexMaxChi2OverNdf: public Hlt::VertexFunction {
  public:
    explicit VertexMaxChi2OverNdf(){}
    double operator() (const LHCb::RecVertex& v) const 
    {
      double chi2_0 = (v.tracks()[0])->chi2PerDoF();
      double chi2_1 = (v.tracks()[1])->chi2PerDoF();
      return (fabs(chi2_0)>fabs(chi2_1)) ? chi2_0 : chi2_1;
    }
    VertexMaxChi2OverNdf* clone() const {return new VertexMaxChi2OverNdf();}
  };

  class VertexTrack1Chi2OverNdf: public Hlt::VertexFunction {
  public:
    explicit VertexTrack1Chi2OverNdf(){}
    double operator() (const LHCb::RecVertex& v) const 
    {
      return v.tracks()[0]->chi2PerDoF();
    }
    VertexTrack1Chi2OverNdf* clone() const {return new VertexTrack1Chi2OverNdf();}
  };

  class VertexTrack2Chi2OverNdf: public Hlt::VertexFunction {
  public:
    explicit VertexTrack2Chi2OverNdf(){}
    double operator() (const LHCb::RecVertex& v) const 
    {
      return v.tracks()[1]->chi2PerDoF();
    }
    VertexTrack2Chi2OverNdf* clone() const {return new VertexTrack2Chi2OverNdf();}
  };

  template <class T, class ITOOL>
  class FunctionTool : public zen::function<T> {
  public:
    FunctionTool(const std::string& toolname, GaudiTool *parent) 
           : _tool( parent->tool<ITOOL>(toolname,parent) ) 
    { }
    explicit FunctionTool(ITOOL* tool) : _tool(tool) 
    { if (!_tool) throw GaudiException(" null tool pointer","",StatusCode::FAILURE ); }
    double operator() (const T& t1) const {
      return _tool->function(t1);
    }
    zen::function<T>* clone() const {
      return new Hlt::FunctionTool<T,ITOOL>(_tool);
    }
  private:
    ITOOL* _tool;
  };

  template <class T, class T2, class ITOOL>
  class BiFunctionTool : public zen::bifunction<T,T2> {
  public:
    BiFunctionTool(const std::string& toolname, GaudiTool *parent) 
           : _tool( parent->tool<ITOOL>(toolname,parent) )
    { }
    explicit BiFunctionTool(ITOOL* tool) :_tool(tool) {
      if (!_tool) throw GaudiException(" null tool pointer","",StatusCode::FAILURE );
    }
    double operator() (const T& t1, const T2& t2) const {
      double value = _tool->function(t1,t2);return value;
    }
    zen::bifunction<T,T2>* clone() const {
      return new Hlt::BiFunctionTool<T,T2,ITOOL>(_tool);
    }
  private:
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
    rIP* clone() const {return new rIP();}
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
    IP* clone() const {return new IP();}
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
    DZ* clone() const {return new DZ();}
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
    FC* clone() const {return new FC();}
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
    DOCA* clone() const {return new DOCA();}
  };

  class DimuonMass : public Hlt::TrackBiFunction {
  public:
    explicit DimuonMass() {}
    double operator() (const LHCb::Track& track1, 
                       const LHCb::Track& track2) const {
      return HltUtils::invariantMass(track1,track2,
                                     105.658369,105.658369);
    }
    DimuonMass* clone() const {return new DimuonMass();}
  };

  class DikaonMass : public Hlt::TrackBiFunction {
  public:
    explicit DikaonMass() {}
    double operator() (const LHCb::Track& track1, 
                       const LHCb::Track& track2) const {
      return HltUtils::invariantMass(track1,track2,
                                     493.677,493.677);
    }
    DikaonMass* clone() const {return new DikaonMass();}
  };

  
  class VertexDOCA : public Hlt::VertexFunction {
  public:
    explicit VertexDOCA() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      const LHCb::Track& t1 = *(vertex.tracks()[0]);
      const LHCb::Track& t2 = *(vertex.tracks()[1]);
      return HltUtils::closestDistanceMod(t1,t2);
    }
    VertexDOCA* clone() const {return new VertexDOCA();}
  };

  class VertexAngle : public Hlt::VertexFunction {
  public:
    explicit VertexAngle() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      const LHCb::Track& t1 = *(vertex.tracks()[0]);
      const LHCb::Track& t2 = *(vertex.tracks()[1]);
      return HltUtils::deltaAngle(t1,t2);
    }
    VertexAngle* clone() const {return new VertexAngle();}
  };



  class VertexDimuonMass : public Hlt::VertexFunction {
  public:
    explicit VertexDimuonMass() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      const LHCb::Track& t1 = *(vertex.tracks()[0]);
      const LHCb::Track& t2 = *(vertex.tracks()[1]);
      return Hlt::DimuonMass()(t1,t2);
    }
    VertexDimuonMass* clone() const {return new VertexDimuonMass();}
  };

  class VertexDikaonMass : public Hlt::VertexFunction {
  public:
    explicit VertexDikaonMass() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      const LHCb::Track& t1 = *(vertex.tracks()[0]);
      const LHCb::Track& t2 = *(vertex.tracks()[1]);
      return Hlt::DikaonMass()(t1,t2);
    }
    VertexDikaonMass* clone() const {return new VertexDikaonMass();}
  };

  class SumPT : public Hlt::TrackBiFunction {
  public:
    explicit SumPT() {}
    double operator() (const LHCb::Track& track1,
                       const LHCb::Track& track2) const {
      return track1.pt()+track2.pt();
    }
    SumPT* clone() const {return new SumPT();}
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
    VertexSumPT* clone() const {return new VertexSumPT();}
  };
  
  class VertexTrack1PT : public Hlt::VertexFunction {
  public:
    explicit VertexTrack1PT() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      return vertex.tracks()[0]->pt();
    }
    VertexTrack1PT* clone() const {return new VertexTrack1PT();}
  };

  class VertexTrack2PT : public Hlt::VertexFunction {
  public:
    explicit VertexTrack2PT() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      return vertex.tracks()[1]->pt();
    }
    VertexTrack2PT* clone() const {return new VertexTrack2PT();}
  };

  class VertexZPosition : public Hlt::VertexFunction {
  public:
    explicit VertexZPosition() {}
   
    double operator() (const LHCb::RecVertex& vertex) const {
      return vertex.position().z();
    }
    VertexZPosition* clone() const {return new VertexZPosition();}
  };

  class VertexTransversePosition : public Hlt::VertexFunction {
  public:
    explicit VertexTransversePosition() {}
   
    double operator() (const LHCb::RecVertex& vertex) const {
      return std::sqrt(vertex.position().x()*vertex.position().x()
		       +vertex.position().y()*vertex.position().y());
    }
    VertexTransversePosition* clone() const {return new VertexTransversePosition();}
  };

  ///* Return the number of tracks of a vertex
  class VertexNumberOfASideTracks : public Hlt::VertexFunction {
  public:
    explicit VertexNumberOfASideTracks() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      int nLeft=0;
      int nRight=0;
      int nOverlap=0;
      const SmartRefVector<LHCb::Track>& tracks = vertex.tracks();
      for (unsigned int it = 0; it < tracks.size(); it++ ) {
        unsigned int count=0;
        const LHCb::Track* track =  tracks[it];
        for( std::vector<LHCb::LHCbID>::const_iterator iId = track->lhcbIDs().begin();
             track->lhcbIDs().end() != iId; ++iId ) {
          unsigned int sensor = (*iId).veloID().sensor();
          count+=sensor%2;
        }
        if(count==0){
          nLeft++;
        }else if(count==track->lhcbIDs().size()){
          nRight++;
        }else{
          nOverlap++;
        }
      }
      return double(nLeft);
    }
    VertexNumberOfASideTracks* clone() const {return new VertexNumberOfASideTracks();}
  };

  ///* Return the number of tracks of a vertex
  class VertexNumberOfCSideTracks : public Hlt::VertexFunction {
  public:
    explicit VertexNumberOfCSideTracks() {}
    double operator() (const LHCb::RecVertex& vertex) const {
      int nLeft=0;
      int nRight=0;
      int nOverlap=0;
      const SmartRefVector<LHCb::Track>& tracks = vertex.tracks();
      for (unsigned int it = 0; it < tracks.size(); it++ ) {
        unsigned int count=0;
        const LHCb::Track* track =  tracks[it];
        for( std::vector<LHCb::LHCbID>::const_iterator iId = track->lhcbIDs().begin();
             track->lhcbIDs().end() != iId; ++iId ) {
          unsigned int sensor = (*iId).veloID().sensor();
          count+=sensor%2;
        }
        if(count==0){
          nLeft++;
        }else if(count==track->lhcbIDs().size()){
          nRight++;
        }else{
          nOverlap++;
        }
      }
      return double(nRight);
    }
    VertexNumberOfCSideTracks* clone() const {return new VertexNumberOfCSideTracks();}
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
    MatchIDsFraction* clone() const {return new MatchIDsFraction();}
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
    DoShareM3* clone() const {return new DoShareM3();}
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
    DeltaAngle* clone() const {return new DeltaAngle();}
  };  
  
  /* FitChi2OverNdf:    
   *   returns the Chi2OverNdf of a fitted track
   */
  class FitChi2OverNdf : public Hlt::TrackFunction {
  public:
    explicit FitChi2OverNdf() {}
    double operator() (const LHCb::Track& t) const {return t.chi2PerDoF();}
    FitChi2OverNdf* clone() const {return new FitChi2OverNdf();}
  };

  /* FitMuChi2:    
   *   returns the Chi2 contribution of the muon hits of a fitted track
   */
  class FitMuChi2 : public Hlt::TrackFunction {
  public:
    explicit FitMuChi2() {}
    double operator() (const LHCb::Track& t) const {
      double muChi2=0;
      BOOST_FOREACH( const LHCb::Node* it, t.nodes() ) {
        if( it->hasMeasurement()
            && LHCb::Measurement::Muon == it->measurement().type()  ) muChi2 += it->chi2();
      }
      return muChi2;
    }
    FitMuChi2* clone() const {return new FitMuChi2();}
  };
  
  /* FitCleanedChi2OverNdf:    
   *   returns the Chi2OverNdf of a fitted track, the contribution of muon hits
   *   is removed
   */
  struct isTypeMeasurement {
    isTypeMeasurement( LHCb::Measurement::Type type ) : type_(type) {}  
    bool operator()(const LHCb::Measurement* m) { return m->type()==type_ ; }
    LHCb::Measurement::Type type_;
  };


  class FitCleanedChi2OverNdf : public Hlt::TrackFunction {
  public:
    explicit FitCleanedChi2OverNdf() {}
    double operator() (const LHCb::Track& t) const {
      int nMuonHits = TrackFunctor::nMeasurements( t, isTypeMeasurement( LHCb::Measurement::Muon ) );
      double chi2=0;
      if(0!=(t.nDoF()-nMuonHits)) {
        chi2=t.chi2()-Hlt::FitMuChi2()(t)/double(t.nDoF()-nMuonHits);
      }
      return chi2;
    }
    FitCleanedChi2OverNdf* clone() const {return new FitCleanedChi2OverNdf();}
  };
  
  class VertexMaxMuChi2: public Hlt::VertexFunction {
  public:
    explicit VertexMaxMuChi2(){}
    double operator() (const LHCb::RecVertex& v) const 
    {
      double chi2_0 = Hlt::FitMuChi2()(*(v.tracks()[0]));
      double chi2_1 = Hlt::FitMuChi2()(*(v.tracks()[1]));
      return (fabs(chi2_0)>fabs(chi2_1)) ? chi2_0 : chi2_1;
    }
    VertexMaxMuChi2* clone() const {return new VertexMaxMuChi2();}
  };


  ///* return the helicity angle cos(theta*) of the daughters in the mother rest frame (from LoKi)
  class CosThetaStar : public Hlt::TrackBiFunction {
  public:
    explicit CosThetaStar(){}
    double operator() (const LHCb::Track& track1,
                       const LHCb::Track& track2) const {
      
      const double mass = 493.677;
      LHCb::ProtoParticle protoP1; protoP1.setTrack(&track1);
      LHCb::ProtoParticle protoP2; protoP2.setTrack(&track2);
      LHCb::ParticleID    pidKPlus(321);
      LHCb::ParticleID    pidKMinus(-321);
      LHCb::ParticleID    pidBs(531);      //for this purpose the choice of particle ID doesn't matter
      
      LHCb::Particle p1(pidKPlus);  p1.setMeasuredMass(mass); p1.setMeasuredMassErr(0); 
      LHCb::Particle p2(pidKMinus); p2.setMeasuredMass(mass); p2.setMeasuredMassErr(0);
      
      if (track1.charge()>0) {
        p1.setProto(&protoP1);
        p2.setProto(&protoP2);
      } else {
        p1.setProto(&protoP2);
        p2.setProto(&protoP1);        
      } // if charge

      // the next part is taken from "Particle2State" - but don't do the errors
      const double e1 = std::sqrt( track1.momentum().mag2()+mass*mass );
      const double e2 = std::sqrt( track2.momentum().mag2()+mass*mass );
      p1.setMomentum( Gaudi::XYZTVector(track1.momentum().X(),track1.momentum().Y(),track1.momentum().Z(),e1) ) ;
      p2.setMomentum( Gaudi::XYZTVector(track2.momentum().X(),track2.momentum().Y(),track2.momentum().Z(),e2) ) ;


      //std::cout << "track1p " << track1.momentum() << "proto " << protoP1.track()->momentum() << " particle " << p1.momentum() << std::endl;
     
      LHCb::Particle mother(pidBs);
      mother.addToDaughters(&p1);
      mother.addToDaughters(&p2);
      mother.setMeasuredMass(HltUtils::invariantMass(track1,track2, mass, mass));
      mother.setMeasuredMassErr(0);
      mother.setMomentum(p1.momentum() + p2.momentum());
      
      //std::cout << "mother p" << mother.momentum() << std::endl;
      const double cosTheta = LoKi::Cuts::LV01(&mother);

      if (!isnan(cosTheta)){
        // sometimes get NAN?
        return fabs(cosTheta);
      } else {
        return -999;
      }
    } // double ()
    CosThetaStar* clone() const {return new CosThetaStar();}
  }; // class CosThetaStar

  //---------- extra utilities -----------------------------
  
  template <class T>
  zen::filter<T>* makeFilter(const zen::function<T>& fun,
                             const std::string& mode, 
                             double x0, double xf) {
    if      (mode == "<")    return (fun < x0).clone();
    else if (mode == ">")    return (fun > x0).clone();
    else if (mode == "=")    return (fun == x0).clone();
    else if (mode == "[]")   return ((fun > x0) && (fun < xf)).clone();
    else if (mode == "||>")  return (zen::abs_function<T>(fun) > x0).clone();
    else if (mode == "||<")  return (zen::abs_function<T>(fun) < x0).clone();
    else if (mode == "||[]") {
      zen::abs_function<T> afun(fun);
      return ((afun > x0) && (afun < xf)).clone();
    } 
    std::cout << " not mode " << mode << std::endl;
    return 0;
  }
  
};
#endif // HLTBASE_HLTOPER_H
