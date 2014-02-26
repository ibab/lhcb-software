// $Id: PrVeloUTTool2.h,v 1.8 2010-01-09 12:46:27 witekma Exp $
#ifndef IPRVELOUTTOOL_H
#define IPRVELOUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"


// local
#include "Event/Track.h"
#include "PrVUTTrack.h"

static const InterfaceID IID_IPrVeloUTTool ( "IPrVeloUTTool", 1, 0 );


/** @class IPrVeloUTTool IPrVeloUTTool2.h
 *
 *  IPrVeloUTTool
 *  
 *  @author Michel De Cian
 *  @date   2013-12-21
 *  
 *
 */

class IPrVeloUTTool : public GaudiTool {
public:
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrVeloUTTool; }
  
  /// Standard constructor
  IPrVeloUTTool( const std::string& type,
                 const std::string& name,
                 const IInterface* parent) : GaudiTool( type, name, parent ){};
  
  virtual void recoVeloUT(LHCb::Track & velotrack, std::vector<LHCb::Track*>& outtracks ) = 0;
        
protected:
  
private:
  
};

#endif // PRVELOUTTOOL_H
