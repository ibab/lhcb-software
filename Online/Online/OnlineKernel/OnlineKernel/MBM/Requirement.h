#ifndef _MBM_REQUIREMENT_H
#define _MBM_REQUIREMENT_H

#include <string>

/*
 *  MBM namespace declaration
 */
namespace MBM {

  /** @class Requirement Requirement.h MBM/Reqirement.h
    *
    *   @author  B.Jost
    *   @version 1.0
    *   @date    12/1/2006
    */
  class Requirement   {
  public:
    int evtype;
    int trmask[4];
    int vetomask[4];
    int userType;
    int freqType;
    int maskType;
    float freq;
  public:
    /// Default constructor
    Requirement();
    /// Parse requirement from string
    void parse(const std::string& reqstring);
    /// Reset requirement to default values
    void reset();
  };
}
#endif  // _MBM_REQUIREMENT_H
