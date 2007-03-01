// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/PVSS/Array.h,v 1.1 2007-03-01 10:39:59 frankb Exp $
//  ====================================================================
//  Array.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $
#ifndef PVSS_ARRAY_H
#define PVSS_ARRAY_H

#include <vector>
#include <string>

/*
 *    PVSS namespace
 */
namespace PVSS {

  /**@class Array  Array.h PVSS/Array.h  
    *
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class Array  {
  public:
    int size;
    int elemSize;
    const void* first;
    void (*load)(const void* s,int,const void** ptr);
    Array(int s, int e, void (*ldf)(const void*,int,const void**)=0) 
      : size(s), elemSize(e), first(0), load(ldf) {}
  };

  /**@class Vector Array.h PVSS/Array.h  
    *
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  template <typename T> class Vector : public Array {
  public:
    const std::vector<T>& vec;
    Vector(const std::vector<T>& v) : Array(v.size(),sizeof(T)), vec(v)
    {  first = size>0 ? &v.front() : 0;                     }
  };
  template <> class Vector<std::string> : public Array {
  public:
    const std::vector<std::string>& vec;
    Vector(const std::vector<std::string>& v, void (*ldf)(const void*,int,const void**)) 
      : Array(v.size(),sizeof(std::string),ldf), vec(v) {}
  };
  template <> class Vector<bool> : public Array {
  public:
    const std::vector<bool>& vec;
    Vector(const std::vector<bool>& v, void (*ldf)(const void*,int,const void**)) 
      : Array(v.size(),sizeof(std::string),ldf), vec(v) {}
  };
}      // End namespace PVSS
#endif // PVSS_ARRAY_H
