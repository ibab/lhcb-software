#ifndef RICHREADOUT_RICHHPDSIGNALSICB_H
#define RICHREADOUT_RICHHPDSIGNALSICB_H 1

#include <fstream>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "RiSicbGeom/SicbGeom.h"
#include "RiSicbGeom/PixelFinder.h"

// RichKernel
#include "RichKernel/RichDetectorType.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"

class RichHPDSignalSICB : public Algorithm {

public:

  RichHPDSignalSICB(const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RichHPDSignalSICB();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  StatusCode ProcessEvent( std::string hitLoc, double tofOffset );
  double SimpleEnergy();

  bool InitParameters();

  MCRichSummedDeposits* mcSummedDeposits;
  MCRichDeposits* mcDeposits;

  /// String containing MCRichDeposits location in TDS
  std::string m_RichHitLocation;
  std::string m_RichPrevLocation;
  std::string m_RichPrevPrevLocation;
  std::string m_RichNextLocation;
  std::string m_RichSummedDepositLocation;
  std::string m_RichDepositLocation;

  typedef std::map<int, double> photonmap;
  typedef std::map<int, int> photonmap2;
  photonmap2 m_NPhotons;
  photonmap m_Angle;

  double x[19][500];
  double y[19][500];

  double m_BunchSpace;

  IPixelFinder* m_finder;
  Rndm::Numbers m_rndm;

};

#endif // RICHREADOUT_RICHHPDSIGNALSICB_H
