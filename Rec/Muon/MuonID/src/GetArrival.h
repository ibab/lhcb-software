#ifndef GETARRIVAL_H 
#define GETARRIVAL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IGetArrival.h"            // Interface
#include "Interpolator.h"

/** @class GetArrival GetArrival.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-02
 */


class GetArrival : public GaudiTool, virtual public IGetArrival {
public: 
  /// Standard constructor
  GetArrival( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~GetArrival( ); ///< Destructor

  StatusCode initialize();
  
  StatusCode getStationsFromTrack(const LHCb::Track& mutrack, std::vector<int>& sts, const bool opt = true);
  
  double findProbAllStations(std::vector<int> sts, const double mom);
  
  double findProbStation(const int st, const double mom);
  
  std::vector<int> getStationsFromTrack(const LHCb::Track& mutrack);
  
  StatusCode getArrivalFromTrack(const LHCb::Track& mutrack, double& parr);
  
  bool stInStations(const int myst,const std::vector<int>& stations);
  
  double probTypeSt(const double p, const std::vector<int>& type_st);
  
  int countArray(const std::vector<int>& type_st, const int up_to, const int el);
  
  double probTypeStStation(const double p,const std::vector<int>& type_st,const int station);
  
  StatusCode clArrival(const LHCb::Track& muTrack, double& clarr);

  StatusCode clArrival(const double p,const std::vector<int>& type_st, double& clarr);

  StatusCode finalize() {m_init.ignore(); return StatusCode::SUCCESS;} //tool finalization

protected:

  bool m_removeSmallest;
  bool m_useFunct;
  
  StatusCode m_init;
  std::vector<double> m_alpha;
  std::vector<double> m_beta;
  std::vector<double> m_probs;
  std::vector<double> m_moms;
  double m_eff;
  int m_minhits;
  
  std::vector< std::vector<double> > m_vprobs;
  
  std::vector<Uniformer> m_functprobs;
  
  std::vector<std::vector<int> > m_types_st;
  
private:

};
#endif // CLTOOL_H
