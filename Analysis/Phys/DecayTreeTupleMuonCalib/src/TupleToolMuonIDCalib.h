// $Id: TupleToolMuonVariables.h 
#ifndef TUPLETOOLMUONIDCALIB_H
#define TUPLETOOLMUONIDCALIB_H 1

// Include files
#include "Kernel/IParticleTupleTool.h"
#include "DecayTreeTupleBase/TupleToolBase.h"

#include "Event/MuonCoord.h"
#include "Kernel/LHCbID.h"
#include "Kernel/MuonTileID.h"
#include "Kernel/Particle2MCLinker.h"

/** @class TupleToolMuonIDCalib TupleToolMuonIDCalib.h
 *
 *  @author Fatima Soomro
 *  @date   2011-09-26
 */

using namespace LHCb;
using namespace Gaudi::Units;
using namespace Gaudi;

class DVAlgorithm;

class TupleToolMuonIDCalib : public TupleToolBase, virtual public IParticleTupleTool
{

public:

  /// Standard constructor
  TupleToolMuonIDCalib( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

  virtual ~TupleToolMuonIDCalib( ); ///< Destructor

  /// Loop over differnt conesizes and fill the variables into the tuple
  virtual StatusCode fill( const LHCb::Particle*
                           , const LHCb::Particle*
                           , const std::string&
                           , Tuples::Tuple& );

private:

  StatusCode fillVars( const LHCb::Particle *part, std::string, Tuples::Tuple& );
  void LoadMuonGeometry();
  StatusCode fillCoordVectors();
  double foiX(const int &station, const int &region, const double &p, const double &dx);
  double foiY(const int &station, const int &region, const double &p, const double &dy);

private:

  int             m_NFoi;         // look for hits winhin m_NFoi*Foi */
  std::string     m_particlesPath;

  /*   bool            m_DoHits;   // true: run the Hits code */
  /*   bool            m_DoMcHits;   // true: run the Hits code */
  /*   bool            m_DoSelectH;// true: store only closest hit */

  /*   double          m_MomentumCut;  // Minimum momentum cut */
  /*   double          m_IpsCut;       // Minimum momentum cut */
  /*   bool            m_DoAccCut;     // Do track within muon detector acc cut */
  /*   bool            m_DoHitCut;     // Do min hits in Nfoi/Nstation cut */
  /*   bool            m_DoMipCut;     // Do Mip cut          */

  /*   int             m_NHitsCut;     // minimum # of Hits within m_NFoi*FOI/station */
  /*                                   // if=-1 do the cut in NFoi as IsMuon  */
  /*   int             m_NStationsCut; // number of stations to require m_NHitsCut */
  /*                                   // if =-N require maximum N hits in M2 & in M3 */
  /*                                   // as well as IsMuon in nFoi */
  /*   double            m_EecalMax, m_EecalMin;  // Asymmetric Ecal Mip enregy window */
  /*   double            m_EhcalMax, m_EhcalMin;  // Asymmetric Hcal Mip enregy window */

  // FOIs parameters:
  // function that defines the field of interest size
  // formula is p(1) + p(2)*momentum+p(3)*exp(-p(4)*momentum)
  std::vector< double >     m_xfoiParam1;
  std::vector< double >     m_xfoiParam2;
  std::vector< double >     m_xfoiParam3;
  std::vector< double >     m_yfoiParam1;
  std::vector< double >     m_yfoiParam2;
  std::vector< double >     m_yfoiParam3;


  //--------------------------
  // Muon Detector variables:
  //--------------------------

  int m_NStation;       /// Number of stations
  int m_NRegion;        ///Number of regions
  //Names of the station
  std::vector<std::string> m_stationNames;
  // fill local arrays of pad sizes and region sizes
  DeMuonDetector*  m_mudet;
  // These are indexed [station]
  std::vector<double> m_stationZ; // station position
  double m_trackX[5];   // position of track in x(mm) in each station
  double m_trackY[5];   // position of track in y(mm) in each station

  std::vector<float> m_hitInFOIx;
  std::vector<float> m_hitInFOIdx;
  std::vector<float> m_hitInFOIy;
  std::vector<float> m_hitInFOIdy;
  std::vector<float> m_hitInFOIz;
  std::vector<float> m_hitInFOIdz;
  std::vector<float> m_hitInFOIID;
  bool m_doVerbose;

  // local array of region sizes
  std::vector<double> m_regionInnerX; // inner edge in abs(x)
  std::vector<double> m_regionOuterX; // outer edge in abs(x)
  std::vector<double> m_regionInnerY; // inner edge in abs(y)
  std::vector<double> m_regionOuterY; // outer edge in abs(y)

  // Store x,dx,y,dy of each coord
  class coordExtent_{
  public:
    coordExtent_(double x, double dx, double y, double dy,  double z, double dz,
                 LHCb::MuonCoord *pCoord) :
      m_x(x), m_dx(dx), m_y(y), m_dy(dy), m_z(z), m_dz(dz), m_pCoord(pCoord)  {};
    double m_x;
    double m_dx;
    double m_y;
    double m_dy;
    double m_z;
    double m_dz;
    LHCb::MuonCoord *m_pCoord;
  };
  // vector of positions of coords (innner vector coords,
  // outer is [station* m_NRegion + region ]
  std::vector<std::vector<coordExtent_> > m_coordPos;


};

#endif // TUPLETOOLMUONIDCALIB_H
