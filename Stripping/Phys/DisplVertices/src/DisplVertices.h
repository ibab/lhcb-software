// $Id: $
#ifndef DISPLVERTICES_H
#define DISPLVERTICES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"

//To decode L0DU raw infos
//#include "L0Interfaces/IL0DUFromRawTool.h"

//Hlt tools
//#include "Kernel/ITriggerTisTos.h"

// Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"

/** @class DisplVertices DisplVertices.h
 *
 *  @brief Apply a set of cuts on given Particles.
 *  @author Neal Gauvin
 *  @date  30 june 2009
 */

class DisplVertices : public DaVinciTupleAlgorithm
{

private:

  //A list is a sequence container that support rapide insertion or deletion
  //at any position but does not support random access.
  typedef std::vector<LHCb::Particle> VecPart;
  typedef std::vector<LHCb::Vertex> VecVert;

public:

  /// Standard constructor
  DisplVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DisplVertices( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  //ITriggerTisTos* m_tisTos;
  //IL0DUFromRawTool* m_l0BankDecoder;
  ITransportSvc * m_transSvc;
  IGeometryInfo* m_lhcbGeo;

  //questions
  bool IsAPointInDet( const LHCb::Particle *, int mode = 2,
                      double range = 1*Gaudi::Units::mm );
  bool IsAPointInDet( const Gaudi::XYZPoint & );
  /// Is a point in the RF-Foil ?
  bool IsInRFFoil( const Gaudi::XYZPoint & );
  /// Has a RecVertex a backward track ?
  bool HasBackwardTracks( const LHCb::Particle * );
  bool HasBackwardTracks( const LHCb::RecVertex * );
  /// Has a RecVertex a backward and a forward track ?
  bool HasBackAndForwardTracks( const LHCb::RecVertex* );
  /// Has a candidate a daughter muon ?
  double HasMuons( const LHCb::Particle * );

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
  double Angle( const Gaudi::LorentzVector &, const Gaudi::LorentzVector & );
  double Angle( const Gaudi::XYZVector &, const Gaudi::XYZVector & );
  double ImpactParam( const Gaudi::XYZPoint &, const Gaudi::LorentzVector &,
                      const Gaudi::XYZPoint & );
  double ImpactParam( const Gaudi::XYZPoint &, const Gaudi::XYZVector&,
                      const Gaudi::XYZPoint &);

  unsigned int GetNbVeloTracks();
  double GetSumPt( const LHCb::Particle * );
  void   GetQPt( const LHCb::Vertex * v, double & sumpt, double & sumq,
                 bool weight = false );
  double GetCharge( const LHCb::Vertex * v, bool weight = false );
  double GetRecMass( const LHCb::Particle::ConstVector & );
  double GetRecMass( const LHCb::Particle::ConstVector &, double & );
  double GetPt( const LHCb::RecVertex* RV );
  double GetPt( const Gaudi::LorentzVector &, const Gaudi::LorentzVector & );
  double GetDeltaPhi( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  double GetDeltaR( const LHCb::Particle *, const LHCb::Particle * );
  double GetR( const Gaudi::XYZPoint & );
  double GetRFromBL( const Gaudi::XYZPoint& );
  void GetMassFromLongTracks( const LHCb::Particle *, double &, int & );
  void GetUpstreamPV(); ///< Get the Upstream PV
  void GetPVs(); ///< Get the PV candidates
  void GetPartsFromRecVtx(const LHCb::RecVertex*,
                          const LHCb::Particle::ConstVector &,
                          LHCb::Particle::ConstVector & );
  StatusCode SaveCaloInfos( Tuple & );
  StatusCode GetCaloInfos( std::string, double &, double & );
  StatusCode GetCaloClusterInfos( std::string, Tuple &, double &, double & );
  // StatusCode SaveTrigInfinTuple( Tuple & );
  StatusCode SaveL0RawInfos( Tuple & );
  StatusCode SavePVs( Tuple & );
  StatusCode fillHeader( Tuple & );
  StatusCode BackToBack( LHCb::Particle::ConstVector & );

  /***************************************************************//**
                                                                    * the type of radial (R) cut to be applied
                                                                    * ""                   : cut with respect to (0,0,z)
                                                                    * "FromUpstreamPV"     : cut with respect to the upstream PV (PV3D)
                                                                    * "FromBeamLine"       : cut with respect to given beam line
                                                                    ******************************************************************/
  std::string m_RCut;
  std::string m_BLLoc;        ///< Location in TES of Beam line
  LHCb::Particle * m_BeamLine;
  bool   m_SaveTuple;         ///<Save prey properties in a tuple
  bool   m_SaveTrigInfos ;    ///<Save trigger infos in a tuple
  bool   m_SaveonTES;         ///<Save Event on TES if fires
  int    m_nEvents ;          // number of events
  const double pi ;
  // cuts

  double m_PreyMinMass ;      ///< Minimum reconstructed mass
  double m_PreyMinMass2 ;      ///< Minimum reconstructed mass
  double m_PreyMaxMass ;      ///< Maximum reconstructed mass
  double m_PreyMaxSumPt ;     ///< Maximum sumpt
  double m_SumPt ;            ///< Sumpt of all daughters tracks
  double m_DistMax;           ///< Max distance of the vertices
  double m_RMin;              ///< Min dist to the z axis
  double m_RMax;              ///< Max dist to the z axis
  double m_MaxChi2OvNDoF;     ///< Max chi2 of a vertex
  double m_PurityMin;         ///< Min purity requested, MC set on
  double m_MuonpT;            ///< Muon with a pT of min m_MuonpT
  unsigned int m_NbCands;     ///< Min nb of desired candidates
  unsigned int m_PVnbtrks;    ///< Min nb of trks for upstream PV candidate
  int    m_nTracks ;          ///< Min # of tracks at reconstructed vertex
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
  ///<Remove vtx if found in RF-Foil area, based on geometric cuts
  bool   m_RemFromRFFoil;
  double m_Backtoback;

  Gaudi::Transform3D m_toVeloLFrame; ///< to transform to local velo L frame
  Gaudi::Transform3D m_toVeloRFrame; ///< to transform to local velo R frame

  const LHCb::RecVertex * PV; //The Primary Vertex !
  std::vector<const LHCb::RecVertex*> PVs; ///< The PV candidates

  std::string m_Prey ;        ///< LHCb Name of the prey
  LHCb::ParticleID m_PreyID;  // PDG ID of this particle
  int    m_PreyPID ;          ///< PID of the prey (for MC and Gen use)
  std::string m_MotherPrey ;  ///< LHCb Name of the mother of the prey
  LHCb::ParticleID m_MotherPreyID;  // PDG ID of this particle

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

#endif // DISPLVERTICES_H
