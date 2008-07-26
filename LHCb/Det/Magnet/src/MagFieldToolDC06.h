// $Id: MagFieldToolDC06.h,v 1.2 2008-07-26 23:03:01 cattanem Exp $
#ifndef MAGFIELDTOOLDC06_H 
#define MAGFIELDTOOLDC06_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IMagFieldTool.h"            // Interface


/** @class MagFieldToolDC06 MagFieldToolDC06.h
 *  Implementation of IMagFieldTool for DC06 field map
 *
 *  @author Marco Cattaneo
 *  @date   2008-07-26
 */
class MagFieldToolDC06 : public GaudiTool, virtual public IMagFieldTool {
public: 
  /// Standard constructor
  MagFieldToolDC06( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~MagFieldToolDC06( ); ///< Destructor

  /// Return the field vector fvec at the point xyz
  virtual void fieldVector( const Gaudi::XYZPoint&  xyz,
                                  Gaudi::XYZVector& fvec ) const;

  /// Update the cached field from the files theFiles scaled by scaleFactor
  virtual StatusCode updateMap( const std::vector<std::string>& theFiles,
                                const double& scaleFactor );

protected:

private:
  /// Read the field map from theFile and apply the scaleFactor
  StatusCode readFile( const std::string& theFile, const double& scaleFactor );

  /// Rescale the cached map be rescaleFactor
  void rescale ( const double& rescaleFactor );
  

  std::string m_mapFileName;  ///< Cached field map file name
  double      m_scaleFactor;  ///< Cached field scaling factor

  std::vector<double> m_Q;       ///< Field vector
  double m_Dxyz[3];              ///< Steps in x, y and z
  int    m_Nxyz[3];              ///< Number of steps in x, y and z
  double m_max_FL[3];            ///< Maximum limits of the field grid
  double m_min_FL[3];            ///< Minimum limits of the field grid
  double m_zOffSet;              ///< The z offset


};
#endif // MAGFIELDTOOLDC06_H
