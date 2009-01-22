// $Id: PPRepresentation.h,v 1.3 2009-01-22 11:43:44 mneedham Exp $
#ifndef _STDAQ_PPRepresentation_H
#define _STDAQ_PPRepresentation_H 1


/** @class PPRepresentaiton PPRepresentation.h  STDAQ/PPRepresentation
*
* Sometimes we number a strip on a tell1 board 0 - 3072
* Sometimes we want to compose/decompose this into Beetles and ports
* This helper class does just that.
*
*  @author M.Needham
*  @date   17/8/2008
*/

#include "Kernel/STDAQDefinitions.h"
#include "Kernel/LHCbConstants.h"
#include "Kernel/StripRepresentation.h"
#include <iostream>

namespace STDAQ{


  class PPRepresentation{

  public:

    /** constructor from:
    * @param unsigned int pp [0 - 3]
    * @param unsigned int beetle [0 - 5]
    * @param unsigned int port [0 - 3]
    * @param unsigned int strip [0 - 31]  
    **/
    PPRepresentation(unsigned int pp,
                     unsigned int beetle = 0,
                     unsigned int port = 0,
                     unsigned int strip = 0); 

    /** constructor  from int */
    explicit PPRepresentation(const STDAQ::StripRepresentation& strip); 

    /** destructor */
    ~PPRepresentation(){}

    /** decompose into beetle, port, strip **/
    void decompose(unsigned int& pp, 
                   unsigned int& beetle,
                   unsigned int& port,
                   unsigned int& strip) const;

    /** cast to int */
    operator int() const;

    /** return the value */
    unsigned int value() const;

    /** value as StripRepresentation */
    STDAQ::StripRepresentation toStripRepresentation() const;

    /** Operator overloading for stringoutput */
    friend std::ostream& operator<< (std::ostream& s, 
                                     const STDAQ::PPRepresentation& obj)
    {
      return obj.fillStream(s);
    }

    /** Fill the ASCII output stream **/ 
    std::ostream& fillStream(std::ostream& s) const;

    /** print method for python, not needed in C++ **/
    std::string toString() const;

  private:

    unsigned int m_value;

  };

}

//#include "Kernel/LHCbConstants.h"

inline STDAQ::PPRepresentation::PPRepresentation(unsigned int pp,
                                                 unsigned int beetle,
                                                 unsigned int port,
				                 unsigned int strip){
  m_value = (STDAQ::nStripPerPPx*pp) + (LHCbConstants::nStripsInBeetle*beetle) 
	  + (LHCbConstants::nStripsInPort *port) + strip; 
}

inline STDAQ::PPRepresentation::PPRepresentation(const STDAQ::StripRepresentation& strip):m_value(strip.value()){
  // constructor
}

inline STDAQ::PPRepresentation::operator int() const {
  return m_value;
}

inline unsigned int STDAQ::PPRepresentation::value() const{
  return m_value;
}

inline void STDAQ::PPRepresentation::decompose(unsigned int& pp,
                                              unsigned int& beetle,
                                              unsigned int& port,
                                              unsigned int& strip) const{
  pp = m_value/STDAQ::nStripPerPPx; 
  beetle = m_value/LHCbConstants::nStripsInBeetle;
  const unsigned int ppStrip = m_value%STDAQ::nStripPerPPx;
  beetle = ppStrip/LHCbConstants::nStripsInBeetle;
  const unsigned beetleStrip = ppStrip%LHCbConstants::nStripsInBeetle;
  port = beetleStrip/LHCbConstants::nStripsInPort;
  strip = beetleStrip%LHCbConstants::nStripsInPort;
} 

inline STDAQ::StripRepresentation STDAQ::PPRepresentation::toStripRepresentation() const{
  return STDAQ::StripRepresentation(m_value);
}

inline std::ostream& STDAQ::PPRepresentation::fillStream(std::ostream& s) const {
   unsigned int pp, beetle, port, strip;
   decompose(pp, beetle, port, strip);
   s << "{ "
    << " PPRepresentation: \t" << value() << std::endl
    << " PP " << pp  << " Beetle: " << beetle << 
       " Port: " << port << "Strip: " << strip << std::endl << "  } ";

  return s;
}

#include <sstream>

inline std::string STDAQ::PPRepresentation::toString() const{
  std::ostringstream o;
  fillStream(o);
  return o.str();
}


#endif //  _STDAQ_PPRepresentation_H
 
