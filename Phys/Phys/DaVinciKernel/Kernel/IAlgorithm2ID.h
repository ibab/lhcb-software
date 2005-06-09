// $Id: IAlgorithm2ID.h,v 1.2 2005-06-09 13:10:40 pkoppenb Exp $
#ifndef KERNEL_IALGORITHM2ID_H 
#define KERNEL_IALGORITHM2ID_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IAlgorithm2ID ( "IAlgorithm2ID", 1, 1 );

/** @class IAlgorithm2ID IAlgorithm2ID.h Kernel/IAlgorithm2ID.h
 *  
 *  Allows to assign an identifier to an algorithm
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-04-15
 */
class IAlgorithm2ID : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAlgorithm2ID; }

  /// Return ID from name
  virtual int idFromName( const std::string& ) = 0;
 
  /// Return name from ID
  virtual std::string nameFromId( const int& ) = 0;
 
  /// Return size
  virtual int size(void) = 0;
 
protected:

private:

};
#endif // KERNEL_IALGORITHM2ID_H
