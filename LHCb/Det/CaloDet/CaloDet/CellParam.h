// $Id: CellParam.h,v 1.13 2009-08-05 14:24:35 ibelyaev Exp $
// ============================================================================
#ifndef CALODET_CELLPARAM_H 
#define CALODET_CELLPARAM_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/PhysicalConstants.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/CaloCellID.h"
// ============================================================================
class DeCalorimeter;
// ============================================================================
typedef LHCb::CaloCellID::Vector CaloNeighbors ;
// ============================================================================
/** @class  CellParam CellParam.h CaloDet/CellParam.h
 *
 *   Auxilliary class to store cell's information inside 
 *   the detector element
 *   The constructor creates a null cellID if no argument, as a default
 *   creator is needed for CaloVector...
 *
 */
// ============================================================================
namespace CaloCellQuality
{
  // ==========================================================================
  enum Flag 
    {
    OK            = 0 , 
    Dead          = 1 ,
    Noisy         = 2 ,
    Shifted       = 4 ,
    DeadLED       = 8,
    VeryNoisy     = 16,
    VeryShifted   = 32
  } ;
  // ==========================================================================
}
// ============================================================================
class CellParam 
{ 
  friend class DeCalorimeter; 
public:
  /// standard constructor 
  CellParam( const LHCb::CaloCellID& id  = LHCb::CaloCellID() ) ;
  /// destructor 
  ~CellParam();
  bool                  valid         () const { return m_valid      ; }   
  LHCb::CaloCellID      cellID        () const { return m_cellID        ; }
  double                x             () const { return m_center.x()    ; }
  double                y             () const { return m_center.y()    ; }
  double                z             () const { return m_center.z() + m_zShower   ; }
  const Gaudi::XYZPoint center        () const { return Gaudi::XYZPoint( x(),y(), z() ) ; }
  double                size          () const { return m_size          ; }
  double                sine          () const { return m_sine          ; }
  double                nominalGain   () const { return m_gain          ; }
  double                time          () const { return m_time+m_dtime  ; }
  int                   cardNumber    () const { return m_cardNumber    ; }
  int                   cardRow       () const { return m_cardRow       ; }
  int                   cardColumn    () const { return m_cardColumn    ; }
  double                deltaTime     () const { return m_dtime         ; }
  double                zShower       () const { return m_zShower       ; }
  int                   quality       () const { return m_quality       ; }
  int                   l0Constant    () const { return m_l0constant    ; }
  double                calibration   () const { return m_calibration   ; } // absolute calibration from 'Calibration' condition (T0)
  double                ledDataRef    () const { return m_ledDataRef    ; } // <LED> data from Calibration condition (Ref T0)
  double                ledMoniRef    () const { return m_ledMoniRef    ; } // <LED/PIN> data from Calibration condition (Ref T0)
  double                ledData       () const { return m_ledData       ; } // <LED> data from Quality condition (current T)
  double                ledDataRMS    () const { return m_ledDataRMS    ; } // RMS(LED) from Quality condition  (current T)
  double                ledMoni       () const { return m_ledMoni       ; } // <LED/PIN> data from Quality condition (current T)
  double                ledMoniRMS    () const { return m_ledMoniRMS    ; } // RMS(LED/PIN) from Quality condition (current T)
  double                ledDataShift  () const { return ( ledDataRef() > 0 ) ? ledData()/ledDataRef() : 1; }
  double                gainShift     () const { return ( ledMoniRef() > 0 ) ? ledMoni()/ledMoniRef() : 1; }
  double                gain          () const { return nominalGain() * calibration() * gainShift() ;}  
  int                   numericGain   () const { return m_nGain         ; }  // for Prs only
    

  const std::vector<LHCb::CaloCellID>& pins() const  { return m_pins ;}
  const std::vector<int>& leds() const  { return m_leds ;}
  const CaloNeighbors& neighbors     () const { return m_neighbors     ; }
  const CaloNeighbors& zsupNeighbors () const { return m_zsupNeighbors ; }
  
  // ** To initialize the cell: Geometry, neighbours, gain
  
  void  setValid( bool valid ) { m_valid = valid ;  }

  void  setCenterSize( const Gaudi::XYZPoint& point, double S) {
    m_center = point; 
    m_size   = S; 
    //    m_sine   = sqrt( (point.x()*point.x() + point.y()*point.y()) / 
    //                 point.mag2() ); 
    m_sine = 0;
    if(point.R() != 0)m_sine   = point.Rho()/point.R();  // MathCore methods (Cartesian3D)
    m_time   = point.R() /Gaudi::Units::c_light *Gaudi::Units::ns ; //R=sqrt(Mag2)
  }
  void setDeltaTime(double dtime){ m_dtime = dtime; }
  void setZshower(double dz){ m_zShower = dz; }

  void addZsupNeighbor( const LHCb::CaloCellID& ID) { 
    m_zsupNeighbors.push_back(ID);
  }
  void addNeighbor    ( const LHCb::CaloCellID& ID) { m_neighbors.push_back(ID); }
  void setNominalGain ( const double gain   ) { m_gain = gain            ; }
  void setFeCard      ( const int num, const int relCol, const int relRow ) {
    m_cardNumber  = num;
    m_cardColumn  = relCol;
    m_cardRow     = relRow;
  }
  void addPin(LHCb::CaloCellID id){ m_pins.push_back(id) ;}
  void addLed(int id){ m_leds.push_back(id) ;}

  // Calibration & quality
  void addQualityFlag   (int quality)           {m_quality = m_quality | quality; }
  void setLedData       (double ledData, double ledDataRMS ){
    m_ledData    = ledData ; 
    m_ledDataRMS = ledDataRMS;  
  }
  void setLedMoni(double ledMoni, double ledMoniRMS){
    m_ledMoni     = ledMoni ; 
    m_ledMoniRMS  = ledMoniRMS;  
  }
  void setLedDataRef   (double ledDataRef, double ledMoniRef){
    m_ledDataRef = ledDataRef ; 
    m_ledMoniRef = ledMoniRef;
  }
  void setCalibration  (double calib)          { m_calibration = calib;           }
  void setL0Constant   (int    cte)            { m_l0constant  = cte;             }
  void setNumericGain  (int    ng)             {  m_nGain = ng        ; }  // for Prs only

  
  bool operator==( const CellParam& c2 ) const { 
    return center() == c2.center() && size() == c2.size(); }
  
private:
  
  LHCb::CaloCellID    m_cellID         ; ///< ID of the cell
  double        m_size           ; ///< Cell size
  Gaudi::XYZPoint m_center       ; ///< Cell centre
  double        m_sine           ; ///< To transform E to Et
  double        m_gain           ; ///< MeV per ADC count
  double        m_time           ; ///< Nominal time of flight from Vertex (ns)
  int           m_cardNumber     ; ///< Front-end card number 
  int           m_cardRow        ; ///< card row and column
  int           m_cardColumn     ;
  CaloNeighbors m_neighbors      ; ///< List of neighbors
  CaloNeighbors m_zsupNeighbors  ; ///< List of neighbors in same area
  bool m_valid;
  std::vector<LHCb::CaloCellID> m_pins;
  std::vector<int> m_leds;
  double m_dtime;
  double m_zShower;
  int    m_quality;
  double m_calibration;
  int    m_l0constant;
  double m_ledDataRef;
  double m_ledMoniRef;
  double m_ledData;
  double m_ledMoni;
  double m_ledDataRMS;
  double m_ledMoniRMS;
  int    m_nGain; // numeric gains (for Prs only)
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALODET_CELLPARAM_H
// ============================================================================
