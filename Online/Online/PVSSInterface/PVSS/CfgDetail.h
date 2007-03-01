// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/CfgDetail.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  CfgDetail.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_CFGDETAIL_H
#define ONLINE_PVSS_CFGDETAIL_H

// Framework include files
#include "PVSS/NamedIdentified.h"

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  class CfgType;
  class CfgManager;

  /** @class CfgDetail   CfgDetail.h  PVSS/CfgDetail.h
    *
    *  PVSS configuration attribute.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class CfgDetail  : public NamedIdentified {
  public:
    friend struct CfgManip<CfgDetail>;
    enum RangeType {
        RANGE_NORANGE,
        RANGE_RANGECHECK,
        RANGE_RANGE_ALL,
        RANGE_SET,
        RANGE_MINMAX,
        RANGE_MATCH
    };

  protected:

    /// Associated configuration type
    CfgType*  m_config;

    /// Initializing constructor
    CfgDetail(CfgType* cfg, int i, const std::string& nam);
    /// Standard destructor
    virtual ~CfgDetail();

  public:

    /// Access configuration type
    CfgType* config() const          {  return m_config;       }
    /// Access to the configuration manager
    CfgManager* manager() const;
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_CFGDETAIL_H
