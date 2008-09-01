// $Id: BindType2ClonerDef.h,v 1.2 2008-09-01 16:59:12 jpalac Exp $
#ifndef MICRODST_BINDTYPE2CLONERDEF_H 
#define MICRODST_BINDTYPE2CLONERDEF_H 1

// Include files

/** @class BindType2Cloner BindType2ClonerDef.h MicroDST/BindType2ClonerDef.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-09-01
 */
template <typename T> struct BindType2Cloner 
{
  typedef T type;
  typedef MicroDST::BasicCopy<T> cloner;
};

#endif // MICRODST_BINDTYPE2CLONERDEF_H
