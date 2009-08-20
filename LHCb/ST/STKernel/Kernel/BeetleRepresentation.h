// $Id: BeetleRepresentation.h,v 1.3 2009-08-20 07:58:46 mneedham Exp $
#ifndef _STDAQ_BeetleRepresentation_H
#define _STDAQ_BeetleRepresentation_H 1


/** @class BeetleRepresentaiton BeetleRepresentation.h  STDAQ/BeetleRepresentation
*
* Sometimes we number a strip on a tell1 board 0 - 3072
* Sometimes we want to compose/decompose this into Beetles and ports
* This helper class does just that.
*
*  @author M.Needham
*  @date   17/8/2008
*/

#include "Kernel/LHCbConstants.h"
#include <iostream>
#include "StripRepresentation.h"

namespace STDAQ{

  class BeetleRepresentation{

  public:

    /** constructor from:
    * @param unsigned int beetle [0 - 23]
    * @param unsigned int port [0 - 3]
    * @param unsigned int strip [0 - 31]  
    **/
    BeetleRepresentation(unsigned int beetle,
                         unsigned int port = 0,
                         unsigned int strip = 0); 

    /** constructor  from int */
    explicit BeetleRepresentation(const StripRepresentation& strip); 

    /** destructor */
    ~BeetleRepresentation(){}

    /** decompose into beetle, port, strip **/
    void decompose(unsigned int& beetle,
                   unsigned int& port,
                   unsigned int& strip) const;

    /** decompose into beetle and strip **/
    void decompose(unsigned int& beetle,
                   unsigned int& strip) const;

    /** cast to int */
    operator int() const;

    /** return the value */
    unsigned int value() const;

    /** value as StripRepresentation */
    StripRepresentation toStripRepresentation() const;

    /** Operator overloading for stringoutput */
    friend std::ostream& operator<< (std::ostream& s, 
                                     const STDAQ::BeetleRepresentation& obj)
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

inline STDAQ::BeetleRepresentation::BeetleRepresentation(unsigned int beetle,
                                                         unsigned int port,
				                         unsigned int strip){
  m_value = (LHCbConstants::nStripsInBeetle*beetle) 
	  + (LHCbConstants::nStripsInPort *port) + strip; 
}

inline STDAQ::BeetleRepresentation::BeetleRepresentation(const STDAQ::StripRepresentation& strip):m_value(strip.value()){
  // constructor
}

inline STDAQ::BeetleRepresentation::operator int() const {
  return m_value;
}

inline unsigned int STDAQ::BeetleRepresentation::value() const{
  return m_value;
}

inline STDAQ::StripRepresentation STDAQ::BeetleRepresentation::toStripRepresentation() const{
  return STDAQ::StripRepresentation(m_value);
}


inline void STDAQ::BeetleRepresentation::decompose(unsigned int& beetle,
                                              unsigned int& port,
                                              unsigned int& strip) const{

  unsigned beetleStrip;
  decompose(beetle,beetleStrip); // turn into Beetle and strip

  port = beetleStrip/LHCbConstants::nStripsInPort;
  strip = beetleStrip%LHCbConstants::nStripsInPort;
} 

inline void STDAQ::BeetleRepresentation::decompose(unsigned int& beetle,
                                                   unsigned int& strip) const{
  beetle = m_value/LHCbConstants::nStripsInBeetle;
  strip = m_value%LHCbConstants::nStripsInBeetle;
} 

inline std::ostream& STDAQ::BeetleRepresentation::fillStream(std::ostream& s) const {
   unsigned int beetle, port, strip;
   decompose(beetle, port, strip);
   s << "{ "
    << " BeetleRepresentation: \t" << value() << std::endl
    << " Beetle: " << beetle << " Port: " << port 
    << "Strip: " << strip << std::endl << "  } ";

  return s;
}


#include <sstream>

inline std::string  STDAQ::BeetleRepresentation::toString() const{
  std::ostringstream o;
  fillStream(o);
  return o.str();
}

#endif //  _STDAQ_BeetleRepresentation_H
 
