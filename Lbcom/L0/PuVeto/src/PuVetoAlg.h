// $Id: PuVetoAlg.h,v 1.13 2006-09-19 14:44:25 ocallot Exp $
#ifndef PUVETOALG_H 
#define PUVETOALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

// from LHCbKernel
#include "Kernel/VeloChannelID.h"

/** @class PuVetoAlg PuVetoAlg.h
 *  Computes the Veto algorithm, from MCVeloFEs
 *
 *  @author Marko Zupan
 *  @date   06/02/2006
 */

class PuVetoAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  PuVetoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PuVetoAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  /// fill the histogram
  void fillHisto(unsigned int hp[4][16]);

  /// return the bin number in the variable size histogram;
  short int zBin( double z ) {
    for (int i=0;i<m_nBins;i++) {
      if ((z>=m_binStart[i]) && (z<m_binStart[i]+m_binLength[i])) return i;
    }
    return -1;
  }
  
  // bit manipulation helper methods
  unsigned short int getBit (unsigned short int bt,unsigned int tgt)
  {
    return (unsigned short int) (tgt >> bt) & 1;
  }
  
  void setBit(unsigned short int bt, unsigned int &tgt,
              unsigned short int state=1) 
  {
    if (state) {
      unsigned int temp = 1 << bt;
      tgt = tgt | temp;
    }
    else {
      unsigned int temp = 0xFFFF ^ (1 << bt);
      tgt = tgt & temp;
    } 
  }
  
  unsigned int Saturate(unsigned int inval) 
  {
    if (inval > 255) return 255;
    else return inval;
  }
  

  // compute z-intersect of a PU "track" 
  double TrackZ (double rA,double zA,double rB,double zB) {
    return (zB*rA - zA*rB)/(rA - rB);
  }


  // returns the integral of the histogram
  // note that this is not used because the total multiplicity
  // is returned instead
  double fullIntegral( ) {
    double sum = 0;
    for ( unsigned int j=0 ; m_nBins > j ; j++ ) {
      sum += m_hist[j];
    }
    return sum;
  }

  /// Returns the position of the maximum, and as argument the peak height 
  /// and the integral.
  double findPeak1(unsigned short int &height, short int &bin);
  double findPeak2(unsigned short int &height, unsigned short int &sum, 
                   short int &bin);
  

  /// Mask the hits contributing to a peak at a given vertex position
  void maskHits( short int bin,short int mwbins);
  
  unsigned short int rawEncode(int sensor, int strip);
  void rawVec(std::vector<unsigned short int> *vecin,
              std::vector<unsigned int> *vecout);

private:

  // job option parameters
  std::string        m_inputContainer;
  std::string        m_outputContainer;
  double             m_threshold;
  unsigned short int m_lowThreshold;
  unsigned short int m_maskingWindow;
  std::string        m_binFile;

  

  // detector geometry variables
  unsigned int   m_nbPuSensor;
  double m_rCluster[128]; 
  double m_zSensor[4];
  DeVelo*        m_velo;

  // binning and masking matrices used in the FPGA
  // there are two matrices, one for the left and one for the right
  // side of the detector, the third dimension of the array is only
  // there to adress them
  short int m_binMatrix[2][128][128];
  short unsigned int m_maskMatrix[2][128][128];

  // bit patterns for hits before and after masking
  // there are four bit patterns, one for each
  // detector plane
  unsigned int m_hitPattern[4][16];
  unsigned int m_maskedPattern[4][16];
  
  // binning variables (maximum number of bins hard coded to 300)
  short unsigned int m_nBins;
  double m_minHistoZ;
  double m_maxHistoZ;
  // note that maxHistoZ is not used, 
  // the maximum Z depends on the width of the bins
  double m_binStart[300];
  double m_binCenter[300];
  double m_binLength[300];
  // the histogram
  unsigned short int m_hist[300];

  // total multiplicity counter
  unsigned int m_totMult;

};
#endif // PUVETOALG_H
