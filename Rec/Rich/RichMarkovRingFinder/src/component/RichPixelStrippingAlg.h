// $Id: RichPixelStrippingAlg.h,v 1.2 2004-10-08 19:00:40 abuckley Exp $
#ifndef RICHPIXELSTRIPPINGALG_H
#define RICHPIXELSTRIPPINGALG_H 1

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// base
#include "RichRecBase/RichRecAlgBase.h"

/** @class RichPixelStrippingAlg RichPixelStrippingAlg.h
 *
 *  Empty Algorithm.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-04-01
 */
class RichPixelStrippingAlg : public RichRecAlgBase {

public:

  /// Standard constructor
  RichPixelStrippingAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichPixelStrippingAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  bool m_StripUntrackedMCMCPixs;

  bool m_savePreStrippedPixels;


};
#endif // RICHPIXELSTRIPPINGALG_H
