#ifndef PVSSMANAGER_DATAPOINTIO_H
#define PVSSMANAGER_DATAPOINTIO_H
// $Id: DatapointIO.h,v 1.3 2007-03-01 20:09:09 frankb Exp $

// Forward declarations
class Variable;
class DpIdentifier;

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
    static void value(const Variable* var, DpIdentifier& val);
    static void value(const Variable* var, void (*ldf)(std::string&,const char*), std::string& val);
    /// PVSS data access to DYN-type variables
    static void value(const Variable* var, void (*ldf)(std::vector<bool>&,const bool),                      std::vector<bool>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<char>&,const char),                      std::vector<char>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<unsigned char>&,const unsigned char),    std::vector<unsigned char>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<short>&,const short),                    std::vector<short>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<unsigned short>&,const unsigned short),  std::vector<unsigned short>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<int>&,const int),                        std::vector<int>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<unsigned int>&,const unsigned int),      std::vector<unsigned int>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<long>&,const long),                      std::vector<long>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<unsigned long>&,const unsigned long),    std::vector<unsigned long>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<float>&,const float),                    std::vector<float>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<double>&,const double),                  std::vector<double>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<DpIdentifier>&,const DpIdentifier&),     std::vector<DpIdentifier>& val);
    static void value(const Variable* var, void (*ldf)(std::vector<std::string>&,const char*),              std::vector<std::string>& val);

  };
}       /*  End namespace PVSS         */
#endif  /*  PVSSMANAGER_DATAPOINTIO_H  */
