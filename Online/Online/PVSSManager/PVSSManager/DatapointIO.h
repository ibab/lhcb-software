#ifndef PVSSMANAGER_DATAPOINTIO_H
#define PVSSMANAGER_DATAPOINTIO_H
// $Id: DatapointIO.h,v 1.6 2007-03-12 09:04:13 frankb Exp $

#include "PVSS/DpID.h"
// Forward declarations
class Variable;

#include <string>
#include <vector>

/*
 *    Namespace PVSS
 */
namespace PVSS {

  /* @class DatapointIO  DatapointIO.h PVSSManager/DatapointIO.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  struct DatapointIO  {
    /// PVSS data access to primitive types
    static void value(const Variable* var, bool& val);
    static void value(const Variable* var, char& val);
    static void value(const Variable* var, unsigned char& val);
    static void value(const Variable* var, short& val);
    static void value(const Variable* var, unsigned short& val);
    static void value(const Variable* var, int& val);
    static void value(const Variable* var, unsigned int& val);
    static void value(const Variable* var, long& val);
    static void value(const Variable* var, unsigned long& val);
    static void value(const Variable* var, float& val);
    static void value(const Variable* var, double& val);
    static void value(const Variable* var, DpID& val);
    static void value(const Variable* var, void (*ldf)(std::string&,const char*), std::string& val);
    /// PVSS data access to DYN-type variables
    static void value(const Variable* var, void (*ldf)(std::vector<bool>&,bool),                      std::vector<bool>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<char>&,char),                      std::vector<char>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<unsigned char>&,unsigned char),    std::vector<unsigned char>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<short>&,short),                    std::vector<short>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<unsigned short>&,unsigned short),  std::vector<unsigned short>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<int>&,int),                        std::vector<int>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<unsigned int>&,unsigned int),      std::vector<unsigned int>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<long>&,long),                      std::vector<long>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<unsigned long>&,unsigned long),    std::vector<unsigned long>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<float>&,float),                    std::vector<float>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<double>&,double),                  std::vector<double>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<DpID>&,const DpID&),               std::vector<DpID>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<std::string>&,const char*),        std::vector<std::string>& val);

    static void value(const Variable* variable,int typ, void* buff);
  };
}       /*  End namespace PVSS         */
#endif  /*  PVSSMANAGER_DATAPOINTIO_H  */
