#ifndef _STWAFER_
#define _STWAFER_ 1
#include <iostream>
#include <vector>
#include "GaudiKernel/MsgStream.h"

/**
STWafer is a class describing a rectangular field of strips
*/

class STWafer {

  public:
 
    /// Standard constructor.
    STWafer(double Pitch, int FirstStrip, unsigned int nWafers,
        unsigned int iStation, unsigned int iLayer, unsigned int iWafer,
        double UL, double UR, double VD, double VU, 
        double DZ, double deadWidth, double thickness);

    virtual ~STWafer() {}

    /// station strip is in
    unsigned int stationID() const;   

    /// layer strip is in
    unsigned int layerID() const;

    /// layer strip is in
    unsigned int waferID() const;

    /// Get strip number at the point (u,v)
    int stripNum(const double u, const double v) const ;

    /// Get number of nearest strip to the point (u,v)
    int nearestStripNum(const double u) const;

    /// Get u-coordinate of a strip
    double U(const int Strip) const ;

    /// Check if a point (u,v) is inside of this subdivision
    bool isInside(const double u, const double v) const;

    /// Check if a point (u,v) is inside of this subdivision with tol
    bool isInside(const double u, const double v, const double tolerance) const;

    bool isInsideFullDetail(const double u, const double v, double zLocal = 0.) const;

    /// Get number of strips in this subdivision
    int numStrips() const;
    
    /// Get number of last strip in this subdivision
    int lastStrip() const;
    
    /// Check whether the specified strip belongs to this subdivision
    bool isStrip(const int Strip) const;

    /// get u-coordinate (along strips) for a given strip
    double centerU() const;    

    /// get v-coordinate (along strips) for a given strip
    double centerV() const;

    /// get half length by u-coordinate for a given strip
    double halfLengthU() const;
    
    /// get half length by v-coordinate for a given strip
    double halfLengthV() const; 

    /// get DZ
    double dZ() const;
    
    /// get left U
    double leftU() const;
    
    /// get right U
    double rightU() const;
    
    /// get upper V
    double upperV() const;
    
    /// get lower V
    double lowerV() const;

    /// print 
    std::ostream& STWafer::printOut( std::ostream& os ) const;
    MsgStream& printOut( MsgStream& os) const;

 private:
    
    /// Strip pitch
    double m_Pitch;
    /// Number of first strip
    int m_FirstStrip;
    /// station
    unsigned int m_station;
    /// layer
    unsigned int m_layer;
    /// wafer id
    unsigned int m_wafer;
    /// Coordinate of a left edge
    double m_UL;
    /// Coordinate of a right edge
    double m_UR;
    /// Coordinate of an upper edge
    double m_VU;
    /// Coordinate of a lower edge
    double m_VD;
    /// Z-shift of a wafer relative to sector Z
    double m_DZ;
    /// Number of strips
    int m_NumStrips;

    // vector of centers of dead areas in u
    std::vector<double> m_deadRegions;

    // half size of dead regions
    double m_deadWidth;
  
    // wafer thickness
    double m_waferThickness;

};

inline unsigned int STWafer::stationID() const{
  return m_station;
}

inline unsigned int STWafer::layerID() const{
  return m_layer;
}

inline unsigned int STWafer::waferID() const{
  return m_wafer;
}

inline std::ostream& operator<<( std::ostream& os , const STWafer* aWafer )
{ return aWafer->printOut( os ); } 

inline MsgStream& operator<<( MsgStream& os , const STWafer* aWafer )
{ return aWafer->printOut( os ); } 

inline int STWafer::numStrips() const { 
  return m_NumStrips; 
} 
    
inline int STWafer::lastStrip() const { 
  return m_FirstStrip + m_NumStrips - 1; 
}    
    
inline bool STWafer::isStrip(const int Strip) const { 
  return (Strip >= m_FirstStrip && Strip < m_FirstStrip + m_NumStrips); 
}
    
inline double STWafer::centerV() const {  
  return 0.5*(m_VU + m_VD); 
}

inline double STWafer::centerU() const {  
  return 0.5*(m_UL + m_UR); 
}

inline double STWafer::halfLengthU() const { 
  return 0.5*(m_UR - m_UL); 
}
    
inline double STWafer::halfLengthV() const { 
  return 0.5*(m_VU - m_VD); 
} 

inline double STWafer::dZ() const { 
  return m_DZ; 
}
    
inline double STWafer::leftU() const { 
  return m_UL; 
}
    
inline double STWafer::rightU() const { 
  return m_UR; 
}
    
    
inline double STWafer::upperV() const { 
  return m_VU; 
}
      
inline double STWafer::lowerV() const { 
  return m_VD; 
}

#endif









