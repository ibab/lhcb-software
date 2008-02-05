// $Id: RawEventPrintout.h,v 1.5 2008-02-05 16:44:18 frankb Exp $
//====================================================================
//  RawEventPrintout.h
//--------------------------------------------------------------------
//
//====================================================================
#ifndef MDF_RAWEVENTPRINTOUT_H
#define MDF_RAWEVENTPRINTOUT_H 1

#include <string>
/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations:
  class RawBank;
  class RawEvent;

  /** @class RawEventPrintout RawEventPrintout.h MDF/RawEventPrintout.h
    *
    */
  class RawEventPrintout  {
  public:
    static std::string bankHeader(const RawBank* r);
    static std::string bankType(const RawBank* r);
    static std::string bankType(int i);
  };
}   // End namespace LHCb
#endif // MDF_RAWEVENTPRINTOUT_H
