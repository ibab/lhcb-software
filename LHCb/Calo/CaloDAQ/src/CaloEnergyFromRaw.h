// $Id: CaloEnergyFromRaw.h,v 1.3 2005-09-06 14:50:01 ocallot Exp $
#ifndef CALOENERGYFROMRAW_H 
#define CALOENERGYFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloEnergyFromRaw.h"            // Interface
#include "CaloDet/DeCalorimeter.h"
#include "Event/RawEvent.h"
#include "CaloDAQ/CaloReadoutTool.h"

/** @class CaloEnergyFromRaw CaloEnergyFromRaw.h
 *  Decode the calorimeter energies, accodrding to version
 *
 *  @author Olivier Callot
 *  @date   2005-01-10
 */
class CaloEnergyFromRaw : public GaudiTool, virtual public ICaloEnergyFromRaw {
public: 
  /// Standard constructor
  CaloEnergyFromRaw( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloEnergyFromRaw( ); ///< Destructor

  virtual StatusCode initialize();

  virtual void setScaleAndShift( double scale, double shift );
  
  virtual void prepare( int type );
  
  virtual StatusCode nextCell( CaloCellID& id, double& energy );

protected:

private:
  const std::vector<RawBank>* m_banks;
  std::vector<RawBank>::const_iterator m_itB;
  int m_dataSize;
  int m_lastData;
  int m_lastID;
  int m_sourceID;
  raw_int* m_data;
  int m_version;
  double m_scale;
  double m_pedShift;
  int    m_offset;
  int    m_bitNum;
  int    m_cardNum;
  int    m_chanNum;
  int    m_lenAdc;
  int    m_lenTrig;
  int    m_pattern;
  DeCalorimeter* m_calo;
  CaloReadoutTool* m_roTool;
  std::vector<CaloCellID> m_chanID;
  std::vector<std::pair<CaloCellID, double> > m_results;
  std::vector<std::pair<CaloCellID, double> >::const_iterator m_resultsIt;
};
#endif // CALOENERGYFROMRAW_H
