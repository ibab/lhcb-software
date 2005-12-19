// $Id: STSelectAllDigits.h,v 1.1.1.1 2005-12-19 15:42:42 mneedham Exp $
#ifndef _STSelectAllDigits_H
#define _STSelectAllDigits_H

#include "GaudiAlg/GaudiTool.h"
#include "ISTDigitSelector.h"
#include "Event/STDigit.h"

/** @class STSelectAllDigits STSelectAllDigits.h  ITAssociators/STSelectAllDigits
 *
 *  Select all the Digits to go into the ref table
 *
 *  @author M.Needham
 *  @date   15/1/2004
*/


class STSelectAllDigits: public GaudiTool, virtual public ISTDigitSelector {

public:

  /// constructer
  STSelectAllDigits(const std::string& type,
            const std::string& name,
            const IInterface* parent);

  // destructer
  virtual ~STSelectAllDigits();

  /// init event
  virtual StatusCode initializeEvent();

  /// get data
  virtual bool execute(const LHCb::STDigit* aDigit);


};


#endif
