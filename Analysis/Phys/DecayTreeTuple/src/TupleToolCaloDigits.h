// $Id: TupleToolCaloDigits.h,v 0.1 2015-08-05 bursche $
#ifndef TUPLETOOLCALODIGITS_H
#define TUPLETOOLCALODIGITS_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IEventTupleTool.h"            // Interface
#include  "CaloDet/DeCalorimeter.h"
#include "TupleToolCaloDigits.h"
class ITupleTool;

/** @class TupleToolCaloDigits TupleToolCaloDigits.h 
 *
 * \brief Add Calorimter digits to DecayTreeTuple
 *
 * Tuple columns:
 * - Digit energy
 * - Cell ID
 * - Cell area, column and row 
 * \sa DecayTreeTuple
 *  @author Albert Bursche
 *  @date   2015-08-05
 */
class TupleToolCaloDigits : public TupleToolBase, virtual public IEventTupleTool {
public:
  /// Standard constructor
  TupleToolCaloDigits( const std::string& type,
                const std::string& name,
                const IInterface* parent);

  virtual ~TupleToolCaloDigits( ){}; ///< Destructor

  StatusCode fill( Tuples::Tuple& );
  StatusCode initialize( );

private :
  std::string m_DigitLocation = LHCb::CaloDigitLocation::Spd;
  std::string m_CaloName = "Spd";
  std::string m_CaloLocation = DeCalorimeterLocation::Spd ;
   
  unsigned int * m_index;
  unsigned char * m_calo;
  unsigned char * m_area;
  unsigned char * m_row;
  unsigned char * m_column;
  bool m_auto_configure = false;
  
  float * m_xs; 
  float * m_ys; 
  float * m_zs; 
  float * m_es;
  
  unsigned long m_maxSize;
};
#endif // 
