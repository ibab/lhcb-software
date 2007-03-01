// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/ArchiveSetting.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  ArchiveSetting.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_ARCHIVESETTING_H
#define ONLINE_PVSS_ARCHIVESETTING_H

// Framework include files
#include "PVSS/CfgSetting.h"

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  /** @class ArchiveSetting   ArchiveSetting.h  PVSS/ArchiveSetting.h
    *
    *  PVSS configuration attribute.
    *
    * _archiveattribute (neutral) type    description
    * _archive                    bool    should variable in the archive be saved
    * _class                      dpid    references of the archive class
    * _initdef                    bool    should variable with default be initialized
    * _num_proc                    int    Reserved for internal purpose. 
    * _round_inv                  bool    should rounding value be inverted 
    * _round_val                  bool    rounding value
    * _std_time                   time    time interval of the standard smoothing procedure
    * _std_tol                   float    tolerance value of the standard smoothing procedure 
    * _std_type                    int    type of standard smoothing procedure (tolerance/time/comb ...)
    * _type                        int    type of archive settings
    *                                     DPCONFIG_NONE: No archive settings
    *                                     DPCONFIG_DB_ARCHIVEINFO: 45  Archive settings
    *  
    * "_archive.._type" attribute:
    * DPATTR_ARCH_PROC_SIMPLESM 3: Archive smoothing is used 
    * DPATTR_ARCH_PROC_VALARCH 15: Data archived in History DB 
    *
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class ArchiveSetting : public CfgSetting {
  protected:
    /// Datapoint name (if supplied by constructor
    bool        m_save;
    bool        m_initdef;
    time_t      m_smoothTime;
    float       m_tolerance;
    std::string m_class;
    int         m_archType;

    /// Apply alarm settings to datapoint identified by datapoint name
    void i_apply(Transaction& tr, const std::string& dp_name);
    /// Apply alarm settings to datapoint identified by datapoint name
    virtual void i_remove(Transaction& tr, const std::string& dp_name);

  public:
    /// Initializing constructor
    ArchiveSetting() : CfgSetting()  {}
    /// Initializing constructor
    ArchiveSetting(const std::string& dp_name) : CfgSetting(dp_name) {}
    /// Initializing constructor
    ArchiveSetting(const DataPoint& dp) : CfgSetting(dp.name()) {}
    /// Standard destructor
    virtual ~ArchiveSetting()   {}
    /// Set alarm range
    ArchiveSetting& set(bool save,
                        bool init,
                        time_t smooth_time,
                        float tol,
                        const std::string& clazz,
                        int   archive_type);
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_ARCHIVESETTING_H
