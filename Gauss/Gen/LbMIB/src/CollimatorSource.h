// $Id: CollimatorSource.h,v 1.3 2008-07-04 10:25:44 mlieng Exp $
#ifndef COLLIMATORSOURCE_H
#define COLLIMATORSOURCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "IMIBSource.h"            // Interface

/** @class CollimatorSource CollimatorSource.h 
 *  
 *  Tool to read file containing particles reaching LHCb cavern
 *  and due to halo impinging on the Tertiary Collimators.
 *  The source can be re-weighted according to the options selected
 *  and produces particles from it.
 *
 *  @author Magnus Lieng
 *  @date   2006-10-10
 */
class CollimatorSource : public         GaudiHistoTool, 
                         virtual public IMIBSource    {
  
public:
  
  /// Standard constructor
  CollimatorSource(const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~CollimatorSource(); ///< Destructor
 
  virtual StatusCode initialize();    ///< Tool initialization
  virtual StatusCode finalize  ();    ///< Tool finalization

  virtual StatusCode generateEvent( LHCb::GenHeader* theHeader,
                                    LHCb::GenCollisions* theCollisions,
                                    LHCb::HepMCEvents* theEvents,
                                    int& numParts);

protected:

  /// Make particles
  StatusCode generateParticle( HepMC::GenEvent* evt );
  
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

  /// The particle source file
  std::string m_pSourceFile;
  bool m_binaryFile;
  std::string m_tempFileName;

  /// Scaling factors
  double m_scalingFactor;
  double m_bunchFreq;
  double m_luminosity;

  /// Histogram generation
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
    long long pid;
    double weight;
    double ekin;
    double x;
    double y;
    double dx;
    double dy;
    double sumOfWeights;
  };

  
  std::istream::pos_type m_mark;     ///< Mark for direct read
  
  double m_zOrigin;                  ///< Z particle origin
  int m_dz;                          ///< Z particle direction 
  
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

  IParticlePropertySvc* m_ppSvc;   ///< Pointer to Particle Property Service

protected:

  /// Get Random Particle
  StatusCode getRandPart(ParticleData* target);

  /// Get Sequencial Particle
  StatusCode getPart(ParticleData* target);

};   

#endif  // COLLIMATORSOURCE_COLLIMATORSOURCEALG_H

