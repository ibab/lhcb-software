// $Id: IGetElementsToBeAligned.h,v 1.9 2009-01-21 16:23:16 wouter Exp $
#ifndef IGETELEMENTSTOBEALIGNED_H 
#define IGETELEMENTSTOBEALIGNED_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <utility>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Kernel
#include "Kernel/LHCbID.h"

// from LoKi
#include "LoKi/Range.h"

/// Local
#include "AlignmentElement.h"

static const InterfaceID IID_IGetElementsToBeAligned ( "IGetElementsToBeAligned", 1, 0 );

/** @class IGetElementsToBeAligned IGetElementsToBeAligned.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-10-08
 */

namespace LHCb {
  class Measurement ;
}

class IGetElementsToBeAligned : virtual public IAlgTool {

 public:
  typedef std::vector<const AlignmentElement*> Elements;

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IGetElementsToBeAligned; };

  // Return pair of forward begin iter and forward end iter
  virtual const Elements& elements() const = 0 ;
  
  // Return method that finds an alignment element for a given LHCb id
  virtual const AlignmentElement* findElement(const LHCb::LHCbID& anLHCbID) const = 0;

  // Return method that finds an alignment element for a given Measuerment
  virtual const AlignmentElement* findElement(const LHCb::Measurement& meas) const = 0;

  // Return method that finds an alignment element for a given detector element
  virtual const AlignmentElement* findElement(const DetectorElement& element) const = 0 ;  

  // Find the list of elements corresponding to a path (which can ba rehulare expression)
  virtual StatusCode findElements(const std::string& path, 
				  std::vector<const AlignmentElement*>& alignelements) const = 0 ;

};
#endif // IGETELEMENTSTOBEALIGNED_H
