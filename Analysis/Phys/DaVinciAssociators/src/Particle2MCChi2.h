// $Id: Particle2MCChi2.h,v 1.2 2002-07-12 15:26:54 phicharp Exp $
#ifndef Particle2MCChi2_H 
#define Particle2MCChi2_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"

// local
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCWithChi2Asct.h"

/** @class Particle2MCChi2 Particle2MCChi2.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   17/05/2002
 */
class Particle2MCChi2 : public Algorithm {
public:
  /// Standard constructor
  Particle2MCChi2( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Particle2MCChi2( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// Location of table in TES
  std::string outputTable() { return m_outputTable; }
  
protected:

private:
  std::vector<std::string> m_inputData;  ///< location of Particles to associate
  std::string m_outputTable; ///< location of relations table
  double m_chi2;            ///< Chi2 maximum

  Particle2MCWithChi2Asct::IAsct* 
    m_pAsctChi2;  ///< Pointer to the Chi2 weighted associator
};
#endif // Particle2MCChi2_H
