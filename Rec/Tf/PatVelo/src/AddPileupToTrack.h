#ifndef TF_ADDPILEUPTOTRACK_H 
#define TF_ADDPILEUPTOTRACK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

#include "CircularRangeUtils.h"
#include "Event/Track.h"
#include "Event/VeloLiteCluster.h"

/** @class AddPileupToTrack AddPileupToTrack.h 
 *
 *  @author David Hutchcroft
 *  @date   2007-06-15
 */
namespace Tf{
  class AddPileupToTrack : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    AddPileupToTrack( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~AddPileupToTrack( ); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute   (); ///< Algorithm execution
    virtual StatusCode finalize  (); ///< Algorithm finalization


  private:
    StatusCode loadPileupHits();
    void extendWithPUTrack(LHCb::Track &track);
    void addBestClus(const unsigned int &sensNum,LHCb::Track &track);

    DeVelo* m_velo;                        ///< pointer to DeVelo
    CircularRangeUtils<double> m_angleUtils; ///< compare phi ranges

    std::vector<double> m_XOffsetTop; ///< X left/right box offset at top
    std::vector<double> m_XOffsetBottom; ///< X left/right box offset 
    std::vector<double> m_YOffsetTop; ///< Y left/right box offset at top
    std::vector<double> m_YOffsetBottom; ///< Y left/right box offset 

    class clusInPUSector{
    public:
      inline void addSensor(const unsigned int &nSens){
	m_cont[nSens] = sensorCont();
      }
      inline void addCluster(const LHCb::VeloLiteCluster &vl, const double &r){
	m_cont[vl.channelID().sensor()].getVec(vl.channelID().strip()/512).
	  push_back(clusInfo(vl,r));
      }
      void clear(){
	for(auto &sensMap : m_cont){
	  for(auto &sensCont : sensMap.second.getVecs()){
	    sensCont.clear();
	  }
	}
      }
      inline bool empty(const unsigned int &nSens,const unsigned int &nSect){
	return m_cont[nSens].getVec(nSect).empty();
      }
      bool bestCluster(const unsigned int &nSens,
		       const unsigned int &nSect,
		       const double &predR,
		       const double &rWindow,
		       LHCb::VeloLiteCluster &bestCluster ){
	auto &vec = m_cont[nSens].getVec(nSect);
	auto lessComp = [](const clusInfo& a, const double& b) 
	  { return a.m_r < b; };
	auto posLow = std::lower_bound(vec.begin(),vec.end(),
				       predR-rWindow, lessComp);	
	auto posHigh = std::lower_bound(posLow,vec.end(),
					predR+rWindow, lessComp);
	if( posLow == posHigh ) return false;
	double dist = 1.e10;
	for(auto &p = posLow ; p != posHigh ; ++p){
	  double cDist = fabs(predR-p->m_r);
	  if( cDist < dist ){
	    dist = cDist;
	    bestCluster = p->m_vl;
	  }else{
	    break;
	  }
	}
	return true;
      }

      struct clusInfo{
	LHCb::VeloLiteCluster m_vl;
	double m_r;
	clusInfo(const LHCb::VeloLiteCluster &vl,
		 const double &r) : m_vl(vl), m_r(r){};
	clusInfo() : m_vl(), m_r(0.) {};
	inline void fill( const LHCb::VeloLiteCluster &vl,
			  const double &r ){
	  m_vl = vl; 
	  m_r = r;
	}
      };

      struct sensorCont{
	sensorCont(){
	  m_vec.resize(4);
	}
	inline std::vector< std::vector<clusInfo> >& getVecs(){
	  return m_vec;
	}
	inline std::vector<clusInfo>& getVec(const unsigned int &i){
	  return m_vec[i];
	}
      private:
	std::vector< std::vector<clusInfo> > m_vec;
      };

    private:
      std::map<unsigned int, sensorCont> m_cont;
    public:
      inline std::map<unsigned int, sensorCont> & getCont(){
	return m_cont;
      }
            
    };

    clusInPUSector m_clusCont; ///< place to store vl clusters per sensor, then sector
    std::vector<unsigned int> m_PULeft; ///< indexes of left side PU sensors
    std::vector<unsigned int> m_PURight; ///< indexes of right side PU sensors
  
    std::vector<double> m_radius;
    std::map< unsigned int, std::vector<std::pair<double,double> > > m_phiRange;
    std::map< unsigned int, double> m_zSens;
    double m_zStartState;
    std::map< unsigned int , bool > m_isLeft;

    std::string m_extendTracksLocation;

    double m_errScale;
    double m_RWindow;

    bool m_isDebug;           ///< is debug output level
    bool m_isVerbose;         ///< is verbose output level 

  };
}
#endif // TF_ADDPILEUPTOTRACK_H 
