// $Id: $
#ifndef KERNEL_IRATEFROMTCK_H 
#define KERNEL_IRATEFROMTCK_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRateFromTCK ( "IRateFromTCK", 2, 0 );

/** @class IRateFromTCK IRateFromTCK.h Kernel/IRateFromTCK.h
 *  
 *  Interface for tools that get a rate from a TCK
 *
 *  @author Patrick Koppenburg
 *  @date   2011-08-10
 */
class IRateFromTCK : virtual public IAlgTool
{

public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRateFromTCK; }
  virtual unsigned int getTCK() const = 0;                            ///< retrieve TCK 
  /// Get rate of rate limiting algorithm given its instance name (See TCKsh)
  virtual double rateFromTCK(std::string instanceName) const = 0;  
  /// Get prescale of prescaler algorithm given its instance name (See TCKsh)
  virtual double prescaleFromTCK(std::string instanceName) const = 0;  
  /// Update
  virtual StatusCode runUpdate() = 0;  

};
#endif // KERNEL_IRATEFROMTCK_H
