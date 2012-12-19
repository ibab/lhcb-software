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

// histogramming 
#include "TFile.h"
#include "TROOT.h"
#include <TH1.h>

/** @class PuVetoAlg PuVetoAlg.h
 *  Computes the PileUp trigger algorithm emulator
 *
 *  @author Marko Zupan, Serena Oggero
 *
 *
 */
 
namespace AIDA {
  class IHistogram2D;
}
 
class TFile;

class PuVetoAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  PuVetoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PuVetoAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();  
  
//protected:
  /// fill the histogram
  void fillHisto(unsigned int hp[4][16]);

  /// count the number of hits for the multiplicity 
  void measureMult(unsigned int hp[4][16]);

  /// fill in PileUp hit map
  void fillPUmap( int wordIt, int word_Tot, unsigned int* data_Ptr, int step, unsigned int PU_hitmap[4][16] );
  
  /// reverse bits in PUhitmap
  void reverseWords( unsigned int PU_hitmap[4][16] );
  
  /// tool    
  std::string binary ( unsigned int );
  
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

protected:

private:

  // job option parameters
  std::string        m_inputContainer;
  std::string        m_outputContainer;
  // double             m_threshold;
  unsigned short int m_lowThreshold;
  unsigned short int m_maskingWindow;
  std::string        m_binFile;
  int		     m_evtNum;
  std::string	     m_rawEventLoc;

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
  unsigned int m_PUhitmap[4][16];
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
  TFile*        m_OutputFile;
  std::string   m_OutputFileName;
  //  bool  m_enablePlots;
  TH1D* m_PUvertices;
  TH1D* m_PUvertex1Pos;
  TH1D* m_PUvertex2Pos;
  TH1D* m_PUvertex1Height;
  TH1D* m_PUvertex2Height;
  TH1D* m_multiplicity;
};
#endif // PuVetoAlg_H
