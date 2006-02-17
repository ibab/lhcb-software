// $Id: IMeasurementProvider.h,v 1.5 2006-02-17 15:34:05 erodrigu Exp $
#ifndef TRACKINTERFACES_IMEASUREMENTPROVIDER_H 
#define TRACKINTERFACES_IMEASUREMENTPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
 class LHCbID;
 class Track;
 class Measurement;
};

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

  /** Load the necessary VeloClusters, ITClusters and OTTimes.
   *  Note: this method should be called for each event
   *        before any call to load( Track& track )!
   */
  virtual void load() = 0;

  /** Load (=create) all the Measurements from the list of LHCbIDs
   *  on the input Track
   */
  virtual StatusCode load( LHCb::Track& track ) = 0;

  /** Construct a Measurement of the type of the input LHCbID
   *  Note: this method is not for general use. A user should preferably call
   *  the "load( Track& track )" method to load=create "in one go" all the
   *  Measurements from the list of LHCbIDs on the Track.
   *  This method is in fact called internally by "load( Track& track )".
   *  @return Pointer the the Measurement created
   *  @param  id:  input LHCbID
   *  @param  par: extra parameter for the XxxMeasurement constructor
   *               (refer to XxxMeasurement.h for details)
   */
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID&, 
                                          double par = 0. ) = 0;

protected:

private:

};
#endif // TRACKINTERFACES_IMEASUREMENTPROVIDER_H
