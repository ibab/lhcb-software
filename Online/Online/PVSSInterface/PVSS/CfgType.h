// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/CfgType.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  CfgType.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_CFGTYPE_H
#define ONLINE_PVSS_CFGTYPE_H

// Framework include files
#include "PVSS/NamedIdentified.h"

// C++ include files
#include <vector>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class CfgManager;
  class CfgDetail;
  class CfgAttribute;

  /** @class CfgType   CfgType.h  PVSS/CfgType.h
    *
    *  PVSS configuration attribute.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class CfgType  : public NamedIdentified {
  public:
    friend struct CfgManip<CfgType>;
    enum Type
    {
      NOCONFIG,
      CONFIG,
      VALUE,
      DEFAULTVALUE,
      CONNDISPATCH,
      DISPATCHINFOCONT,
      PVSS_RANGECHECK,
      MINMAX_PVSS_RANGECHECK,
      SET_PVSS_RANGECHECK,
      USER_RANGECHECK,
      ALERT_STATEBITS,
      ALERT_VALUE,
      ALERT_BINARYSIGNAL,
      ALERT_NONBINARYSIGNAL,
      ALERT_CLASS,
      PERIPH_ADDR,
      PERIPH_ADDR_MAIN,
      PERIPH_ADDR_AUX,
      SSI_PERIPH_ADDR,
      SSI_PERIPH_ADDR_MAIN,
      SSI_PERIPH_ADDR_AUX,
      PROFI_PERIPH_ADDR_old_dont_use,
      PROFI_PERIPH_ADDR_MAIN_old_dont_use,
      PROFI_PERIPH_ADDR_AUX_old_dont_use,
      PROFI_DRIVER,
      PROFI_KBLHDR,
      PROFI_KBL,
      PROFI_OVHDR,
      PROFI_OVSIMPLE,
      PROFI_OVARRAY,
      CONV_OR_SMOOTH,
      CONVERSION,
      CONVERSION_RAW_TO_ING,
      CONVERSION_RAW_TO_ING_MAIN,
      CONVERSION_RAW_TO_ING_AUX,
      CONVERSION_ING_TO_RAW,
      CONVERSION_ING_TO_RAW_MAIN,
      CONVERSION_ING_TO_RAW_AUX,
      SMOOTHING,
      SMOOTH_MAIN,
      SMOOTH_AUX,
      UIDISPATCHINFO,
      UIINTERN,
      MAN,
      INTERNAL,
      DB_ARCHIVEINFO,
      CONNECTION,
      CONNECTCOUNT,
      SMOOTH_MAIN_SIMPLE,
      SMOOTH_MAIN_DERIV,
      SMOOTH_MAIN_FLUTTER,
      SMOOTH_AUX_SIMPLE,
      SMOOTH_AUX_DERIV,
      SMOOTH_AUX_FLUTTER,
      FIRST_ARCHIVE,
      GENERAL_CONFIG,
      DISTRIBUTION_INFO,
      LOGGER_INFO,
      DB_ARCHIVECLASS,
      SUM_ALERT,
      DP_FUNCTION,
      DPLOCK,
      AUTH,
      STAT_FUNCTION,
      MATCH_PVSS_RANGECHECK
    };

    /// Internal type definitions: attribute container
    typedef std::vector<CfgAttribute*> Attrs;
    /// Internal type definitions: details container
    typedef std::vector<CfgDetail*>    Details;

    //static const char* ORIGINAL  = "original";
    //static const char* DEFAULT   = "default";
    //static const char* ADDRESS   = "address";
    //static const char* ALERT_HDL = "alert_hdl";

  protected:
    /// Access to manager
    CfgManager *m_manager;
    /// Attribute container
    Attrs       m_attrs;
    /// Detail container
    Details     m_details;

    /// Initializing constructor
    CfgType(CfgManager* mgr, int i, const std::string& nam);
    /// Standard destructor
    virtual ~CfgType();
    /// Add detail to type
    void addDetail(CfgDetail* attr);
    /// Add attribute to type
    void addAttribute(CfgAttribute* attr);

  public:

    /// Access configuration type
    CfgManager* manager() const        {  return m_manager;   }
    /// Client access to attributes
    const Attrs& attributes()  const   {  return m_attrs;     }
    /// Client access to attributes
    const Details& details()  const    {  return m_details;   }
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_CFGTYPE_H
