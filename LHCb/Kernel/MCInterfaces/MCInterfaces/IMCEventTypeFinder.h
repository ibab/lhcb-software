// $Id: IMCEventTypeFinder.h,v 1.1 2009-03-06 16:36:36 rlambert Exp $
#ifndef TOOLS_IMCEVENTTYPEFINDER_H 
#define TOOLS_IMCEVENTTYPEFINDER_H 1

// Include files
// from STL
#include <set>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/MCParticle.h"
#include "Kernel/IEvtTypeSvc.h"

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IMCEventTypeFinder("IMCEventTypeFinder", 1, 0);

/** @class IMCEventTypeFinder IMCEventTypeFinder.h DaVinciTools/IMCEventTypeFinder.h
 *
 *  Interface which looks through the MCEvent/Particle for EventTypes
 *  Please see the file MCEventTypeFinder.h
 *  in the src directory of package Sim/SimComponents
 *  Uses the IEvtTypeSvc, in Kernel/LHCbKernel
 *
 *  @author R Lambert
 *  @date   20/02/2009
 */
class IMCEventTypeFinder : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMCEventTypeFinder; }

  /// Set up the event types, in case you want to change them with some other code.
  virtual StatusCode setEventTypes(const LHCb::EventTypeSet) =0;

  /// SLOW. Find all event types in this event using the decay descriptors
  virtual StatusCode findEventTypes(LHCb::EventTypeSet & ) =0;
  
  /// SLOW. Find all decay types from this particle using the decay descriptors
  virtual StatusCode findDecayType(LHCb::EventTypeSet &, const LHCb::MCParticle * ) =0;

  /// SLOW. Find all decay types from these particles using the decay descriptors
  virtual StatusCode findDecayType(LHCb::EventTypeSet &, const LHCb::MCParticle::ConstVector & ) =0;

  /// SLOW. Find all decay types from these particles using the decay descriptors
  virtual StatusCode findDecayType(LHCb::EventTypeSet &, const LHCb::MCParticles & ) =0;

  /// Guess event types in this event based on the quarks/decays in the event
  virtual StatusCode constructEventTypes(LHCb::EventTypeSet & ) =0;
  
  /// Guess decay types from this particle based on its decay
  virtual StatusCode constructDecayType(LHCb::EventTypeSet &, const LHCb::MCParticle * ) =0;
  
  /// Guess decay types from these particles based on their decay
  virtual StatusCode constructDecayType(LHCb::EventTypeSet &, const LHCb::MCParticle::ConstVector &) =0;
  
  /// Guess decay types from these particles based on their decay
  virtual StatusCode constructDecayType(LHCb::EventTypeSet &, const LHCb::MCParticles &) =0;
  
  /// Given an input code, will fill the list of event types this belongs too.
  /// e.g. all events are covered in MinimumBias, any bb-event is covered by inc-b.
  /// so an input of 11154101 ->  11154100    ->   11000000    -> 10000000 -> 30000000
  ///        [specific decay] -> [decay type] -> [hadron mode] -> [inclusive] -> [minbias]
  virtual StatusCode parentEventTypes(long unsigned int, LHCb::EventTypeSet &) =0;

};
#endif // TOOLS_IMCEVENTTYPEFINDER_H
