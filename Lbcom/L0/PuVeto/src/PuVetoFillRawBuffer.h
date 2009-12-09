#ifndef PUVETOFILLRAWBUFFER_H 
#define PUVETOFILLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

// from LHCbKernel
#include "Kernel/VeloChannelID.h"

class Word {
  public:
    unsigned int w;
    bool flag;
};

/** @class PuVetoFillRawBuffer PuVetoFillRawBuffer.h
 *
 *  
 *  @author Serena Oggero
 *  @date   2009-07-14
 *  rewrite for latest L0PU bank format
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
  void inizializePUcontainer ( Word PUcontainerBee[4][16] );
  void adjustPUcontainer ( Word PUcontainerBee[4][16] );
  void writeDataVec ( Word PUcontainerBee[4][16], std::vector <unsigned int> & rawDataVec);
  
  std::string binary ( unsigned int );		      
  
private:
  // job option parameters
  std::string        m_inputContainer;
  double             m_threshold;

  // detector geometry variables
  //unsigned int   m_nbPuSensor;
  unsigned int 	m_firstPuSensor;
  DeVelo* 	m_velo;
  Word 		m_PUcontainerBee[4][16];
};
#endif // PUVETOFILLRAWBUFFER_H
