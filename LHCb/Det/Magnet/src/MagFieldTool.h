// $Id: MagFieldTool.h,v 1.2 2008-07-26 23:03:01 cattanem Exp $
#ifndef MAGFIELDTOOL_H 
#define MAGFIELDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IMagFieldTool.h"            // Interface


/** @class MagFieldTool MagFieldTool.h
 *  Implementation of IMagFieldTool for real field map (one file per quadrant)
 *
 *  @author Marco Cattaneo
 *  @date   2008-07-26
 */
class MagFieldTool : public GaudiTool, virtual public IMagFieldTool {
public: 
  /// Standard constructor
  MagFieldTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~MagFieldTool( ); ///< Destructor

  /// Return the field vector fvec at the point xyz
  virtual void fieldVector( const Gaudi::XYZPoint&  xyz,
                                  Gaudi::XYZVector& fvec ) const;

  /// Update the cached field from the files theFiles scaled by scaleFactor
  virtual StatusCode updateMap( const std::vector<std::string>& theFiles,
                                const double& scaleFactor );

protected:

private:
  /// Read the field map from theFiles and apply the scaleFactor
  StatusCode readFiles( const std::vector<std::string>& theFiles, 
                        const double& scaleFactor );

  /// Rescale the cached map be rescaleFactor
  void rescale ( const double& rescaleFactor );
  

  std::vector<std::string> m_mapFileNames; ///< Cached field map file names
  double                   m_scaleFactor;  ///< Cached field scaling factor

  std::vector<double> m_Q_quadr[4];        ///< Field vectors for 4 quadrants map
  double m_Dxyz[3];              ///< Steps in x, y and z
  int    m_Nxyz[3];              ///< Number of steps in x, y and z
  double m_max_FL[3];            ///< Maximum limits of the field grid
  double m_min_FL[3];            ///< Minimum limits of the field grid
  double m_zOffSet;              ///< The z offset

};
#endif // MAGFIELDTOOL_H
