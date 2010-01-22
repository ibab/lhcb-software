#ifndef Hlt2DisplVerticesDEV_H
#define Hlt2DisplVerticesDEV_H 1
// Include files:
// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/VectorMap.h"

// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"

// Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"

// Beam line
#include "AIDA/IHistogram1D.h"
#include "TH1.h"

/** @class Hlt2DisplVerticesDEV Hlt2DisplVerticesDEV.h
 *
 * THIS IS A DEVELOPMENT VERSION
 * please refer to Hlt2DisplVertices
 *
 *  @author Neal Gauvin 
 *  @date   2009-july-1
 */

class Hlt2DisplVerticesDEV : public DVAlgorithm {
public:
  Hlt2DisplVerticesDEV(const std::string& name, ISvcLocator* pSvcLocator);
  // Destructor
  virtual ~Hlt2DisplVerticesDEV();
  // Algorithm initialization
  virtual StatusCode initialize();
  // Algorithm execution
  virtual StatusCode execute();
  // Algorithm finalization
  virtual StatusCode finalize();

private:
  StatusCode fillHeader( Tuple & );
  StatusCode Done( LHCb::Particle::ConstVector & );
  StatusCode SaveHidValSel( Tuple &, LHCb::RecVertices* );
  void GetPartsFromRecVtx(const LHCb::RecVertex*, 
			  LHCb::Particle::ConstVector & );
  void CreateMap();
  void Kinematics( LHCb::Particle::ConstVector &,double &,double &,double & );
  bool HasBackwardTracks( const LHCb::RecVertex* RV );
  bool RemVtxFromDet( const LHCb::RecVertex* );
  bool BeamLineCalibration();
  const LHCb::Particle* DefaultParticle( const LHCb::Track * p );
  Gaudi::XYZPoint GetCorrPosition( const LHCb::RecVertex*, 
                                   LHCb::RecVertex::ConstVector & );
  Gaudi::XYZPoint Plus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  Gaudi::XYZPoint Minus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  double RadDist( const Gaudi::XYZPoint & );

  //Check up
  void PrintTracksType();
  void PrintParticle();

  ///To sort PVs by closest one to a z reference point in z position.
  struct sortPVdz {
    double refz; 
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second ) { 

      return std::abs( first->position().z() - refz ) < 
	std::abs( second->position().z() - refz );
    }
  } SortPVdz;
  ///To sort PVs with ascending z position 
  struct sortPVz {
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second ) { 
      return first->position().z() < second->position().z();
    }
  } SortPVz;

  ITransportSvc * m_transSvc;
  IGeometryInfo* m_lhcbGeo;

  //Job options
  std::string m_InputDisplVertices;
  std::string m_RCutMethod;   
  // the type of R cut to be applied 
  // ""               : cut with respect to (0,0,z)
  // "LocalVeloFrame" : cut with respect to (0,0,z) in the local Velo frame
  // "FromUpstreamPV" : cut with respect to the upstream PV
  // "FromUpstreamPV3D" : cut with respect to the upstream PV from 
  //                      private PatPV3D
  // "CorrFromUpstreamPV" : cut with respect to the upstream PV 2D. 
  //                        Take the position of the associated 2D RV, if any.
  // "FromBeamLine" : compute the beam interaction axis and cut wrst to it.

  bool m_HidValSel;       //Save cuts from Hlt2HidValley
  bool m_Save;            // save info in tuples
  bool m_DefMom;        // work with default pion when no long tracks
  int m_MinNbtrks;        //min number of daughters tracks in a RecVertex
  double m_RMin1;          //min radial displacement 
  double m_RMin2;          //min radial displacement 
  double m_MinMass1;      //min reconstructed mass
  double m_MinMass2;      //min reconstructed mass
  double m_MinSumpt1;     //min sum of all daughters track
  double m_MinSumpt2;     //min sum of all daughters track
  double m_RemVtxFromDet ;// remove vtx reco in detector material
  // if = 0  : disabled
  // if < 0  : remove reco vtx if in detector material
  // if > 0  : remove reco vtx if rad length from decay pos - m_RemVtxFromDet 
  //           to decay pos + m_RemVtxFromDet along z is > threshold

  GaudiUtils::VectorMap<int, const LHCb::Particle *> m_map;

  int m_nbevent;
  int m_nbpassed;

  Gaudi::Transform3D m_toVeloFrame; ///< to transform to local velo frame

  //a pions
  const LHCb::Particle* Pion;
  double m_piMass;  ///< the pion mass
  double m_pt;      ///< default pt for default pions

  LHCb::Particle * m_BeamLine; ///<The beam line
  std::vector<double> m_BLInitPos; ///< Beam line initial position
  std::vector<double> m_BLInitDir; ///< Beam line initial direction
  int m_cycle; ///< compute beam position every m_cycle events

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

  int m_nbpv; // total number of PV considered
  int m_bin; ///< histo binning

  ///cuts on the PV / bounds on histograms
  double m_minz;
  double m_maxz;
  double m_maxx;
  double m_maxy;
  double m_maxchi;
  int m_mintrks;

};
#endif // Hlt2DisplVerticesDEV_H
