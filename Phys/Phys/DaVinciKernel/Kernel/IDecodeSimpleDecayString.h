// $Id: IDecodeSimpleDecayString.h,v 1.1 2005-01-06 10:30:43 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IDECODESIMPLEDECAYSTRING_H 
#define DAVINCIKERNEL_IDECODESIMPLEDECAYSTRING_H 1

// Include files
// from STL
#include <string>
typedef std::vector<std::string> strings;
typedef std::vector<int> ints;

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IDecodeSimpleDecayString
( "IDecodeSimpleDecayString", 1, 0 );

/** @class IDecodeSimpleDecayString 
 *  IDecodeSimpleDecayString.h Kernel/IDecodeSimpleDecayString.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-06-30
 */
class IDecodeSimpleDecayString : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { 
    return IID_IDecodeSimpleDecayString;}

  virtual StatusCode setDescriptor(const std::string&) = 0;
  virtual std::string getDescriptor(void) const = 0;
  virtual StatusCode getStrings(std::string&, strings&) const = 0;
  virtual StatusCode getPIDs(int&, ints&) const = 0;
  virtual StatusCode getStrings_cc(std::string&, strings&) const = 0;
  virtual StatusCode getPIDs_cc(int&, ints&) const = 0;
  virtual bool is_cc(void) const = 0;
  
};
#endif // DAVINCIKERNEL_IDECODESIMPLEDECAYSTRING_H
