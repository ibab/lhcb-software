/// ===========================================================================
// $Id: CellParam.h,v 1.4 2006-05-17 16:01:55 cattanem Exp $
#ifndef CALODET_CELLPARAM_H 
#define CALODET_CELLPARAM_H 1
/// ===========================================================================

// Include files
#include "GaudiKernel/PhysicalConstants.h"

class DeCalorimeter;

typedef std::vector<LHCb::CaloCellID> CaloNeighbors ;

/** @class  CellParam CellParam.h CaloDet/CellParam.h
 *
 *   Auxilliary class to store cell's information inside 
 *   the detector element
 *   The constructor creates a null cellID if no argument, as a default
 *   creator is needed for CaloVector...
 *
 */


class CellParam 
{
  
  friend class DeCalorimeter;
  
public:
  
  /// standard constructor 
  CellParam( const LHCb::CaloCellID& id  = LHCb::CaloCellID() ) ;
  
  /// destructor 
  ~CellParam();
  
  bool                 valid         () const { return size() > 0      ; }   
  LHCb::CaloCellID           cellID        () const { return m_cellID        ; }
  double               x             () const { return m_center.x()    ; }
  double               y             () const { return m_center.y()    ; }
  double               z             () const { return m_center.z()    ; }
  const Gaudi::XYZPoint&  center     () const { return m_center        ; }
  double               size          () const { return m_size          ; }
  double               sine          () const { return m_sine          ; }
  double               gain          () const { return m_gain          ; }
  double               time          () const { return m_time          ; }
  int                  cardNumber    () const { return m_cardNumber    ; }
  int                  cardRow       () const { return m_cardRow       ; }
  int                  cardColumn    () const { return m_cardColumn    ; }
  
  const CaloNeighbors& neighbors     () const { return m_neighbors     ; }
  const CaloNeighbors& zsupNeighbors () const { return m_zsupNeighbors ; }
  
  // ** To initialize the cell: Geometry, neighbours, gain
  
  void  setCenterSize( const Gaudi::XYZPoint& point, double S) {
    m_center = point; 
    m_size   = S; 
    //    m_sine   = sqrt( (point.x()*point.x() + point.y()*point.y()) / 
    //                 point.mag2() ); 
    m_sine   = point.Rho()/point.R();  // MathCore methods (Cartesian3D)
    m_time   = point.R() /Gaudi::Units::c_light *Gaudi::Units::ns; //R=sqrt(Mag2)
  }
  
  void addZsupNeighbor( const LHCb::CaloCellID& ID) { 
    m_zsupNeighbors.push_back(ID);
  }
  void addNeighbor    ( const LHCb::CaloCellID& ID) { m_neighbors.push_back(ID); }
  void setGain        ( const double gain   ) { m_gain = gain            ; }
  void setFeCard      ( const int num, const int relCol, const int relRow ) {
    m_cardNumber  = num;
    m_cardColumn  = relCol;
    m_cardRow     = relRow;
  }
  
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
  
};

/// ===========================================================================
#endif // CALODET_CELLPARAM_H
/// ===========================================================================
