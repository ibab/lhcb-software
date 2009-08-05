// $Id: Pymssm.cpp,v 1.1.1.2 2009-08-05 09:43:37 ngueissa Exp $
#include "LbPythia/Pymssm.h"
#include <iostream>

Pymssm::PYMSSM* Pymssm::s_pymssm = 0;

Pymssm::Pymssm() : m_dummy(-999) , m_realdummy(-999.0) {
  init();
}

Pymssm::~Pymssm() { }

int& Pymssm::imss(int n) {
  if (n<0 || n>=s_lenImss) {
    std::cout << "Pymssm: attempt to read or write IMSS out of bounds" 
              << std::endl;
    m_dummy=-999;
    return m_dummy;
  } else {
    return s_pymssm->imss[n]; 
    // note the lack of a ``-1''. This is intentional.
  }
}

double& Pymssm::rmss(int n) {
  if (n<0 || n>=s_lenRmss) {
    std::cout << "Pymssm: attempt to read or write RMSS out of bounds" 
              << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pymssm->rmss[n]; 
    // note the lack of a ``-1''. This is intentional.
  }
}
