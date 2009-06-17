// $Id: ErrorHandler.h,v 1.5 2009-06-17 16:38:27 jucogan Exp $
#ifndef L0MUONKERNEL_ERRORHANDLER_H 
#define L0MUONKERNEL_ERRORHANDLER_H 1

// Include files
#include <iostream>
#include "boost/format.hpp"


namespace L0Muon {

  /** @class ErrorHandler ErrorHandler.h L0MuonKernel/ErrorHandler.h
  
  Helper class to handle an error found during the decoding of L0Muon banks.

  It contains :
  - the value of the evaluated field
  - a flag indicating if the field was in error
  - a counter indicating thenumber of time the field was detected in error
  - method to display the error status and counter

  @author Julien Cogan
  @date   2008-03-27
  */
  class ErrorHandler {
  
  public:
  
    /// Default constructor
    ErrorHandler();

    /// Constructor with format and mask
    ErrorHandler(std::string fmt, int mask);

    /// Constructor with format, mask and name
    ErrorHandler(std::string fmt, int mask, std::string name);

    /// Destructor
    virtual ~ErrorHandler();
    
    /** Method to set a value.
        
        The given value is stored in the #m_value attribut. 
        It is compared to the given reference using the #m_mask attribute (defined in the constructor). 
        It they don't match, the #m_inError flag is raised and the #m_counter counter is incremented. 

        @param value : value to be set
        @param ref (optional, default is 0) : reference which the value is compared to 
    */
    virtual void set(int value, int ref=0) {
      m_value= value;
      m_inError=false;
      if ((m_value&m_mask)!=(ref&m_mask)) {
        m_inError=true;
        ++m_counter;
        //std::cout<<m_name<<m_value<<m_counter<<std::endl;
      }
    }
    
    /// Format for output of current value 
    const std::string fmt() const { return m_fmt;}
    
    /// Current value
    int value() const { return m_value;}
    
    /// True if in error (value does not match reference)
    bool inError() const { return m_inError;}
    
    /// Error counter (incremented each time the value did not match the reference)
    int counter() const {return m_counter;}

    /** Fill the input string with the counter value and the given name (formatted output)

        @param os   : input string 
        @param name : error name to put in the input string 
     */
    void printCounter(std::string& os,std::string name)  const { 
      if (m_counter>0){
        os +=  (boost::format("%-30s  : %3d\n") % name % m_counter).str();
      }
    }

    /** Fill the input string with the counter value. Return true if the counter is not zero.

        @param os   : input string 
    */
    bool strCounter(std::string& os)  const { 
      if (m_counter>0){
        os =  (boost::format("%-20s : %3d") % m_name % m_counter).str();
        return true;
      }
      return false;
    }

  private:
  
    int  m_value;    ///< current value
    bool m_inError;  ///< flag set to true if the value does not match the reference
    int  m_counter;  ///< counter incremented each time the value did not match the reference

    std::string m_name; ///< error name (for output) 
    std::string m_fmt;  ///< value output format  
    int m_mask;         ///< mask apply to value and reference during comparison
    
  };
  
  std::ostream &operator<<(std::ostream &os, const L0Muon::ErrorHandler &field);

} // namespace L0Muon


#endif // L0MUONKERNEL_ERRORHANDLER_H
