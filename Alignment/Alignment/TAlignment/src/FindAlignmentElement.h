// $Id: FindAlignmentElement.h,v 1.5 2008-01-07 11:01:16 janos Exp $
#ifndef TALIGNMENT_FINDALIGNMENTELEMENT_H
#define TALIGNMENT_FINDALIGNMENTELEMENT_H 1

// Include files
// from STD
#include <string>
#include <map>

// from DetDesc
#include "DetDesc/DetectorElement.h"
#include "AlignmentElement.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"

// from STDet
#include "STDet/DeSTSector.h"
#include "VeloDet/DeVeloSensor.h"

// from BOOST
#include "boost/function.hpp"

/** @class FindAlignmentElement FindAlignmentElement.h
 *
 *
 *  @author Jan Amoraal
 *  @date   2007-08-31
 */

/// These are some free functions
namespace Alignment {
  typedef boost::function<bool (LHCb::LHCbID)> IsDet;
  typedef boost::function<const DetectorElement* (LHCb::LHCbID)> DetElemFromID;
  typedef std::map<std::string, DetElemFromID> FunctorMap;

  /** Find a tracking station for a given LHCb id */
  const DetectorElement* findTrackingStation(const LHCb::LHCbID& anLHCbID);

  /** This should go into the sub-detector interfaces */
  /// This returns the Velo box, i.e. parent, that a sensor belongs to
  const DetectorElement* findVeloBox(const DeVeloSensor* sensor);

  /// This returns the Velo module, i.e. parent, that a sensor belongs to
  const DetectorElement* findVeloModule(const DeVeloSensor* sensor);

  /// This returns the TT half module, i.e. parent, that a sensor belongs to
  const DetectorElement* findHalfModule(const DeSTSector* sensor);

  /// This returns the IT ladder, i.e. parent, that a sensor belongs to
  const DetectorElement* findLadder(const DeSTSector* sensor);

  /* ***************************************************** */

  DetElemFromID FindElement(const std::string& findWhat);
};

struct FindAlignmentElement {
  
  typedef std::vector<AlignmentElement>::const_iterator            AlignElemIter;
  typedef std::pair<std::vector<AlignmentElement>::const_iterator,
                    std::vector<AlignmentElement>::const_iterator> Range;

  FindAlignmentElement();

  FindAlignmentElement(Alignment::DetElemFromID elemFromLHCbID, Range rangeElements);

  const AlignmentElement* operator()(const LHCb::LHCbID& anLHCbID);

 private:

  Alignment::DetElemFromID m_elemFromLHCbID;
  Range                    m_rangeElements;

};

#endif // TALIGNMENT_FINDALIGNMENTELEMENT_H
