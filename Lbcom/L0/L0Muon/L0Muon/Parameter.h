// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/Parameter.h,v 1.1 2001-06-07 16:46:20 atsareg Exp $
#ifndef PARAMETER_H
#define PARAMETER_H 1


// Include files

#include <vector>
#include <map>
#include <string>


//------------------------------------------------------------------------------
//
// ClassName:   Parameter
//  
// Description: Provide parameters to Detector elements.
//
//------------------------------------------------------------------------------


class Parameter 
{
public:
  // Constructors
  Parameter();

  // Destructor
  ~Parameter();  

  // Get parameters
  
  double  get(const std::string& name) ;
  double  get(const std::string& name, unsigned int index) ;
  long    getInt(const std::string& name) ;
  long    getInt(const std::string& name, unsigned int index) ;
  std::vector<double> getVector(const std::string& name) ;
  std::vector<long> getVectorInt(const std::string& name) ;
  long    sizeVector(const std::string& name) const;

  // Set parameters

  void   addParameter( double x, const std::string& name);
  void   addParameterVector( double y, const std::string& name);
  void   addParameterVector( const std::vector<double>& y, 
                             const std::string& name);
  void   addParameterVector( const std::vector<long>& y, 
                             const std::string& name);	
  void   addParameterVector( const std::vector<int>& y, 
                             const std::string& name);				     		     

private:

  inline long toLong ( double d ) const ; 
  std::map<std::string, double> m_parameter;
  std::map<std::string, std::vector<double> > m_parameterVector;

};

#endif    // PARAMETER_H
