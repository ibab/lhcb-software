#ifndef VELOALIGNMENT_ALIGN_H 
#define VELOALIGNMENT_ALIGN_H 1

// Include files

#include "math.h"

//from DetDesc
#include "VeloDet/DeVelo.h"
#include "DetDesc/AlignmentCondition.h"
#include "DetDesc/ParamValidDataObject.h"

// Event
#include "Event/Track.h"

// from Gaudi

#include "GaudiAlg/GaudiTupleAlg.h"

// from VeloAlignment

#include "ITrackStore.h"
#include "AlignmentInterfaces/IMillepede.h"
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

class Align : public GaudiTupleAlg {
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
};
#endif // VELOALIGNMENT_ALIGN_H
