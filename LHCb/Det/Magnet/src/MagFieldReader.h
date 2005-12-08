// $Id: MagFieldReader.h,v 1.2 2005-12-08 15:16:44 cattanem Exp $
#ifndef MAGFIELDREADER_H 
#define MAGFIELDREADER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// Forward references
class IMagneticFieldSvc;

/** @class MagFieldReader MagFieldReader.h
 *  @param An Algorithm to read and plot magnetic filed maps
 *  @param for x and y kept constant and varying z. The x, y
 *  @param positions and the z range can be set in job options.
 *
 *  @author Edgar De Oliveira
 *  @date   08/05/2002
 */

class MagFieldReader : public GaudiTupleAlg {
public:
  /// Standard constructor
  MagFieldReader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MagFieldReader( ){ }; ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  
  // Pointer to the magnetic field service
  IMagneticFieldSvc* m_pIMF;

  // range and steps in z to get the magnetic field 
  double m_zMin, m_zMax, m_step;

  // range in x and y
  double m_xMin, m_xMax, m_yMin, m_yMax;

};
#endif // MAGFIELDREADER_H
