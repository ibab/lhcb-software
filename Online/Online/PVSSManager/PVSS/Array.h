// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/PVSS/Array.h,v 1.4 2007-03-12 09:04:13 frankb Exp $
//  ====================================================================
//  Array.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: Array.h,v 1.4 2007-03-12 09:04:13 frankb Exp $
#ifndef PVSS_ARRAY_H
#define PVSS_ARRAY_H

#include <vector>
#include <string>

/*
 *    PVSS namespace
 */
namespace PVSS {

  // Forward declarations
  class DpID;
  class Array;

  /** @union DataPointValues
    *
    *  @author  M.Frank
    *  @version 1.0
    */
  union DataPointValues {
    DpID*                 dpid;
    unsigned char*                uchar;
    char*                         schar;
    unsigned int*                 uint;
    int*                          sint;
    unsigned short*               ushort;
    short*                        sshort;
    float*                        real;
    char*                         text;
    time_t*                       time;
    bool*                         boolean;
    std::string*                  string;
    Array*                        array;
    std::vector<std::string>*     stringV;
    std::vector<DpID>*    identV;
    std::vector<unsigned char>*   ucharV;
    std::vector<char>*            charV;
    std::vector<unsigned int>*    uintV;
    std::vector<int>*             intV;
    std::vector<short>*           shortV;
    std::vector<unsigned short>*  ushortV;
    std::vector<float>*           floatV;
    std::vector<time_t>*          timeV;
    std::vector<bool>*            boolV;
    const void*                   pvoid;
    DataPointValues(const void* p) : pvoid(p) {}
  };

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
    const void* (*load)(const void* s,int);
    Array(int s, int e, const void* (*ldf)(const void*,int)=0) 
      : size(s), elemSize(e), first(0), load(ldf) {}
  };

  /**@class Vector Array.h PVSS/Array.h  
    *
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  template <typename T> class ArrayVector : public Array {
  public:
    const std::vector<T>& vec;
    ArrayVector(const std::vector<T>& v) : Array(v.size(),sizeof(T)), vec(v)
    {  first = size>0 ? &v.front() : 0;                     }
  };
  template <> class ArrayVector<std::string> : public Array {
  public:
    const std::vector<std::string>& vec;
    ArrayVector(const std::vector<std::string>& v, const void* (*ldf)(const void*,int)) 
      : Array(v.size(),sizeof(std::string),ldf), vec(v) {}
  };
  template <> class ArrayVector<bool> : public Array {
  public:
    const std::vector<bool>& vec;
    ArrayVector(const std::vector<bool>& v, const void* (*ldf)(const void*,int)) 
      : Array(v.size(),sizeof(std::string),ldf), vec(v) {}
  };
}      // End namespace PVSS
#endif // PVSS_ARRAY_H
