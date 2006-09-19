// $Id: PuVetoFillRawBuffer.h,v 1.1 2006-09-19 14:44:25 ocallot Exp $
#ifndef PUVETOFILLRAWBUFFER_H 
#define PUVETOFILLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

// from LHCbKernel
#include "Kernel/VeloChannelID.h"


/** @class PuVetoFillRawBuffer PuVetoFillRawBuffer.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2006-09-19
 */
class PuVetoFillRawBuffer : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PuVetoFillRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PuVetoFillRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  unsigned short int rawEncode(int sensor, int strip) {
    return strip + (sensor << 14);
  }

  // bit manipulation helper methods
  unsigned short int getBit (unsigned short int bt,unsigned int tgt) {
    return (unsigned short int) (tgt >> bt) & 1;
  }

  void PuVetoFillRawBuffer::rawVec ( std::vector<unsigned short int> *vecin,
                                     std::vector<unsigned int> *vecout);

private:
  // job option parameters
  std::string        m_inputContainer;
  double             m_threshold;
  unsigned short int m_lowThreshold;
  unsigned short int m_maskingWindow;
  std::string        m_binFile;

  

  // detector geometry variables
  unsigned int   m_nbPuSensor;
  unsigned int m_firstPuSensor;
  DeVelo*        m_velo;
};
#endif // PUVETOFILLRAWBUFFER_H
