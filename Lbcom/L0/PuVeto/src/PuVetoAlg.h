// $Id: PuVetoAlg.h,v 1.9 2004-03-25 08:39:48 cattanem Exp $
#ifndef PUVETOALG_H 
#define PUVETOALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

class VetoInput {
public:
  VetoInput( double zSensor )  { m_zSensor = zSensor;  };
  virtual ~VetoInput( ) { };
  double zSensor()               const { return m_zSensor; };

  void addStrip( int strip )        { m_strip.push_back( strip ); };
  
  std::vector<int>* strips( )          { return &m_strip;            };
  
private:
  double m_zSensor;
  std::vector<int> m_strip;
};


/** @class PuVetoAlg PuVetoAlg.h
 *  Computes the Veto algorithm, from the PuVetoDigits
 *
 *  @author Olivier Callot
 *  @date   11/01/2002
 */
class PuVetoAlg : public Algorithm {
public:
  /// Standard constructor
  PuVetoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PuVetoAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  /// fill the histogram
  void fillHisto( );

  /// return the bin number in the variable size histogram;
  long    zBin( double z ) {
    int bin = -1;
    if ( m_lowBound[0] < z ) {
      for ( int j=0 ;(m_histoBins - 1) > j ; j++ ) {
        if ( m_lowBound[j+1] > z ) {
          bin = j; 
          break;
        }
      }
    }
    return bin;
  }
  
  /// returns the integral of the histogram
  double fullIntegral( ) {
    double sum = 0;
    for ( unsigned int j=0 ; m_hist.size() > j ; j++ ) {
      sum += m_hist[j];
    }
    return sum;
  }

  /// Returns the position of the maximum, and as argument the peak height 
  /// and the integral.
  double peakValue( double& height, double& sum, double& width ) ;

  /// Mask the hits contributing to a peak at a given vertex position
  void maskHits( double zVertex, double zTol );
  
  unsigned short int rawEncode(int sensor, int strip);
  void rawVec(std::vector<unsigned short int> *vecin,
              std::vector<raw_int> *vecout);
  


private:
  std::string    m_inputContainer;
  std::string    m_outputContainer;
  double         m_threshold;
  double         m_lowThreshold;
  double         m_highThreshold;
  double         m_highPosition;
  double         m_secondPosition;

  int            m_binningScenario;
  int            m_histoBins;
  double         m_bin2Constant;
  double         m_bin2Slope;
  double         m_maskingWindow;
  
  DeVelo*        m_velo;

  std::vector< VetoInput > m_input;
  
  // Storage for the variable bound histogram

  std::vector<double> m_lowBound;
  std::vector<double> m_step;
  std::vector<int>    m_nBin;
  int                 m_totBin;
  std::vector<double> m_hist;
  std::vector<double> m_binCenter;

};
#endif // PUVETOALG_H
