// $Id: CaloDigitToMCParticleAsct.h,v 1.1 2003-03-20 08:22:16 ocallot Exp $
#ifndef CALODIGITTOMCPARTICLEASCT_H 
#define CALODIGITTOMCPARTICLEASCT_H 1

// Include files
// from STL
#include <string>

// LHCbKernel
#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h" 

// from Gaudi
#include "GaudiKernel/Algorithm.h"


/** @class CaloDigitToMCParticleAsct CaloDigitToMCParticleAsct.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2003-02-20
 */
class CaloDigitToMCParticleAsct : public Algorithm {  
  friend AlgFactory<CaloDigitToMCParticleAsct>; 
public:
  /// Standard constructor
  CaloDigitToMCParticleAsct( const std::string& name, 
                             ISvcLocator* pSvcLocator );

  virtual ~CaloDigitToMCParticleAsct( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputContainer;
  std::string m_mcContainer;
  std::string m_outputTable;
  double      m_minimalFraction;
  double      m_zMax;
};
#endif // CALODIGITTOMCPARTICLEASCT_H
