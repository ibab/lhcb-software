#ifndef STHisto2DProperties_H 
#define STHisto2DProperties_H 1

/** @class STHisto2DProperties STHisto2DProperties
 *
 *  class to hold 2-D histogram properties
 *
 *  @author Johan Luisier
 *  @author M Needham
 *  @date   2009-07-22
 */

#include "Kernel/STChannelID.h"
#include <string>


namespace ST
{
  class Histo2DProperties {

    //protected:

public:

    Histo2DProperties(const std::string& name, const std::string& title,
                   const unsigned int nbinx, const unsigned int nbiny,
                   const double xMin, const double xMax,
                   const double yMin, const double yMax);

    virtual ~Histo2DProperties();


    const double& minBinX() const;

    const double& minBinY() const;

    const double& maxBinX() const;

    const double& maxBinY() const;

    const unsigned int& nBinX() const;

    const unsigned int& nBinY() const;

    const std::string& title() const;

    const std::string& name() const;

private:

    std::string m_name;
    std::string m_title;
    unsigned int m_nBinX, m_nBinY;
    double m_xMin, m_xMax, m_yMin, m_yMax;
    
         
  };


inline Histo2DProperties::~Histo2DProperties(){
  // destructor
}

inline Histo2DProperties::Histo2DProperties(const std::string& name, const std::string& title,
                                                    const unsigned int nbinx, const unsigned int nbiny,
                                                    const double xMin, const double xMax,
                                                    const double yMin, const double yMax):
m_name(name),
m_title(title),
m_nBinX(nbinx),
m_nBinY(nbiny),
m_xMin(xMin),
m_xMax(xMax),
m_yMin(yMin),
m_yMax(yMax){
  // constructor
}

inline const double& Histo2DProperties::minBinX() const
{
  return m_xMin;
}

inline const double& Histo2DProperties::maxBinX() const
{
  return m_xMax;
}

inline const double& Histo2DProperties::minBinY() const
{
  return m_yMin;
}

inline const double& Histo2DProperties::maxBinY() const
{
  return m_yMax;
}

inline const unsigned int& Histo2DProperties::nBinX() const
{
  return m_nBinX;
}

inline const unsigned int& Histo2DProperties::nBinY() const
{
  return m_nBinY;
}

inline const std::string& Histo2DProperties::title() const
{
  return m_title;
}

inline const std::string& Histo2DProperties::name() const
{
  return m_name;
}

} // namespace ST

#endif // STDETECTORPLOT_H 
