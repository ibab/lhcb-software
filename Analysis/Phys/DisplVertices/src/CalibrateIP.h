// $Id: $
#ifndef CALIBRATEIP_H 
#define CALIBRATEIP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackInterfaces/ITrackVertexer.h"

#include "Event/Particle.h"

#include "AIDA/IHistogram1D.h"

// from ROOT
#include "TH1.h"
#include "TF1.h"

//a class to hold beam line infos
#include "BeamLineInfo.h"

/** @class CalibrateIP CalibrateIP.h
 *  
 * Compute the beam line position and store infos
 * THIS IS INTENDED FOR DEBUGGING/TESTING AND OFFLINE MC STUDIES 
 * DO NOT USE ONLINE
 *
 *  @author Neal Gauvin
 *  @date   2009-09-17
 */


class CalibrateIP : public GaudiHistoAlg {
public: 
  /// Standard constructor
  CalibrateIP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CalibrateIP( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  void SaveBeamLine(); ///< Save the beam line in TES
  void GetLongs( const LHCb::RecVertex*, std::vector<const LHCb::Track*> & );
  void SmearPV( Gaudi::XYZPoint & );
  

  ToolHandle<ITrackVertexer> m_vertexer ;

  std::string m_PVContainerName; ///< where the PVs are located
  std::string m_outloc;          ///< where the beam line is saved on TES

  bool m_refit; ///< refit the PV with long tracks only
  bool m_local; ///< work in local Velo frame
  bool m_histo; ///< compute IP from fitting histograms
  bool m_smear; ///< smear vertex artificially

  Gaudi::Transform3D m_toVeloFrame; ///< to transform to local velo frame
  Gaudi::Transform3D m_toGlobalFrame; ///< to transform from local velo frame


  ///AIDA histograms
  AIDA::IHistogram1D*  m_x;
  AIDA::IHistogram1D*  m_y;
  AIDA::IHistogram1D*  m_z;
  AIDA::IHistogram1D*  m_xx;
  AIDA::IHistogram1D*  m_xy;
  AIDA::IHistogram1D*  m_xz;
  AIDA::IHistogram1D*  m_yy;
  AIDA::IHistogram1D*  m_yz;
  AIDA::IHistogram1D*  m_zz;

  int m_nbevent;
  int m_nbpv; // total number of pv considered
  int m_bin; ///< histo binning
  double m_beamstab; ///< threshold to consider beam stable

  ///cuts on the pv / bounds on histograms
  double m_minz;
  double m_maxz;
  double m_maxx;
  double m_maxy;
  double m_maxchi;
  int m_minlongs;
  int m_mintrks;

  BeamLineInfo * m_Beam;           ///<The beam line
  std::vector<double> m_BLInitPos; ///< Beam line initial position
  std::vector<double> m_BLInitDir; ///< Beam line initial direction
  int m_cycle; ///< compute beam position every m_cycle events

};


#endif // CALIBRATEIP_H
