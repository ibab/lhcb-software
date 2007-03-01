// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/ArchiveSetting.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $
//  ====================================================================
//  ArchiveSetting.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: ArchiveSetting.cpp,v 1.2 2007-03-01 15:47:56 frankb Exp $

// Framework include files
#include "PVSS/CfgType.h"
#include "PVSS/ArchiveSetting.h"

using namespace PVSS;

/// Set alarm range
ArchiveSetting& 
ArchiveSetting::set(bool save,
                    bool    init,
                    time_t  smooth_time,
                    float   tol,
                    const std::string& clazz,
                    int archive_type)
{
  m_save        = save;
  m_initdef     = init;
  m_smoothTime  = smooth_time;
  m_tolerance   = tol;
  m_class       = clazz;
  m_archType    = archive_type;
  return *this;
}

/// Apply alarm settings to datapoint identified by datapoint name
void ArchiveSetting::i_remove(Transaction& tr, const std::string& dp_name)   {
  try  {
  }
  catch(const std::exception& e)  {
    std::cout << e.what() << std::endl;
  }
}

/// Apply alarm settings to datapoint identified by datapoint name
void ArchiveSetting::i_apply(Transaction& tr, const std::string& dp)   {
  tr->setValue(dp+".:_archive.._type",     CfgType::DB_ARCHIVEINFO);
  tr->setValue(dp+".:_archive.._archive",  m_save);
  tr->setValue(dp+".:_archive.._initdef",  m_initdef);
  tr->setValue(dp+".:_archive.._std_tol",  m_tolerance);
  tr->setValue(dp+".:_archive.._std_time", DPTime(m_smoothTime));
  tr->setValue(dp+".:_archive.1._type",    m_archType);
  tr->setValue(dp+".:_archive.1._class",   m_class);
}
