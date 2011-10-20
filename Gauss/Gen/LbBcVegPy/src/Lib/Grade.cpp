// $Id: Grade.cpp,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
// access BcGen common Grade
#include "LbBcVegPy/Grade.h"
#include <iostream>

// set pointer to zero at start
Grade::GRADE* Grade::s_grade =0;

// Constructor
Grade::Grade() : m_dummy( 0 ) , m_realdummy( 0. ) { }

// Destructor
Grade::~Grade() { }

// access xi in common
double& Grade::xi(int i, int j) {
  init(); // check COMMON is initialized
  if(i < 0 || j > s_lenXi || j<0 || i > s_widthXi ) {
    std::cout << "Grade: attempt to read or write XI out of bounds"
              << std::endl;
    m_realdummy = -999.0;
    return m_realdummy;
  }
  else {
    return s_grade->xi[j-1][i-1];
}
}
