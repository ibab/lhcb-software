#ifndef TIMECORRSOURCE_H
#define TIMECORRSOURCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "Kernel/IParticlePropertySvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "IMIBSource.h"            // Interface

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "RooStringVar.h"

/** @class TimeCorrSource TimeCorrSource.h 
 *  
 *  Tool to read file containing particles reaching LHCb cavern
 *  due to various background sources.
 *  The source can be re-weighted according to the options selected
 *  and produces particles from it.
 *
 *  @author Magnus Lieng
 *  @date   2009-03-31
 */
class TimeCorrSource : public         GaudiHistoTool, 
                         virtual public IMIBSource    {
  
public:
  
  /// Standard constructor
  TimeCorrSource(const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~TimeCorrSource(); ///< Destructor
 
  virtual StatusCode initialize();    ///< Tool initialization
  virtual StatusCode finalize  ();    ///< Tool finalization

  virtual StatusCode generateEvent( LHCb::GenHeader* theHeader,
                                    LHCb::GenCollisions* theCollisions,
                                    LHCb::HepMCEvents* theEvents,
                                    int& numPart);

protected:

  /// Make particle
  StatusCode generateParticle( HepMC::GenEvent* evt, int i, int& numPart );
    
  /// Make Envelopes
  StatusCode createEnvelopes();

  /// Book Histograms
  StatusCode bookHistos();

  /// Get Momentum
  HepMC::FourVector getMomentum(double ekin, int pid, double dx, double dy);

  /// Get Vertex
  HepMC::FourVector getVertex(double ekin, int pid, double x, double y, 
                              double dx, double dy, double dt);

private:

  /// The particle source file
  std::string m_pSourceFile;

  /// Scaling factors
  double m_overrideSource;
  double m_scalingFactor;
  double m_protInFile;
  double m_bunchSize;
  double m_luminosity;
  double m_beamEnergy;

  /// Histogram generation
  bool m_genHist;

  /// Mode modifiers
  int m_pPerEvt;
  int m_fileOffset;

  /// Timing
  double m_timeOffset;

  /// Random number generators
  Rndm::Numbers m_flatGenerator;
  Rndm::Numbers m_poissonGenerator;

  /// Envelope method variables
  unsigned int m_envelopeSize;
  std::vector<double> m_envelopeHolders;
  int m_counter;
  double m_sumOfWeights;

  /// ROOT file and trees
  TFile* m_rootFile;
  TTree* m_evtTree;
  TTree* m_partTree;

  /// Structure for full envelope version
  struct ParticleData {
    Int_t pid;
    Double_t ekin;
    Double_t x;
    Double_t y;
    Double_t dx;
    Double_t dy;
    Double_t dt;
    Double_t weight;
    Double_t sumOfWeights;
  };
  
  double m_zOrigin;                  ///< Z particle origin in file
  double m_zGenOff;                  ///< Z particle offset from origin during generation
  double m_zGen;                     ///< Z particle origin generated
  int m_dz;                          ///< Z particle direction 

  /// Defaults
  int m_dzDef;
  double m_zOriginDef;
  double m_protInFileDef;
  double m_beamEnergyDef;
  
  /// Histograms
  AIDA::IHistogram2D* m_xyDistGen;
  AIDA::IHistogram2D* m_pxVSrGen;
  AIDA::IHistogram2D* m_pyVSrGen;
  AIDA::IHistogram2D* m_pzVSrGen;
  AIDA::IHistogram1D* m_absPGen;
  AIDA::IHistogram1D* m_thetaGen;
  AIDA::IHistogram1D* m_timeGen;


  LHCb::IParticlePropertySvc* m_ppSvc;   ///< Pointer to Particle Property Service

protected:

  /// Get Random Interaction
  StatusCode getRandInt( int &firstPart, int &nParts );

  /// Get Sequencial Interaction
  StatusCode getInt( int &firstPart, int &nParts );

  /// Get Sequencial Particle
  StatusCode getPart(ParticleData* target, int i, int &nPart);

};   

#endif  // TIMECORRSOURCE_TIMECORRSOURCEALG_H

