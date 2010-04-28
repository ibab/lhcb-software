#ifndef VELOALIGNMENT_ALIGN_H 
#define VELOALIGNMENT_ALIGN_H 1

// Include files

#include "math.h"

//from DetDesc
#include "DetDesc/AlignmentCondition.h"
#include "DetDesc/ParamValidDataObject.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/FitNode.h"
#include "Event/RecVertex.h"
#include "Event/ODIN.h"
#include "Event/VeloCluster.h"
#include "Event/TrackTypes.h"

//#include "Event/TrackStateVertex.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "TrackInterfaces/IPVOfflineTool.h"

// from VeloAlignment
#include "ITrackStore.h"
#include "AlignmentInterfaces/IMillepede.h"
#include "MilleConfig.h"
#include "PVHisto.h"
#include "VeloTrack.h"
//from TrackInterfaces
#include "TrackInterfaces/IVeloClusterPosition.h"
#include "TrackInterfaces/IVeloExpectation.h"
#include "TrackInterfaces/IMeasurementProvider.h"

using namespace Gaudi::Units;
using namespace LHCb;
using namespace Gaudi;


/** @class VAlign VAlign.h VAlign/VAlign.h
 *  
 *
 *  @author Sebastien Viret
 *  @date   2005-07-28
 */

class ITrackStore;
class IMillepede;

class VAlign : public GaudiTupleAlg {
public: 
  /// Standard constructor
  VAlign( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VAlign( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode FindAlignment(MilleConfig* my_config); 

  StatusCode DefineVeloMap();   
  StatusCode GetAlignmentConstants();


  StatusCode fill_params(VeloTrack& my_track, int my_step);
  StatusCode fill_params2(VeloTrack& my_track, int my_step,
                          int nVeloHits,float adcpertrack);
  StatusCode fill_params3(VeloTrack& my_track, int my_step, 
                          int nExpectedHits, int nVeloHits,
                          float adcpertrack);
  StatusCode fill_overlaps(VeloTrack& my_track, int my_step);
  StatusCode fill_trackmoni(VeloTrack& my_track, int my_step);
  StatusCode fill_misalignments(std::vector<double> constants, 
                                std::vector<double> errors, 
                                std::vector<double> pulls, int my_step);
  StatusCode fill_infoevent(int event, int tracknumperev,int hitnumperevent,
                            int tralignnumperevent, int troverlapnumperevent);
  StatusCode fill_officialPV_Lin(int nt_ev,int nt_pvn, 
                                 int nt_trn, double nt_pvx,double nt_pvy,double nt_pvz
                                 ,double nt_chi2,
                                 int nt_trnl, double nt_pvxl,double nt_pvyl,double nt_pvzl,
                                 double nt_chi2l,
                                 int nt_trnr, double nt_pvxr,double nt_pvyr,double nt_pvzr,
                                 double nt_chi2r,
                                 int nt_trn_lin, double nt_pvx_lin,double nt_pvy_lin,double nt_pvz_lin,double nt_chi2_lin,
                                 int nt_trnl_lin, double nt_pvxl_lin,double nt_pvyl_lin,double nt_pvzl_lin,
                                 double nt_chi2l_lin,
                                 int nt_trnr_lin, double nt_pvxr_lin,double nt_pvyr_lin,double nt_pvzr_lin,
                                 double nt_chi2r_lin,
                                 double loc_pvxl, double loc_pvyl, double loc_pvzl,
                                 double loc_pvxr, double loc_pvyr, double loc_pvzr);
  


  inline std::string itos(int i)	// convert int to string
  {
    std::stringstream s;
    s << i;
    return s.str();
  }

  StatusCode updateConditions( std::vector<double> const_left, std::vector<double> const_right, 
                               std::vector<double> const_box, std::vector<double> err_left, 
                               std::vector<double> err_right);

protected:

private:
  const LHCb::RecVertices* pvcontainer;
  
  // TrackStore Variables

  MilleConfig       *my_Config;
  ITrackStore       *my_tracks;
  IMillepede        *my_align;
  PVHisto           *my_PV_finder;
  PVHisto           *my_PV_finderR;
  PVHisto           *my_PV_finderL;
  ToolHandle<ITrackVertexer> m_vertexer ;
  IVeloExpectation* m_expectTool;
  
  int m_runodin;
  int m_eventodin;
  int m_bunchid;
  
  VeloTracks    selected_tracks;   // Used for step one
  VeloTracks    control_tracks;    // Used for the control sample
  VeloTracks    overlap_tracks;    // Used for the overlap tracks
  VeloTracks    PV_tracks_corrected;         // Temporary container for tracks coming from a same vertex
  VeloTracks    PV_tracksR_corrected;         // Temporary container for tracks coming from a same vertex
  VeloTracks    PV_tracksL_corrected;         // Temporary container for tracks coming from a same vertex

  VeloTracks    PV_tracks_linfit;         // Temporary container for tracks coming from a same vertex
  VeloTracks    PV_tracksR_linfit;         // Temporary container for tracks coming from a same vertex
  VeloTracks    PV_tracksL_linfit;         // Temporary container for tracks coming from a same vertex


  VeloTracks    PV_tracks;         // Temporary container for tracks coming from a same vertex
  VeloTracks    PV_tracksL;         // Temporary container for tracks coming from a same vertex
  VeloTracks    PV_tracksR;         // Temporary container for tracks coming from a same vertex
  VeloTracks    PV_tracksnewleftright;         // Temporary container for tracks coming from a same vertex
  std::vector<VeloTracks> vector_PV_tracksL;
  std::vector<VeloTracks> vector_PV_tracksR;
  std::vector<VeloTracks> vector_PV_tracks;


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

  std::string m_trackContainerName;
  float  m_minadcpertrack;
  double m_maxtheta;
  
  bool                m_step1;
  std::vector<bool>   m_VELOmap_l;
  std::vector<bool>   m_VELOmap_r;
  std::vector<bool>   m_align;
  std::vector<double> m_sigma;
  std::vector<bool>   m_constrain;
  std::vector<double> m_residual_cut;

  bool                m_step2;
  bool                m_VELOopen;
  std::vector<bool>   m_alignb;
  std::vector<double> m_sigmab;
  std::vector<bool>   m_constrainb;
  std::vector<double> m_residual_cutb;
  double              m_z_range;
  unsigned int        m_PV_trackmin;
  double              m_z_sigma;
  double              m_IPmax;
  double              m_TrIPmax;

  double              m_starfactr;
  unsigned int        m_maxtrack;
  double              m_maxChi2;
  bool                m_moni_constants;   
  bool                m_moni_PV;  
  bool                m_moni_overlaps;  
  bool                m_moni_tracks; 
  bool                m_moni_events; 

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
  int ntotaleTracks;
  int ntotaleTracks2;
};
#endif // VELOALIGNMENT_ALIGN_H
