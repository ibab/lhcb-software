// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/Parameter.cpp,v 1.2 2001-05-03 09:10:31 atsareg Exp $
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
    std::map<std::string, double>::const_iterator ci;
    ci = m_parameter.find(name);
    if ( ci != m_parameter.end() ) {
        return m_parameter[name];
    } else {
        return 0.;
    }	
}
  
double  Parameter::get(const std::string& name, unsigned int index) {
    std::map<std::string, std::vector<double> >::iterator ci;
    ci = m_parameterVector.find(name);
    if ( ci != m_parameterVector.end() ) {
        if( index < (*ci).second.size() ) {
            return m_parameterVector[name][index];
	} else {
	    return 0.;
	}   
    } else {
        return 0.;
    }	
}  

long  Parameter::getInt(const std::string& name) {
    std::map<std::string, double>::const_iterator ci;
    ci = m_parameter.find(name);
    if ( ci != m_parameter.end() ) {
        return toLong(m_parameter[name]);
    } else {
        return 0;
    }	
}
  
long  Parameter::getInt(const std::string& name, unsigned int index) {
    std::map<std::string, std::vector<double> >::const_iterator ci;
    ci = m_parameterVector.find(name);
    if ( ci != m_parameterVector.end() ) {
        if( index < (*ci).second.size() ) {
            return toLong(m_parameterVector[name][index]);
	} else {
	    return 0;
	}   
    } else {
        return 0;
    }
} 

std::vector<double> Parameter::getVector(const std::string& name) {
    std::map<std::string, std::vector<double> >::const_iterator ci;
    ci = m_parameterVector.find(name);
    if ( ci != m_parameterVector.end() ) {
        return m_parameterVector[name];
    } else {
        return std::vector<double>();
    }
}

std::vector<long> Parameter::getVectorInt(const std::string& name) {
    std::map<std::string, std::vector<double> >::const_iterator ci;
    ci = m_parameterVector.find(name);
    if ( ci != m_parameterVector.end() ) {
        std::vector<long> tmp;
	std::vector<double>::iterator it;

	for (it=m_parameterVector[name].begin();
             it != m_parameterVector[name].end(); it++) {
            tmp.push_back(toLong(*it));
	}
	return tmp;	
    } else {
        return std::vector<long>();
    }
}

long Parameter::sizeVector(const std::string& name) const {
    std::map<std::string, std::vector<double> >::const_iterator ci;
    ci = m_parameterVector.find(name);
    if ( ci != m_parameterVector.end() ) {
        return (*ci).second.size();
    } else {
        return 0;
    }	
}   
 

void Parameter::addParameter( double x, const std::string& name) {
    m_parameter[name] = x;
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
