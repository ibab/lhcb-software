// $Id: L1VeloFPGA.h,v 1.2 2004-03-19 12:44:05 parkesb Exp $
#ifndef L1VELFPGA_H 
#define L1VELOFPGA_H 1
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "Event/MCVeloFE.h"

// Include files

class MCVeloFE;
class L1VeloFPGACluster;

#define NITER_FPGA   2
#define NSTRIPS_FPGA 32

/** @class L1VeloFPGA L1VeloFPGA.h 
 *  Contains the 32 strips of the VELO on which LCMS is performed. 
 *
 *  @author Niels Tuning
 *  @date   17/05/2002
 */
class L1VeloFPGA {
public:
  /// Standard constructor
  L1VeloFPGA( ); 

  ///
  L1VeloFPGA( int firstStrip, 
        std::vector<MCVeloFE*> groupOfMCVeloFE  ); 
  ///
  virtual ~L1VeloFPGA( ); ///< Destructor

  /// First strip of group of 32
  int firstStrip() { return m_firstStrip; };

  /// Number of strips in group of 32, with valid MCVeloFE 
  int nStrips() { return m_nStrips; };

  /// First MCVeloFE of group of 32
  MCVeloFE *const * fe() { return m_fe; };

  /// Fill array with raw charges
  void   setRawCharge( );

  /// Generate noise for empty strips
  void   setNoise( Rndm::Numbers& gaussDist );

  /// Get charge of strip 0 <= istrip <= 31
  double rawCharge( int istrip) {
    if (istrip >= 0 && istrip < NSTRIPS_FPGA){
      return m_rawCharge[istrip];
    } else {
      return 0.;
    }
  };

  /// Apply linear common mode correction on group of 32
  void   cmCorrection( double thresholdSigma, 
                       int doSlopeCor);

  /// Get corrected charge of strip 0 <= istrip <= 31 after iteration 0,1
  double corCharge( int istrip, int iter){
    // To get sensible results, first call FPGA.CMcorrection(); !!
    if (istrip >= 0 && istrip < NSTRIPS_FPGA &&  
        iter   >= 0 && iter   < NITER_FPGA      ) {
      return m_corCharge[istrip][iter];
    } else { 
      return 0.0;
    }
  };

  /// Outcome of common mode correction: 1 if the strip is above threshold
  int    detectHit(       int istrip, int iter) {
    // To get sensible results, first call FPGA.CMcorrection(); !!
    if (istrip >= 0 && istrip < NSTRIPS_FPGA &&   
        iter   >= 0 && iter   < NITER_FPGA     ) {
      return m_detectHit[istrip][iter];
    } else { 
      return 0;
    }
  };

  /// Strip number in sensor 0 <= istrip <= 31:   [0,2048]
  int    stripNr( int istrip) { return (m_firstStrip+istrip); };

  /// Station number [0,20]
  int    stationNr( ) { return int( ((*m_fe)->sensor())/4.); };

  /// Sensor number [0,83]
  int    sensorNr(  ) { return (*m_fe)->sensor(); };

  /// Mean of the 32 (- hits) charges after iteration 0,1
  double mean( int iter) {
    if (iter >= 0 && iter < NITER_FPGA){
      return m_mean[iter];
    } else {
      return 0.;
    }
  };

  /// Slope of the 32 (- hits) charges after iteration 0,1
  double slope( int iter) {
    if (iter >= 0 && iter < NITER_FPGA){
      return m_slope[iter];
    } else {
      return 0.;
    }
  };

  /// Variance of the 32 (- hits) charges after iteration 0,1
  double variance( int iter) {
    if (iter >= 0 && iter < NITER_FPGA){
      return m_variance[iter];
    } else {
      return 0.;
    }
  };
  
  /// make a new L1FPGACluster
  L1VeloFPGACluster makeCluster( std::vector<int> iDAQchannelsCluster, 
				 double totalCharge);

  /// Do clustering
  void   clustering(double thresholdSigma, 
                    double relThresholdNeighbour,
                    int maxClusSize);
  void   clustering(double thresholdSigma, 
                    double relThresholdNeighbour);

  /// Get the MCVeloFE of strip 0 <= istrip <= 31
  MCVeloFE* fe( int istrip);

  /// Get the vector with L1VeloFPGAClusters
  std::vector<L1VeloFPGACluster> FPGAClusters();


private:
  int m_firstStrip;
  int m_nStrips;
  MCVeloFE *const *  m_fe;
  std::vector<MCVeloFE*> m_groupOfMCVeloFE;
  double m_rawCharge[NSTRIPS_FPGA];
  double m_corCharge[NSTRIPS_FPGA][NITER_FPGA];
  int    m_detectHit[NSTRIPS_FPGA][NITER_FPGA];
  double m_mean[NITER_FPGA];
  double m_slope[NITER_FPGA];
  double m_variance[NITER_FPGA];
  std::vector<L1VeloFPGACluster>   m_FPGAClusters;

};
#endif // L1VELOFPGAVELO_H
