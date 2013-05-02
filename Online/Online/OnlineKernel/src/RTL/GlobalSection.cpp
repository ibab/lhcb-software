/*
** QUEUE ROUTINES BY TIM CHARITY - Sep 89
*/
#include "RTL/GlobalSection.h"
#include <cerrno>
#include <cstdio>

RTL::GlobalSection::GlobalSection(const std::string& nam, int size, bool create, bool enable_locking)
: m_name(nam), m_status(0), m_id(0)
{
  if ( create )
    m_status = lib_rtl_create_section(nam.c_str(), size, &m_address);
  else
    m_status = lib_rtl_map_section(nam.c_str(), size, &m_address);

  if ( !lib_rtl_is_success(m_status) )  {
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to map global section:%s\n",nam.c_str());
    return;
  }
  if ( enable_locking )  {
    std::string lck_name = nam+"_lock";
    m_status = ::lib_rtl_create_lock(lck_name.c_str(),&m_id);
    if ( !lib_rtl_is_success(m_status) )  {
      ::lib_rtl_output(LIB_RTL_ERROR,"Failed create lock for global section:%s\n",nam.c_str());
      return;
    }
  }
}

RTL::GlobalSection::~GlobalSection()  {
  if ( lib_rtl_is_success(m_status) )  {
    m_status = lib_rtl_unmap_section(m_address);
    if ( lib_rtl_is_success(m_status) )  {
      return;
    }
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to unmap global section:%s\n",m_name.c_str());
  }
}

/// Lock the global section
int RTL::GlobalSection::lock() {
  return m_status=(m_id ? ::lib_rtl_lock(m_id) : 0);
}

/// Unlock the global section
int RTL::GlobalSection::unlock() {
  return m_status=(m_id ? ::lib_rtl_unlock(m_id) : 0);
}

