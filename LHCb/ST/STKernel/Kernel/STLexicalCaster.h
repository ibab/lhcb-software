#ifndef ST_STLEXICALCASTER_H
#define ST_STLEXICALCASTER_H

#include "boost/lexical_cast.hpp"

namespace ST{

  /**
   * Template converter from basic anytype to string
   * @param type the number you want to convert
   * @param digits the number of desired digits (ie '1' -> "001" if digits is 3
   * @return the wanted string
   *
   * @author Johan Luisier
   * @date 2008-10-14
   */
  template <typename T>
  std::string toString(const T& type, const unsigned int& digits = 0)
  {
    std::string result(boost::lexical_cast<std::string>(type));
    if (digits != 0)
    {
      while(result.size() < digits)
        result = "0" + result;
    }
    return result;
  };

  /**
   * Template converter from string to any numeric type.
   * @param m_string the string which has to be converted
   * @param type the variable where the value will be stored
   *
   * @author Johan Luisier
   * @date 2008-10-14
   */
  template <typename T>
  void fromString(const std::string& m_string, T& type)
  {
    type = boost::lexical_cast<T>(m_string);
  }

}

#endif //ST_STLEXICALCASTER_H
