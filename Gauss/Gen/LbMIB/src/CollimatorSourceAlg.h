// $Id: CollimatorSourceAlg.h,v 1.1.1.1 2007-05-16 17:14:28 gcorti Exp $
#ifndef COLLIMATORSOURCEALG_H
#define COLLIMATORSOURCEALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

/** @class CollimatorSourceBaseAlg CollimatorSourceBaseAlg.h 
 *  
 *  Algorithm to read file containing particles reaching LHCb cavern
 *  and due to halo impinging on the Tertiary Collimators.
 *  The source can be re-weighted according to the options selected
 *  and produces particles from it.
 *
 *  @author Magnus Lieng
 *  @date   2006-10-10
 */
class CollimatorSourceAlg : public GaudiHistoAlg {
public:
  
  /// Standard constructor
  CollimatorSourceAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~CollimatorSourceAlg();; ///< Destructor
 
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /// Make Envelopes
  StatusCode createEnvelopes();

  /// Book Histograms
  StatusCode bookHistos();

  /// Get Momentum
  HepLorentzVector getMomentum(double ekin, int pid, double dx, double dy);

  /// Get Vertex
  HepLorentzVector getVertex(double ekin, int pid, double x, double y, 
                             double z, double dx, double dy);

  /// Create Binary File
  StatusCode createBinFile();

private:

  /// Locations
  std::string m_eventLoc;
  std::string m_headerLoc;
  std::string m_collLoc;

  /// The particle source file
  std::string m_pSourceFile;
  bool m_binaryFile;
  std::string m_tempFileName;

  /// Scaling factors
  double m_scalingFactor;
  double m_bunchFreq;
  double m_luminosity;

  /// Historgram generation
  bool m_genHist;

  /// Mode modifiers
  int m_pPerEvt;
  int m_fileOffset;
  bool m_saveBinFile;

  /// Random number generators
  Rndm::Numbers m_flatGenerator;
  Rndm::Numbers m_poissonGenerator;

  /// Envelope method variables
  unsigned int m_envelopeSize;
  std::vector<double> m_envelopeHolders;
  int m_counter;
  double m_sumOfWeights;

  /// Structure for full envelope version
  struct ParticleData {
    int pid;
    double weight;
    double ekin;
    double x;
    double y;
    double dx;
    double dy;
    double sumOfWeights;
  };

  /// Mark for direct read
  std::istream::pos_type m_mark;

  /// Z particle origin and direction
  double m_zOrigin;
  int m_dz;

  /// Event Type
  int m_evtType;

  /// Histograms
  AIDA::IHistogram2D* m_xyDistInput;
  AIDA::IHistogram1D* m_eKinInput;
  AIDA::IHistogram1D* m_logEKinInput;
  AIDA::IHistogram1D* m_thetaInput;

  AIDA::IHistogram2D* m_xyDistGen;
  AIDA::IHistogram2D* m_pxVSrGen;
  AIDA::IHistogram2D* m_pyVSrGen;
  AIDA::IHistogram2D* m_pzVSrGen;
  AIDA::IHistogram1D* m_absPGen;
  AIDA::IHistogram1D* m_thetaGen;

  AIDA::IHistogram2D* m_xyDistGenWeight;
  AIDA::IHistogram2D* m_pxVSrGenWeight;
  AIDA::IHistogram2D* m_pyVSrGenWeight;
  AIDA::IHistogram2D* m_pzVSrGenWeight;
  AIDA::IHistogram1D* m_absPGenWeight;
  AIDA::IHistogram1D* m_thetaGenWeight;

  /// Particle Property Service
  IParticlePropertySvc* m_ppSvc;

protected:

  /// Get Random Particle
  StatusCode getRandPart(ParticleData* target);

  /// Get Sequencial Particle
  StatusCode getPart(ParticleData* target);

};   

#endif  // COLLIMATORSOURCE_COLLIMATORSOURCEALG_H

