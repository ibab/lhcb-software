// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/Parameter.cpp,v 1.1.1.1 2001-04-06 14:26:03 atsareg Exp $
#define PARAMETER_CPP

// Include files
#include "Parameter.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  Parameter
//
//------------------------------------------------------------------------------


// Standard Constructors
Parameter::Parameter()
{
}


// Standard Destructor
Parameter::~Parameter()
{
}


double  Parameter::get(const std::string& name) {
    return m_parameter[name];
}
  
double  Parameter::get(const std::string& name, long index) {
    return m_parameterVector[name][index];
}  

long  Parameter::getInt(const std::string& name) {
    return toLong(m_parameter[name]);
}
  
long  Parameter::getInt(const std::string& name, long index) {
    return toLong(m_parameterVector[name][index]);				    
} 

std::vector<double> Parameter::getVector(const std::string& name) {
    return m_parameterVector[name];
}

std::vector<long> Parameter::getVectorInt(const std::string& name) {
    std::vector<long> tmp;
    std::vector<double>::iterator it;
    
    for (it=m_parameterVector[name].begin();
         it != m_parameterVector[name].end(); it++) {
        tmp.push_back(toLong(*it));
    }
    return tmp;	
}

long Parameter::sizeVector(const std::string& name) const {
    return m_parameter.size();
}   
 

void Parameter::addParameter( double x, const std::string& name) {
//    cout << " adding to "+name+" " << x << endl;
    m_parameter[name] = x;
//    cout << "added " << m_parameter[name] << endl;
}

void Parameter::addParameterVector( double y, const std::string& name) {
    m_parameterVector[name].push_back(y);
}

void Parameter::addParameterVector( const std::vector<double>& y,
                                             const std::string& name) {
    m_parameterVector[name] = y;
}

void Parameter::addParameterVector( const std::vector<long>& y,
                                             const std::string& name) {
    std::vector<double> tmp(y.begin(),y.end());					     
    m_parameterVector[name] = tmp;
}

void Parameter::addParameterVector( const std::vector<int>& y,
                                             const std::string& name) {
    std::vector<double> tmp(y.begin(),y.end());					     
    m_parameterVector[name] = tmp;
}

inline long Parameter::toLong ( double d ) const {
    if ( d>0 ) {
        return (long)(d+0.5);
    } else {
        return (long)(d-0.5);
    }
} 
