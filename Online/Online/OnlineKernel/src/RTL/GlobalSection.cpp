/*
** QUEUE ROUTINES BY TIM CHARITY - Sep 89
*/
#include "RTL/GlobalSection.h"
#include <cerrno>
#include <cstdio>

RTL::GlobalSection::GlobalSection(const std::string& nam, int size, bool create)
: m_name(nam), m_status(0)
{
  if ( create )
    m_status = lib_rtl_create_section(nam.c_str(), size, &m_address);
  else
    m_status = lib_rtl_map_section(nam.c_str(), size, &m_address);
  if ( !lib_rtl_is_success(m_status) )  {
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to map global section:%s\n",nam.c_str());
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
