
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef VeloTrack_H
#define VeloTrack_H 1

// Include files

#include <vector>
#include "Kernel/Point3DTypes.h"

#include "GaudiKernel/MsgStream.h"

//#include "GaudiKernel/KeyedObject.h"
//#include "GaudiKernel/KeyedContainer.h"

class VeloTrack
{
 public:


    
   /// Pair of XYZPoints, valid coordinates in the first and errors and station number in the second
   typedef std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> alignPoint;
  
    /// Constructor with allocation of vector sizes
    VeloTrack();
  
    /// Default Destructor
    virtual ~VeloTrack() {}
  
    /// Clear the AlignTrack before use
    void reset();
  
    /// Add a coordinate to the track
    void addCoord(const Gaudi::XYZPoint& coord1,
                  const Gaudi::XYZPoint& coord2);
  
    /// Retrieve const  Event Number
    int nEvent() const;
  
    /// Update  Event Number
    void setNEvent(int value);
  
    /// Retrieve const  Track Number
    int nTrack() const;
  
    /// Update  Track Number
    void setNTrack(int value);
  
    /// Retrieve const  Track Type (0=Left 1=Right 2=Overlap)
    int nType() const;
  
    /// Update  Track Type (0=Left 1=Right 2=Overlap)
    void setNType(int value);
  
    /// Retrieve const  Number of valid coordinates
    int nGoodCoordinates() const;
  
    /// Update  Number of valid coordinates
    void setNGoodCoordinates(int value);
  
    /// Retrieve const  Number of the first station with a valid coordinate
    int nFirstStation() const;
  
    /// Update  Number of the first station with a valid coordinate
    void setNFirstStation(int value);
  
    /// Retrieve const  X slope of the track
    double nSlope_x() const;
  
    /// Update  X slope of the track
    void setNSlope_x(double value);
  
    /// Retrieve const  Error on X slope
    double nErrX_x() const;
  
    /// Update  Error on X slope
    void setNErrX_x(double value);
  
    /// Retrieve const  Y slope of the track
    double nSlope_y() const;
  
    /// Update  Y slope of the track
    void setNSlope_y(double value);
  
    /// Retrieve const  Error on Y slope
    double nErrY_y() const;
  
    /// Update  Error on Y slope
    void setNErrY_y(double value);
  
    /// Retrieve const  X track value at z=0
    double nXo_x() const;
  
    /// Update  X track value at z=0
    void setNXo_x(double value);
  
    /// Retrieve const  Y track value at z=0
    double nYo_y() const;
  
    /// Update  Y track value at z=0
    void setNYo_y(double value);
  
    /// Retrieve const  Track Z at closest approach
    double nZclos() const;
  
    /// Update  Track Z at closest approach
    void setNZclos(double value);
  
    /// Retrieve const  Track already belongs to a primary vertex
    bool nPVtag() const;
  
    /// Update  Track already belongs to a primary vertex
    void setNPVtag(bool value);
  
    /// Retrieve const  Reference number of the primary vertex
    int nPVnumber() const;
  
    /// Update  Reference number of the primary vertex
    void setNPVnumber(int value);
  
    /// Retrieve const  X coordinate of the track corresp. PV
    double nPV_x() const;
  
    /// Update  X coordinate of the track corresp. PV
    void setNPV_x(double value);
  
    /// Retrieve const  Y coordinate of the track corresp. PV
    double nPV_y() const;
  
    /// Update  Y coordinate of the track corresp. PV
    void setNPV_y(double value);
  
    /// Retrieve const  Z coordinate of the track corresp. PV
    double nPV_z() const;
  
    /// Update  Z coordinate of the track corresp. PV
    void setNPV_z(double value);
  
    /// Retrieve const  Chi-square the corresp. PV
    double nPV_chi() const;
  
    /// Update  Chi-square the corresp. PV
    void setNPV_chi(double value);
  
    /// Retrieve const  Is the track good or not (acc. to our selection criteria) ?
    bool nIsGood() const;
  
    /// Update  Is the track good or not (acc. to our selection criteria) ?
    void setNIsGood(bool value);
  
    /// Retrieve const  Container with valid alignPoints
    const std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> >& Coords() const;
  
    /// Update  Container with valid alignPoints
    void setCoords(const std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> >& value);
  

  protected:

  private:

    int                                                      m_nEvent;           ///< Event Number
    int                                                      m_nTrack;           ///< Track Number
    int                                                      m_nType;            ///< Track Type (0=Left 1=Right 2=Overlap)
    int                                                      m_nGoodCoordinates; ///< Number of valid coordinates
    int                                                      m_nFirstStation;    ///< Number of the first station with a valid coordinate
    double                                                   m_nSlope_x;         ///< X slope of the track
    double                                                   m_nErrX_x;          ///< Error on X slope
    double                                                   m_nSlope_y;         ///< Y slope of the track
    double                                                   m_nErrY_y;          ///< Error on Y slope
    double                                                   m_nXo_x;            ///< X track value at z=0
    double                                                   m_nYo_y;            ///< Y track value at z=0
    double                                                   m_nZclos;           ///< Track Z at closest approach
    bool                                                     m_nPVtag;           ///< Track already belongs to a primary vertex
    int                                                      m_nPVnumber;        ///< Reference number of the primary vertex
    double                                                   m_nPV_x;            ///< X coordinate of the track corresp. PV
    double                                                   m_nPV_y;            ///< Y coordinate of the track corresp. PV
    double                                                   m_nPV_z;            ///< Z coordinate of the track corresp. PV
    double                                                   m_nPV_chi;          ///< Chi-square the corresp. PV
    bool                                                     m_nIsGood;          ///< Is the track good or not (acc. to our selection criteria) ?
    std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> > m_Coords;           ///< Container with valid alignPoints
  
  }; // class VeloTrack
  

   /// typedef for std::vector of AlignTrack
   typedef std::vector<VeloTrack> VeloTracks;


// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline VeloTrack::VeloTrack() 
{

          m_Coords.reserve(42);
          reset();         
        
}


inline int VeloTrack::nEvent() const 
{
  return m_nEvent;
}

inline void VeloTrack::setNEvent(int value) 
{
  m_nEvent = value;
}

inline int VeloTrack::nTrack() const 
{
  return m_nTrack;
}

inline void VeloTrack::setNTrack(int value) 
{
  m_nTrack = value;
}

inline int VeloTrack::nType() const 
{
  return m_nType;
}

inline void VeloTrack::setNType(int value) 
{
  m_nType = value;
}

inline int VeloTrack::nGoodCoordinates() const 
{
  return m_nGoodCoordinates;
}

inline void VeloTrack::setNGoodCoordinates(int value) 
{
  m_nGoodCoordinates = value;
}

inline int VeloTrack::nFirstStation() const 
{
  return m_nFirstStation;
}

inline void VeloTrack::setNFirstStation(int value) 
{
  m_nFirstStation = value;
}

inline double VeloTrack::nSlope_x() const 
{
  return m_nSlope_x;
}

inline void VeloTrack::setNSlope_x(double value) 
{
  m_nSlope_x = value;
}

inline double VeloTrack::nErrX_x() const 
{
  return m_nErrX_x;
}

inline void VeloTrack::setNErrX_x(double value) 
{
  m_nErrX_x = value;
}

inline double VeloTrack::nSlope_y() const 
{
  return m_nSlope_y;
}

inline void VeloTrack::setNSlope_y(double value) 
{
  m_nSlope_y = value;
}

inline double VeloTrack::nErrY_y() const 
{
  return m_nErrY_y;
}

inline void VeloTrack::setNErrY_y(double value) 
{
  m_nErrY_y = value;
}

inline double VeloTrack::nXo_x() const 
{
  return m_nXo_x;
}

inline void VeloTrack::setNXo_x(double value) 
{
  m_nXo_x = value;
}

inline double VeloTrack::nYo_y() const 
{
  return m_nYo_y;
}

inline void VeloTrack::setNYo_y(double value) 
{
  m_nYo_y = value;
}

inline double VeloTrack::nZclos() const 
{
  return m_nZclos;
}

inline void VeloTrack::setNZclos(double value) 
{
  m_nZclos = value;
}

inline bool VeloTrack::nPVtag() const 
{
  return m_nPVtag;
}

inline void VeloTrack::setNPVtag(bool value) 
{
  m_nPVtag = value;
}

inline int VeloTrack::nPVnumber() const 
{
  return m_nPVnumber;
}

inline void VeloTrack::setNPVnumber(int value) 
{
  m_nPVnumber = value;
}

inline double VeloTrack::nPV_x() const 
{
  return m_nPV_x;
}

inline void VeloTrack::setNPV_x(double value) 
{
  m_nPV_x = value;
}

inline double VeloTrack::nPV_y() const 
{
  return m_nPV_y;
}

inline void VeloTrack::setNPV_y(double value) 
{
  m_nPV_y = value;
}

inline double VeloTrack::nPV_z() const 
{
  return m_nPV_z;
}

inline void VeloTrack::setNPV_z(double value) 
{
  m_nPV_z = value;
}

inline double VeloTrack::nPV_chi() const 
{
  return m_nPV_chi;
}

inline void VeloTrack::setNPV_chi(double value) 
{
  m_nPV_chi = value;
}

inline bool VeloTrack::nIsGood() const 
{
  return m_nIsGood;
}

inline void VeloTrack::setNIsGood(bool value) 
{
  m_nIsGood = value;
}

inline const std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> >& VeloTrack::Coords() const 
{
  return m_Coords;
}

inline void VeloTrack::setCoords(const std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> >& value) 
{
  m_Coords = value;
}

inline void VeloTrack::reset() 
{

        m_nEvent = 0;
        m_nTrack = 0;
        m_nType = 0;
        m_nGoodCoordinates = 0;
        m_nFirstStation = 0;
        m_nIsGood = true;
        m_Coords.clear();
        
}

inline void VeloTrack::addCoord(const Gaudi::XYZPoint& coord1,
                                       const Gaudi::XYZPoint& coord2) 
{

        alignPoint my_point(coord1,coord2);
        m_Coords.push_back(my_point);
        
}


#endif ///VeloAlignment_AlignTrack_H
