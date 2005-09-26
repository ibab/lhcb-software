/*
** QUEUE ROUTINES BY TIM CHARITY - Sep 89
*/
#include "RTL/GlobalSection.h"
#include <cerrno>
#include <cstdio>

RTL::GlobalSection::GlobalSection(const std::string& nam)
: m_name(nam), m_status(0)
{
  m_status = lib_rtl_map_section(nam.c_str(), m_address);
  if ( !lib_rtl_is_success(m_status) )  {
    ::printf("Failed to map global section:%s\n",nam.c_str());
  }
}

RTL::GlobalSection::GlobalSection(const std::string& nam, int size)
: m_name(nam), m_status(0)
{
  m_status = lib_rtl_create_section(nam.c_str(), size, m_address);
  if ( !lib_rtl_is_success(m_status) )  {
    ::printf("Failed to map global section:%s\n",nam.c_str());
  }
}

RTL::GlobalSection::~GlobalSection()  {
  if ( lib_rtl_is_success(m_status) )  {
    m_status = lib_rtl_unmap_section(m_address);
    if ( lib_rtl_is_success(m_status) )  {
      return;
    }
    ::printf("Failed to unmap global section:%s\n",m_name.c_str());
  }
}
