// $Id: $
#ifndef FASTVALIDATEWITHTT_H 
#define FASTVALIDATEWITHTT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "FastTTValidationTool.h"

/** @class FastValidateWithTT FastValidateWithTT.h
 *   
 *
 *  @author Olivier Callot
 *  @date   2011-01-19
 */
class FastValidateWithTT : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FastValidateWithTT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FastValidateWithTT( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  void printTTHit( const PatTTHit* hit );
  
private:
  ITrackSelector* m_validationTool;

  std::string m_inputName;

  int m_nForward;
  int m_nGood;
};
#endif // FASTVALIDATEWITHTT_H
