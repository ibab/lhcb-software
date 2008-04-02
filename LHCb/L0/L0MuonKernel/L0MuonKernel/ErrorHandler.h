// $Id: ErrorHandler.h,v 1.1 2008-04-02 12:45:49 jucogan Exp $
#ifndef L0MUONKERNEL_ERRORHANDLER_H 
#define L0MUONKERNEL_ERRORHANDLER_H 1

// Include files
#include <iostream>
#include "boost/format.hpp"

/** @class ErrorHandler ErrorHandler.h L0MuonKernel/ErrorHandler.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-03-27
 */

namespace L0Muon {

  class ErrorHandler {
  
  public:
  
    ErrorHandler();
    ErrorHandler(std::string fmt, int mask);

    virtual ~ErrorHandler();

    virtual void set(int value, int ref=0) {
      m_value= (value&m_mask);
      m_inError=false;
      if (m_value!=(ref&m_mask)) {
        m_inError=true;
        ++m_counter;
      }
    }

    const std::string fmt() const { return m_fmt;}
    const int value() const { return m_value;}
    const int inError() const { return m_inError;}
    const int counter() const {return m_counter;}

    void printCounter(std::string& os,std::string name)  const { 
      if (m_counter>0){
        os +=  (boost::format("%-30s  : %3d\n") % name % m_counter).str();
      }
    }


  private:
  
    int  m_value;
    bool m_inError;
    int  m_counter;

    std::string m_fmt;
    int m_mask;
    
  };
  
  std::ostream &operator<<(std::ostream &os, const L0Muon::ErrorHandler &field);

} // namespace L0Muon


#endif // L0MUONKERNEL_ERRORHANDLER_H
