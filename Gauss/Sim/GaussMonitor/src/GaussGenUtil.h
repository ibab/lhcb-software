// $Id: GaussGenUtil.h,v 1.1 2007-03-07 18:51:00 gcorti Exp $
#ifndef GAUSSGENUTIL_H 
#define GAUSSGENUTIL_H 1

// Include files

// forward declaration
namespace HepMC {
  class GenParticle;
}


/** @namespace GenUtil GenUtil.h
 *  @brief Utilities for generator
 *  The lifetime returned is c*tau (mm)
 *
 *  @author Gloria CORTI
 *  @date   2007-02-16
 */

namespace GaussGenUtil {

  double lifetime( const HepMC::GenParticle* );
  
}

#endif // GAUSSGENUTIL_H
