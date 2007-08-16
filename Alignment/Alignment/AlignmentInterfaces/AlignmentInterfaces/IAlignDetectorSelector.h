// $Id: IAlignDetectorSelector.h,v 1.2 2007-08-16 13:43:56 graven Exp $
#ifndef OPTIONS_IALIGNDETECTORSELECTOR_H 
#define OPTIONS_IALIGNDETECTORSELECTOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from kernel
#include "Kernel/LHCbID.h"

/// DetDesc
#include "DetDesc/DetectorElement.h"

static const InterfaceID IID_IAlignDetectorSelector ( "IAlignDetectorSelector", 2, 0 );

/** @class IAlignDetectorSelector IAlignDetectorSelector.h options/IAlignDetectorSelector.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-02-07
 */

class IAlignDetectorSelector : virtual public IAlgTool {
public: 
  typedef std::vector<DetectorElement*> Elements;
  typedef std::pair<bool, unsigned int> ValidIndex;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAlignDetectorSelector; }
  
  /** Virtual method to get detector elements to align
  * @return const reference to flat vector of pointers to detector elements, 
  * i.e. std::vector<DetectorElement*>
  */
  virtual const Elements& getDetectorElements() const = 0;
  
  /** Virtual method to return the number of elements to align
  * @return number of elements of type unsigned int
  */
  virtual unsigned int nElements() const = 0;
  
  virtual const std::vector<std::vector<double> >& constraints() const = 0;
  
  
  /** Virtual method to return an index associated to a detector element and whether it's
  * valid, i.e. a pair of a bool and int std::pair<bool, unsigned int>
  * @return a pair of a bool, (in)valid, and unsigned int, index
  */ 
  virtual ValidIndex index(const LHCb::LHCbID& anLHCbID) const = 0;
  
protected:

private:

};
#endif // OPTIONS_IALIGNDETECTORSELECTOR_H
