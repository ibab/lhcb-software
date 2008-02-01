// $Id: IGetElementsToBeAligned.h,v 1.2 2008-02-01 09:09:32 wouter Exp $
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

#include "LoKi/Range.h"

/// Where am I going to put these?
#include "AlignmentElement.h"
#include "FindAlignmentElement.h"

static const InterfaceID IID_IGetElementsToBeAligned ( "IGetElementsToBeAligned", 1, 0 );

/** @class IGetElementsToBeAligned IGetElementsToBeAligned.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-10-08
 */

class IGetElementsToBeAligned : virtual public IAlgTool {

 public:
  typedef std::vector<AlignmentElement>                            Elements;
  typedef LoKi::Range_<Elements>                                   ElementRange ;
  typedef std::vector<std::vector<double> >                        Constraints;

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IGetElementsToBeAligned; };

  // Return pair of forward begin iter and forward end iter
  virtual const ElementRange& rangeElements() const = 0;

  // Return constraints
  virtual const Constraints& constraints() const = 0;
  

  // Return method that finds an alignment element for a given LHCb id
  virtual const AlignmentElement* findElement(LHCb::LHCbID anLHCbID) const = 0;
  

 protected:
  
 private:

};
#endif // IGETELEMENTSTOBEALIGNED_H
