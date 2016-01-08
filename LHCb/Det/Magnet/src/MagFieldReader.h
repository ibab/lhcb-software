// $Id: MagFieldReader.h,v 1.8 2008-07-03 10:35:16 ahicheur Exp $
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
  virtual StatusCode finalize   ();    ///< Algorithm finalization
  void TestBdl();

protected:

private:

  // Pointer to the magnetic field service
  IMagneticFieldSvc* m_pIMF;
  //  IMagneticFieldSvc* m_pIAF;




  // range and steps in z to get the magnetic field
  double m_zMin, m_zMax, m_step;

  // range in x and y
  double m_xMin, m_xMax, m_yMin, m_yMax;
  std::string m_FieldServiceName; ///< Indicate the name of the service to be tested (default is MagneticFieldSvc)
  bool m_testbdl; ///< Test field integral for random "track" slopes
  int m_nInt; ///< number of field integrals to generate



};
#endif // MAGFIELDREADER_H
