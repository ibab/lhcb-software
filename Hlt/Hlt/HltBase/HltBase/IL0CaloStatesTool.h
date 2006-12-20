// $Id: IL0CaloStatesTool.h,v 1.1 2006-12-20 09:32:45 hernando Exp $
#ifndef HLTBASE_IL0CALOSTATESTOOL_H 
#define HLTBASE_IL0CALOSTATESTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/L0CaloCandidate.h"
#include "Event/Track.h"

static const InterfaceID IID_IL0CaloStatesTool ( "IL0CaloStatesTool", 1, 0 );

/** @class IL0CaloStatesTool IL0CaloStatesTool.h HltBase/IL0CaloStatesTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-12-14
 */
class IL0CaloStatesTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0CaloStatesTool; }

  virtual StatusCode seeds(const LHCb::L0CaloCandidate& calo,
                           LHCb::State& state0, LHCb::State& state1) = 0;
  

protected:

private:

};
#endif // HLTBASE_IL0CALOSTATESTOOL_H
