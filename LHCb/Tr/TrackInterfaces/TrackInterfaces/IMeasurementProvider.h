// $Id: IMeasurementProvider.h,v 1.8 2007-07-05 14:05:17 cattanem Exp $
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
 class StateVector;
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

  /** Load (=create) all the Measurements from the list of LHCbIDs
   *  on the input Track
   */
  virtual StatusCode load( LHCb::Track& track ) const = 0;

  /** Construct a Measurement of the type of the input LHCbID
   *  Note: this method is not for general use. A user should preferably call
   *  the "load( Track& track )" method to load=create "in one go" all the
   *  Measurements from the list of LHCbIDs on the Track.
   *  This method is in fact called internally by "load( Track& track )".
   *  @return Pointer the the Measurement created
   *  @param  id:  input LHCbID
   */
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id) const = 0;

  /** Construct a measurement with a statevector. This takes care that
      things like errors depending on track angle are correctly
      set. */

  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, 
					  const LHCb::StateVector& ref) const = 0;

  /** update a measurement with a statevector **/
  virtual StatusCode update( LHCb::Measurement& m, const LHCb::StateVector& state ) const = 0 ;

  /** get the z-position from the lhcb-id **/
  virtual double nominalZ( const LHCb::LHCbID& id ) const = 0 ;

  /** This method is depricated **/
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, double /* par */ ) const { return measurement(id) ; }
  /** This method is depricated **/
  virtual void load() {}

};
#endif // TRACKINTERFACES_IMEASUREMENTPROVIDER_H
