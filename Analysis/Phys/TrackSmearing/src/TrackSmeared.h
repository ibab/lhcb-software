// $Id: $
#ifndef TRACKSMEARED_H 
#define TRACKSMEARED_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"
#include "TrackInterfaces/ITrackFitter.h"
#include <TF1.h>
#include <TRandom3.h>
/** @class TrackSmeared TrackSmeared.h
 *  
 *
 *  @author Sascha Stahl
 *  @date   2010-08-27
 */
class TrackSmeared : public DaVinciAlgorithm {
public: 
  /// Standard constructor
  TrackSmeared( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackSmeared( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  void smearStates(  const std::vector< LHCb::State * > & states );
  ROOT::Math::Plane3D::Vector Ip_vector(const LHCb::State *state, const LHCb::VertexBase* vtx );
    
protected:
  
private:
  std::string m_trackLocation;
  std::string m_outputLocation;
  std::string m_protoLocation;
  std::string m_settings;
  
  
  double getSigma( LHCb::State* state,  std::vector<TF1*> funcs_data, std::vector<TF1*> funcs_mc);
  double getSigmax(LHCb::State* state);
  double getSigmay(LHCb::State* state);
  
  StatusCode smearProto();
  StatusCode smearBest();
  StatusCode smearCopied();
  ITrackFitter* m_trackFitter;
  TF1* m_funcx_data;
  TF1* m_funcx_mc;  
  TF1* m_funcy_data;
  TF1* m_funcy_mc;
  std::vector<TF1*> m_funcsx_mc;
  std::vector<TF1*> m_funcsy_mc;
  std::vector<TF1*> m_funcsx_data;
  std::vector<TF1*> m_funcsy_data;
  std::vector< std::pair<double,double> > m_paramsx_data;
  std::vector< std::pair<double,double> > m_paramsx_mc;
  std::vector< std::pair<double,double> > m_paramsy_data;
  std::vector< std::pair<double,double> > m_paramsy_mc;
  unsigned int m_nPhiBins;
  double m_xpar1_mc;
  double m_xpar2_mc;
  double m_xpar1_data;
  double m_xpar2_data;
  double m_ypar1_mc;
  double m_ypar2_mc;
  double m_ypar1_data;
  double m_ypar2_data;
  double m_seed;
  double m_scale;
  bool m_smear;
  bool m_plots;
  bool m_usePhi;
  bool m_smearBest;
  bool m_smearProto;
  bool m_smearCopied;
  TRandom3* m_rnd;
 
  
    

};
#endif // TRACKSMEARED_H
