// $Id: ChargedPP2MC.h,v 1.3 2004-06-11 15:26:16 phicharp Exp $
#ifndef ChargedPP2MC_H 
#define ChargedPP2MC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "AsctAlgorithm.h"

#include "DaVinciAssociators/ProtoParticle2MCAsct.h"

/** @class ChargedPP2MC ChargedPP2MC.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   10/07/2002
 */

// Forward declaration
class Object2MCLink;

class ChargedPP2MC : public AsctAlgorithm {

  friend class AlgFactory<ChargedPP2MC>;
  
public:
  /// Standard constructor
  ChargedPP2MC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedPP2MC( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  Tr2MCPartAsct* m_track2MCAsct;
  Object2MCLink* m_track2MCLink;
  std::string m_trackAsctName;
};
#endif // ChargedPP2MC_H
