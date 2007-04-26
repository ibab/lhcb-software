#ifndef VELOALIGNMENT_ALIGN_H 
#define VELOALIGNMENT_ALIGN_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

#include "MilleConfig.h"
#include "PVHisto.h"
#include "VeloTrack.h"

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
  StatusCode GetAlignmentConstants();


  StatusCode bookNTuple();
  StatusCode writeNtuple(std::string ntupleName); 
  StatusCode fill_params(VeloTrack& my_track, int my_step);
  StatusCode fill_overlaps(VeloTrack& my_track, int my_step);
  StatusCode fill_misalignments(std::vector<double> constants, std::vector<double> errors, 
				std::vector<double> pulls, int my_step);
  StatusCode fill_primary(VeloTracks& aPV, int PV_numb);

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
  PVHisto           *my_PV_finder;


  VeloTracks    selected_tracks;   // Used for step one
  VeloTracks    control_tracks;    // Used for the control sample
  VeloTracks    overlap_tracks;    // Used for the overlap tracks
  VeloTracks    PV_tracks;         // Temporary container for tracks coming from a same vertex

  std::vector<double> misal_left;
  std::vector<double> misal_right;
  std::vector<double> misal_box;

  std::vector<double> error_left;
  std::vector<double> error_right;
  std::vector<double> error_box;

  std::vector<double> pulls_left;
  std::vector<double> pulls_right;
  std::vector<double> pulls_box;

  std::vector<double> misal_init_left;
  std::vector<double> misal_init_right;
  std::vector<double> misal_init_box;


  bool                m_step1;
  std::vector<bool>   m_VELOmap_l;
  std::vector<bool>   m_VELOmap_r;
  std::vector<bool>   m_align;
  std::vector<double> m_sigma;
  std::vector<bool>   m_constrain;
  std::vector<double> m_residual_cut;

  bool                m_step2;
  bool                m_VELOopen;
  int                 m_trackmin;
  std::vector<bool>   m_alignb;
  std::vector<double> m_sigmab;
  std::vector<bool>   m_constrainb;
  std::vector<double> m_residual_cutb;
  double              m_z_range;
  int                 m_PV_trackmin;
  double              m_z_sigma;
  double              m_IPmax;
  double              m_TrIPmax;

  double              m_starfactr;
  int                 m_maxtrack;

  bool                m_moni_constants;   
  bool                m_moni_PV;  
  bool                m_moni_overlaps;  
  bool                m_moni_tracks; 

  int                 m_nTrackSample;

  std::string         my_TrackStore;
  std::string         my_Millepede;

  std::vector<double> mis_const;
  std::vector<double> mis_error;
  std::vector<double> mis_pull;

  // VELO geometry info

  DeVelo* m_velo;

  const DeVeloRType* rDet;
  const DeVeloPhiType* phiDet;

  double VELOmap[42];
  double zmoy_R, zmoy_L, szmoy_R, szmoy_L;

  //track info
  int nEvents;
  int nTrackSample;

  int nLeft_tracks;
  int nRight_tracks;

 /// Pointer to N-tuple

  NTuple::Item<double>              n_typeA;
  NTuple::Item<double>              n_stationA;
  NTuple::Item<double>              n_dof;
  NTuple::Item<double>              n_const;
  NTuple::Item<double>              n_error;
  NTuple::Item<double>              n_pull;

  NTuple::Item<double>              n_eventV;
  NTuple::Item<double>              n_vertex;
  NTuple::Item<double>              n_PVtracks;
  NTuple::Item<double>              n_PVx;
  NTuple::Item<double>              n_PVy;
  NTuple::Item<double>              n_PVz;

  NTuple::Item<double>              n_event;
  NTuple::Item<double>              n_track;
  NTuple::Item<double>              n_type;
  NTuple::Item<double>              n_x;
  NTuple::Item<double>              n_y;
  NTuple::Item<double>              n_z;
  NTuple::Item<double>              n_station;

  NTuple::Item<double>              n_step;
  NTuple::Item<double>              n_side;
  NTuple::Item<double>              n_vx;
  NTuple::Item<double>              n_vy;
  NTuple::Item<double>              n_vz;
  NTuple::Item<double>              n_stationB;
  NTuple::Item<double>              n_X;
  NTuple::Item<double>              n_Y;
  NTuple::Item<double>              n_resX;
  NTuple::Item<double>              n_resY;

};
#endif // VELOALIGNMENT_ALIGN_H
