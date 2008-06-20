// $Id: GenericConfiguration.h,v 1.1 2008-06-20 09:54:22 jonrob Exp $
#ifndef GENERICRINGFINDER_GENERICCONFIGURATION_H
#define GENERICRINGFINDER_GENERICCONFIGURATION_H 1

// includes
#include <iostream>
#include <vector>
#include <map>
#include "Utils/OstreamUtils.h"

namespace GenRingF {

  /** @class GenericConfiguration GenericConfiguration.h GenericRingFinder/GenericConfiguration.h
   *
   *  Generic configuration options
   *
   *  @author Chris Jones
   *  @date   2006-08-04
   */

  class GenericConfiguration
  {

  public:

    class BadConfiguration : public std::exception 
    {
    public:
      BadConfiguration( const std::string & mess = "" )
        : m_message("BadParameter["+mess+"]") { }
      const char * what() const throw() 
      {
        return m_message.c_str();
      }
      virtual ~BadConfiguration() throw() {}
    private:
      std::string m_message;
    };

  public:

    /// Default constructor
    GenericConfiguration() 
    { 
      setParam("RunForFixedTime",1); 
      setParam("AbsMaxTime",5); 
    }
    /// Print method
    std::ostream & printMeTo(std::ostream & os) const
    {
      return os << "GenericConfiguration " << params;
    }
    /// Check if a given parameter is defined
    inline bool hasParam( const std::string & param ) const
    {
      Params::const_iterator i = params.find(param);
      return ( i != params.end() );
    }
    /// return the value of a given parameter
    /// behaviour undefined if pN not checked first by hasParam and found to be there.
    inline double param( const std::string & pN ) const
    {
      return params.find(pN)->second;
    }
    /// check if a param exists and return if it does, otherwise throw exception
    inline double checkThenGetParam( const std::string & pN ) const
    {
      if ( !hasParam(pN) ) throw BadConfiguration(pN);
      return param(pN);
    }
    /// set a given parameter
    inline void setParam( const std::string & pN, const double val )
    {
      params[pN] = val;
    }
    /// remove a given parameter
    inline void removeParam( const std::string & pN )
    {
      params.erase(pN);
    }
    /// Clear all parameters
    inline void clearAllparams()
    {
      params.clear();
    }

  private:

    typedef std::map<std::string,double> Params;
    Params params;
  };

}

inline std::ostream & operator<<(std::ostream & os, const GenRingF::GenericConfiguration & obj) {
  return obj.printMeTo(os);
};

#endif // GENERICRINGFINDER_GENERICCONFIGURATION_H
