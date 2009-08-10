// $Id: Digitalizer.h,v 1.1 2009-08-10 08:48:01 szumlat Exp $
#ifndef DIGITALIZER_H 
#define DIGITALIZER_H 1

// Include files

/** @class Digitalizer Digitalizer.h Kernel/Digitalizer.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-05-22
 */

template <class T>
class Digitalizer {
public: 
  /// Standard constructor
  Digitalizer(const double adcMax, const double elMax):
    m_adcMax ( adcMax ),
    m_elMax ( elMax ) 
  { } 

  virtual ~Digitalizer( ) { }   ///< Destructor
  signed int operator()(T& inCharge)
  {
    signed int adc=static_cast<T>((inCharge/m_elMax)*m_adcMax);
    if(adc>(m_adcMax-1)) adc=static_cast<signed int>(m_adcMax-1);
    return ( adc );
  }

protected:

  Digitalizer() 
  { }

private:

  double m_adcMax;
  double m_elMax;

};
#endif // DIGITALIZER_H
