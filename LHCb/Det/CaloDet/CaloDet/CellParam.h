/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
#ifndef CALODET_CELLPARAM_H 
#define CALODET_CELLPARAM_H 1
/// ===========================================================================

// Include files

class DeCalorimeter;

typedef std::vector<CaloCellID> CaloNeighbors ;

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
  CellParam( const CaloCellID& id  = CaloCellID() ) ;
  
  /// destructor 
  ~CellParam();
  
  bool                 valid         () const { return size() > 0      ; }   
  CaloCellID           cellID        () const { return m_cellID        ; }
  double               x             () const { return m_Center.x()    ; }
  double               y             () const { return m_Center.y()    ; }
  double               z             () const { return m_Center.z()    ; }
  const HepPoint3D&    center        () const { return m_Center        ; }
  double               size          () const { return m_Size          ; }
  double               sine          () const { return m_Sine          ; }
  double               gain          () const { return m_Gain          ; }
  double               time          () const { return m_Time          ; }
  int                  cardNumber    () const { return m_CardNumber    ; }
  int                  cardRow       () const { return m_CardRow       ; }
  int                  cardColumn    () const { return m_CardColumn    ; }
  
  const CaloNeighbors& neighbors     () const { return m_Neighbors     ; }
  const CaloNeighbors& zsupNeighbors () const { return m_ZsupNeighbors ; }
  
  // ** To initialize the cell: Geometry, neighbours, gain
  
  void  setCenterSize( const HepPoint3D& point, double S) {
    m_Center = point; 
    m_Size   = S; 
    m_Sine   = sqrt( (point.x()*point.x() + point.y()*point.y()) / 
                     point.mag2() ); 
    m_Time   = point.mag() /c_light *ns;
  }
  
  void addZsupNeighbor( const CaloCellID& ID) { 
    m_ZsupNeighbors.push_back(ID);
  }
  void addNeighbor    ( const CaloCellID& ID) { m_Neighbors.push_back(ID); }
  void setGain        ( const double gain   ) { m_Gain = gain            ; }
  void setFeCard      ( const int num, const int relCol, const int relRow ) {
    m_CardNumber  = num;
    m_CardColumn  = relCol;
    m_CardRow     = relRow;
  }
  
  bool operator==( const CellParam& c2 ) const { 
    return center() == c2.center() && size() == c2.size(); }
  
private:
  
  CaloCellID    m_cellID         ; ///< ID of the cell
  double        m_Size           ; ///< Cell size
  HepPoint3D    m_Center         ; ///< Cell centre
  double        m_Sine           ; ///< To transform E to Et
  double        m_Gain           ; ///< MeV per ADC count
  double        m_Time           ; ///< Nominal time of flight from Vertex (ns)
  int           m_CardNumber     ; ///< Front-end card number 
  int           m_CardRow        ; ///< card row and column
  int           m_CardColumn     ;
  CaloNeighbors m_Neighbors      ; ///< List of neighbors
  CaloNeighbors m_ZsupNeighbors  ; ///< List of neighbors in same area
  
};

/// ===========================================================================
#endif // CALODET_CELLPARAM_H
/// ===========================================================================
