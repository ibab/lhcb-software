// $Id: RawEventPrintout.h,v 1.4 2007-12-14 11:42:24 frankb Exp $
//====================================================================
//	RawEventPrintout.h
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
