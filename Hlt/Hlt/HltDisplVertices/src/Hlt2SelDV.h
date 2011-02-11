// $Id: Hlt2SelDV.h,v 1.4 2010-08-21 09:24:44 ngueissa Exp $
#ifndef HLT2SELDV_H 
#define HLT2SELDV_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

// Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"

#include <VeloDet/DeVeloSensor.h>
#include <VeloDet/DeVeloRType.h>
#include "VeloDet/DeVelo.h"


/** @class Hlt2SelDV Hlt2SelDV.h
 *  
 *  @brief Apply a set of cuts on given Particles.
 *  @author Neal Gauvin
 *  @date   12 janvier 2010
 */
class Hlt2SelDV : public DVAlgorithm {
public: 
  /// Standard constructor
  Hlt2SelDV( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Hlt2SelDV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  ITransportSvc * m_transSvc;
  IGeometryInfo* m_lhcbGeo;


  //questions
  bool IsAPointInDet( const LHCb::Particle *, int mode = 2,
                      double range = 1*Gaudi::Units::mm );
  /// Is a point in the RF-Foil ?
  bool IsInRFFoil( const Gaudi::XYZPoint & );
  /// Has a RecVertex a backward and a forward track ?
  bool HasBackAndForwardTracks( const LHCb::RecVertex* );
  /// Has a candidate a daughter muon ?
  double HasMuons( const LHCb::Particle * );
  double GetSumPt( const LHCb::Particle * );
  double GetRFromBL( const Gaudi::XYZPoint& );
  void GetUpstreamPV(); ///< Get the Upstream PV
  StatusCode SaveGEC( Tuple &,  LHCb::Particle::ConstVector & );
  StatusCode fillHeader( Tuple & );
  StatusCode SaveCaloInfos( Tuple & );
  StatusCode GetCaloInfos( std::string, double &, double & );

  //Geometric tools
  double Mult( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  double Mult( const Gaudi::LorentzVector &, const Gaudi::LorentzVector & );
  Gaudi::XYZPoint Plus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  Gaudi::XYZPoint Minus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  double Norm( const Gaudi::LorentzVector &);
  double Norm( const Gaudi::XYZPoint &);
  double Norm( const Gaudi::XYZVector &);
  Gaudi::XYZPoint Normed( const Gaudi::LorentzVector &, double range = 1 );
  double VertDistance( const Gaudi::XYZPoint &, const Gaudi::XYZPoint &);

  
  void InitialiseGeoInfo();///< Store geometry infos
  bool IsInMaterialBoxLeft(const Gaudi::XYZPoint &);///<Point in material region in Left halfbox
  bool IsInMaterialBoxRight(const Gaudi::XYZPoint &);///<Point in material region in Right halfbox

  /***************************************************************//**
   * the type of R cut to be applied 
   * ""                   : cut with respect to (0,0,z)
   * "FromUpstreamPV"     : cut with respect to the upstream PV (PV3D)
   * "FromBeamLine"       : cut with respect to given beam line
   ******************************************************************/
  std::string m_RCut;         
  std::string m_BLLoc;        ///< Location in TES of Beam line
  LHCb::Particle * m_BeamLine;
  bool   m_SaveTuple;         ///<Save prey properties in a tuple
  bool   m_SaveonTES;         ///<Save Event on TES if fires


  int m_nbevent;
  int m_nbpassed;

  // cuts
  unsigned int m_NbCands;     ///< Min nb of desired candidates
  unsigned int m_PVnbtrks;    ///< Min nb of trks for upstream PV candidate
  int    m_nTracks ;          ///< Min # of tracks at reconstructed vertex
  double m_PreyMinMass ;      ///< Minimum reconstructed mass
  double m_PreyMaxMass ;      ///< Maximum reconstructed mass
  double m_SumPt ;            ///< Sumpt of all daughters tracks
  double m_PreyMaxSumPt ;      ///< Maximum reconstructed mass
  double m_RMin;              ///< Min dist to the z axis
  double m_RMax;              ///< Max dist to the z axis
  double m_MaxChi2OvNDoF;     ///< Max chi2 of a vertex
  double m_MuonpT;            ///< Muon with a pT of min m_MuonpT
  double m_MinRecpt;          ///< Min measured pT
  double m_SigmaZ;            ///< Max on error Z
  double m_SigmaR;            ///< Max on error R
  double m_MinX;              ///< Min X position     
  double m_MaxX;              ///< Max X position     
  double m_MinY;              ///< Min Y position     
  double m_MaxY;              ///< Max Y position     
  double m_MinZ;              ///< Min Z position     
  double m_MaxZ;              ///< Max Z position     
  /***************************************************************//**
   * Remove vtx if in detector material ?
   * if = 0  : disabled
   * if = 1  : remove reco vtx if in detector material
   * if = 2  : remove reco vtx if rad length from decay pos - DetDist 
   *           to decay pos + DetDist along momentum is > threshold
   * if = 3 : remove reco vtx if rad length along 
   *                             +- DetDist * PositionCovMatrix
   * if = 4 : 3 but range+3 if in RF foil.
   ******************************************************************/
  int m_RemVtxFromDet ;    
  double m_DetDist;           ///< Min distance to det material 
  //Remove vtx if found in RF-Foil area, based on geometric cuts
  bool   m_RemFromRFFoil;


  Gaudi::Transform3D m_toVeloLFrame; ///< to transform to local velo L frame
  Gaudi::Transform3D m_toVeloRFrame; ///< to transform to local velo R frame
  std::vector<Gaudi::XYZPoint > m_LeftSensorsCenter;
  std::vector<Gaudi::XYZPoint > m_RightSensorsCenter;
  /// set to true when geometry is already initialised
  bool        m_GeoInit;


  const LHCb::RecVertex * PV; //The Primary Vertex !
  std::string m_Prey ;        ///< LHCb Name of the prey
  LHCb::ParticleID m_PreyID;  // PDG ID of this particle
  int    m_PreyPID ;          ///< PID of the prey (for MC and Gen use)

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

};

#endif // HLT2SELDVTUPLE_H
