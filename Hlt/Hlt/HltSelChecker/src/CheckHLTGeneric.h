// $Id: CheckHLTGeneric.h,v 1.1 2005-01-27 15:55:20 pkoppenb Exp $
#ifndef CHECKHLTGENERIC_H 
#define CHECKHLTGENERIC_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "DaVinciTools/DVAlgorithm.h"


/** @class CheckHLTGeneric CheckHLTGeneric.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-01-21
 */
class CheckHLTGeneric : public DVAlgorithm {
public: 
  /// Standard constructor
  CheckHLTGeneric( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheckHLTGeneric( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_scoreLocation ;
  
};
#endif // CHECKHLTGENERIC_H
