// $Id: RichPixelStrippingAlg.h,v 1.1 2004-06-25 11:53:07 buckley Exp $
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

};
#endif // RICHPIXELSTRIPPINGALG_H
