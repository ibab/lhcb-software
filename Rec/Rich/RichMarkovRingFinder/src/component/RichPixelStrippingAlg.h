// $Id: RichPixelStrippingAlg.h,v 1.3 2004-11-10 18:36:06 abuckley Exp $
#ifndef RICHPIXELSTRIPPINGALG_H
#define RICHPIXELSTRIPPINGALG_H 1

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// base
#include "RichRecBase/RichRecAlgBase.h"

/// Algorithm for stripping pixels which don't belong to rec-tracked rings
/// @author Chris Jones  Christopher.Rob.Jones@cern.ch
/// @date   2004-04-01
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
