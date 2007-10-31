// $Id: IL0ProcessorDataDecoder.h,v 1.2 2007-10-31 16:42:21 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
//
// ============================================================================
#ifndef L0DU_IL0PROCESSORDATADECODER_H
#define L0DU_IL0PROCESSORDATADECODER_H

// STD & STL 
#include <string>
#include <vector>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// from Event
#include "Event/L0ProcessorData.h"
#include "Event/L0DUBase.h"
static const InterfaceID IID_L0ProcessorDataDecoder( "IL0ProcessorDataDecoder", 1, 0 );



/** @class IL0ProcessorDataDecoder IL0ProcessorDataDecoder.h
 * 
 * Generic interface for tools decoding the container of L0Processor data
 *
 * ProcessorDataDecoder : L0DU/L0ProcessorDataDecoder
 *
 * [replace the obsolete IL0Candidate interface] 
 *
 */

class IL0ProcessorDataDecoder : public virtual IAlgTool {

 public:

  static const InterfaceID& interfaceID() { return IID_L0ProcessorDataDecoder; }

    
  /** The main methods
   *
   */
  virtual bool   setL0ProcessorData(std::vector<std::string> dataLocs )=0;
  virtual bool   setL0ProcessorData(std::vector<LHCb::L0ProcessorDatas*> datass ) = 0;
  virtual bool   setL0ProcessorData(std::string dataLoc )=0;
  virtual bool   setL0ProcessorData(LHCb::L0ProcessorDatas* datas ) = 0;
  virtual double value(  const unsigned int  data[L0DUBase::Index::Size]) = 0;
  virtual unsigned long digit(  const unsigned int  data[L0DUBase::Index::Size]) = 0;
  virtual bool   isValid()=0;
  
};
#endif // L0DU_IL0PROCESSORDATADECODER_H 
