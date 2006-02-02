#ifndef VELOALIGNMENT_ALIGN_H 
#define VELOALIGNMENT_ALIGN_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

#include "TrackInterfaces/IMeasurementProvider.h"

#include "MilleConfig.h"

/** @class Align Align.h Align/Align.h
 *  
 *
 *  @author Sebastien Viret
 *  @date   2005-07-28
 */

class ITrackStore;
class IMillepede;

class Align : public GaudiAlgorithm {
public: 
  /// Standard constructor
  Align( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Align( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode FindAlignment(MilleConfig* my_config); 

  StatusCode DefineVeloMap();   

  StatusCode bookNTuple();
  StatusCode writeNtuple(std::string ntupleName); 
  
  StatusCode GetAlignmentConstants(MilleConfig* my_config, double misalignments[]);
  StatusCode UpdateAlignmentConstants();

  inline std::string itos(int i)	// convert int to string
  {
    std::stringstream s;
    s << i;
    return s.str();
  }

protected:

private:

  // TrackStore Variables

  MilleConfig       *my_Config;
  ITrackStore       *my_tracks;
  IMillepede        *my_align;

  AlignTracks    *selected_tracks;
  AlignTracks    *PV_tracks;

  IMeasurementProvider* m_measProvider;

  double  m_z_min;
  double  m_z_max;
  int     m_PV_trackmin;
  double  m_z_sigma;
  double  m_IPmax;
  double  m_TrIPmax;

  bool m_halo;

  int  m_nTracks;
  bool m_iteration;
  double m_starfactr;
  double m_residual_cut_init;
  double m_residual_cut;
  bool m_align[6];
  bool m_constrain[9];
  double m_sigma[6];
  bool m_alignb[6];
  double m_sigmab[6];

  std::string my_TrackStore;
  std::string my_Millepede;

  std::vector<double> mis_const;
  std::vector<double> mis_error;
  std::vector<double> mis_pull;

  // 

  DeVelo* m_velo;

  //track info
  int nEvents;
  int nPV;

  double VELOmap[42];

 /// Pointer to N-tuple

  NTuple::Item<double>              n_typeA;
  NTuple::Item<double>              n_stationA;
  NTuple::Item<double>              n_dof;
  NTuple::Item<double>              n_const;
  NTuple::Item<double>              n_error;
  NTuple::Item<double>              n_pull;

  NTuple::Item<double>              n_res_r;
  NTuple::Item<double>              n_res_phi;

  NTuple::Item<double>              n_eventV;
  NTuple::Item<double>              n_vertex;
  NTuple::Item<double>              n_PVtracks;
  NTuple::Item<double>              n_PVx;
  NTuple::Item<double>              n_PVy;
  NTuple::Item<double>              n_PVz;

};
#endif // VELOALIGNMENT_ALIGN_H
