// $Id: RootNTupleDescriptor.h,v 1.1 2010-01-11 17:14:49 frankb Exp $
//------------------------------------------------------------------------------
// Definition of class :  RootNTupleDescriptor, RootTokenWrap
//
//  Author     : M.Frank
//
//------------------------------------------------------------------------------
#ifndef ROOT_ROOTNTUPLEDESCRIPTOR_H
#define ROOT_ROOTNTUPLEDESCRIPTOR_H

// Framework include files
#include "GaudiKernel/DataObject.h"

/*
 *   Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class RootNTupleDescriptor RootNTupleDescriptor.h RootCnv/RootNTupleDescriptor.h
   *
   * Description:
   *
   * @author  M.Frank
   * @version 1.0
   */
  struct RootNTupleDescriptor : public DataObject {
    /// Description string
    std::string   description;
    /// Optional description 
    std::string   optional;
    /// Identifier of description
    std::string   container;
    /// Class ID of the described object
    unsigned long clid;
    
    /// Standard destructor
    virtual ~RootNTupleDescriptor() {}
  };
}
#endif // ROOT_ROOTNTUPLEDESCRIPTOR_H
