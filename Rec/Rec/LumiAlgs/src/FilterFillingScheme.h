// $Id: $
#ifndef FILTERFILLINGSCHEME_H 
#define FILTERFILLINGSCHEME_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class FilterFillingScheme FilterFillingScheme.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2011-08-09
 */
class FilterFillingScheme : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterFillingScheme( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterFillingScheme( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  virtual StatusCode registerDB();    ///< register DB conditions
  virtual bool processDB();           ///< DB checking code

private:
  StatusCode i_cacheFillingData();    ///< Function extracting data from Condition
  StatusCode i_cacheMagnetData();     ///< Function extracting data from Condition

protected:

  std::string m_beam;                 ///< Beam looked at
  std::string m_MagnetState;          ///< Magnet state looked at (if empty: all)
  int m_BXOffset;                     ///< bcid offset imspected
  long m_bxid;                        ///< BX id from ODIN

  // database conditions and calibration factors
  SmartIF<IDetDataSvc> m_dds;         ///< DetectorDataSvc

  Condition *m_condMagnet;            ///< Condition for magnet
  std::string m_parMagnetState;       ///< magnet state (UP/DOWN)

  Condition *m_condFilling;           ///< Condition for LHC filling scheme
  std::string m_B1FillingScheme;      ///< filled bunches 00101010....
  std::string m_B2FillingScheme;      ///< filled bunches 00101010....

private:

};
#endif // FILTERFILLINGSCHEME_H
