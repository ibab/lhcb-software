// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/CfgAttribute.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  CfgAttribute.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_CFGATTRIBUTE_H
#define ONLINE_PVSS_CFGATTRIBUTE_H

// Framework include files
#include "PVSS/NamedIdentified.h"

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  class CfgType;
  class CfgManager;

  /** @class CfgAttribute   CfgAttribute.h  PVSS/CfgAttribute.h
    *
    *  PVSS configuration attribute.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class CfgAttribute  : public NamedIdentified {
    friend struct CfgManip<CfgAttribute>;

    /// Associated configuration type
    CfgType*  m_config;

  protected:

    /// Initializing constructor
    CfgAttribute(CfgType* cfg, int i, const std::string& nam);
    /// Standard destructor
    virtual ~CfgAttribute();

  public:

    /// Access configuration type
    CfgType* config() const          {  return m_config;       }
    /// Access to the configuration manager
    CfgManager* manager() const;
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_CFGATTRIBUTE_H
