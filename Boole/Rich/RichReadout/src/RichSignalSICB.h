#ifndef RICHREADOUT_RICHSIGNALSICB_H
#define RICHREADOUT_RICHSIGNALSICB_H 1

#include <fstream>

// base class
#include "RichUtils/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/AlgFactory.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Vector/LorentzVector.h"

// dectector
#include "RiSicbGeom/SicbGeom.h"
#include "RiSicbGeom/PixelFinder.h"

// RichKernel
#include "RichKernel/RichDetectorType.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"

class RichSignalSICB : public RichAlgBase {

public:

  RichSignalSICB( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichSignalSICB();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private: // methods

  StatusCode ProcessEvent( std::string hitLoc, double tofOffset );

  double SimpleEnergy();

  bool InitParameters();

private: // data

  MCRichSummedDeposits* mcSummedDeposits;
  MCRichDeposits* mcDeposits;

  /// String containing MCRichDeposits location in TDS
  std::string m_RichHitLocation;
  std::string m_RichPrevLocation;
  std::string m_RichPrevPrevLocation;
  std::string m_RichNextLocation;
  std::string m_RichNextNextLocation;
  std::string m_RichSummedDepositLocation;
  std::string m_RichDepositLocation;

  typedef std::map<int, double> photonmap;
  typedef std::map<int, int> photonmap2;
  photonmap2 m_NPhotons;
  photonmap m_Angle;

  bool m_doChargedTracks;
  bool m_doSpillover;

  double x[19][500];
  double y[19][500];

  IPixelFinder* m_finder;
  Rndm::Numbers m_rndm;

};

#endif // RICHREADOUT_RICHSIGNALSICB_H
