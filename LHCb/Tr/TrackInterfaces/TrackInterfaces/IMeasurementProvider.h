// $Id: IMeasurementProvider.h,v 1.1 2005-06-28 15:59:57 erodrigu Exp $
#ifndef TRACKINTERFACES_IMEASUREMENTPROVIDER_H 
#define TRACKINTERFACES_IMEASUREMENTPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class Track;
class Measurement;
class LHCbID;

static const InterfaceID IID_IMeasurementProvider ( "IMeasurementProvider", 1, 0 );

/** @class IMeasurementProvider IMeasurementProvider.h TrackInterfaces/IMeasurementProvider.h
 *  
 *  Interface for the measurement provider tool
 *
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-06-28
 */
class IMeasurementProvider : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMeasurementProvider; }

  virtual StatusCode initialize() = 0;

  virtual void load() = 0;

  virtual StatusCode load( Track& track ) = 0;

  virtual Measurement& measurement( const LHCbID&, 
                                    double par0 = 999.,
                                    double par1 = 999.) = 0;

protected:

private:

};
#endif // TRACKINTERFACES_IMEASUREMENTPROVIDER_H
