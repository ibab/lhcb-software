// $Id: IDecodeSimpleDecayString.h,v 1.1 2004-07-01 16:36:36 pkoppenb Exp $
#ifndef DAVINCITOOLS_IDECODESIMPLEDECAYSTRING_H 
#define DAVINCITOOLS_IDECODESIMPLEDECAYSTRING_H 1

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
 *  IDecodeSimpleDecayString.h DaVinciTools/IDecodeSimpleDecayString.h
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
  virtual std::string getDescriptor(void) = 0;
  virtual StatusCode getStrings(std::string&, strings&) = 0;
  virtual StatusCode getPIDs(int&, ints&) = 0;
  virtual StatusCode getStrings_cc(std::string&, strings&) = 0;
  virtual StatusCode getPIDs_cc(int&, ints&) = 0;
  virtual bool is_cc(void) = 0;
  
};
#endif // DAVINCITOOLS_IDECODESIMPLEDECAYSTRING_H
