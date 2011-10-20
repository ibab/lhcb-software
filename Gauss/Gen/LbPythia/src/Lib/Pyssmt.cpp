// $Id: Pyssmt.cpp,v 1.1.1.2 2009-08-05 09:43:37 ngueissa Exp $
#include "LbPythia/Pyssmt.h"
#include <iostream>

Pyssmt::PYSSMT const * Pyssmt::s_pyssmt = 0;

const double& Pyssmt::zmix(int i, int j) const {
  if (i<0 || i>s_sizeZmix || j<0 || j>s_sizeZmix) {
    std::cout << "Pyssmt: attempt to read or write ZMIX out of bounds" 
              << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->zmix[j-1][i-1];
  }
}

const double& Pyssmt::umix(int i, int j) const {
  if (i<0 || i>s_sizeUmix || j<0 || j>s_sizeUmix) {
    std::cout << "Pyssmt: attempt to read or write UMIX out of bounds" 
              << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->umix[j-1][i-1];
  }
}

const double& Pyssmt::vmix(int i, int j) const {
  if (i<0 || i>s_sizeVmix || j<0 || j>s_sizeVmix) {
    std::cout << "Pyssmt: attempt to read or write VMIX out of bounds" 
              << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->vmix[j-1][i-1];
  }
}

const double& Pyssmt::smz(int i) const {
  if (i<0 || i>s_lenSmz) {
    std::cout << "Pyssmt: attempt to read or write SMZ out of bounds" 
              << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->smz[i-1];
  }
}

const double& Pyssmt::smw(int i) const {
  if (i<0 || i>s_lenSmw) {
    std::cout << "Pyssmt: attempt to read or write SMW out of bounds" 
              << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->smw[i-1];
  }
}

const double& Pyssmt::sfmix(int i, int j) const 
{
  if ( i<0 || j>s_lenSfmix || j<0 || i>s_widthSfmix ) {
    std::cout << "Pyssmt: attempt to read or write SFMIX out of bounds" 
              << std::endl ;
    m_realdummy=-999.0;
    return m_realdummy ;
  }
  else {
    return s_pyssmt->sfmix[j-1][i-1];
  }
}

const double& Pyssmt::vmixi(int i, int j) const 
{
  if ( i<0 || i>s_sizeVmix || j<0 || j>s_sizeVmix ) {
    std::cout << "Pyssmt: attempt to read or write VMIXI out of bounds" 
              << std::endl ;
    m_realdummy=-999.0;
    return m_realdummy ;
  }
  else {
    return s_pyssmt->vmixi[j-1][i-1];
  }
}

const double& Pyssmt::zmixi(int i, int j) const 
{
  if ( i<0 || i>s_sizeZmix || j<0 || j>s_sizeZmix ) {
    std::cout << "Pyssmt: attempt to read or write ZMIXI out of bounds" 
              << std::endl ;
    m_realdummy=-999.0;
    return m_realdummy ;
  }
  else {
    return s_pyssmt->zmixi[j-1][i-1];
  }
}

const double& Pyssmt::umixi(int i, int j) const 
{
  if ( i<0 || i>s_sizeUmix || j<0 || j>s_sizeUmix ) {
    std::cout << "Pyssmt: attempt to read or write UMIXI out of bounds" 
              << std::endl ;
    m_realdummy=-999.0;
    return m_realdummy ;
  }
  else {
    return s_pyssmt->umixi[j-1][i-1];
  }
}
