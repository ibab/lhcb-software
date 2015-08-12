// $Id: RichHPDImageSummary.h,v 1.2 2009/11/25 18:35:55 tblake Exp $
#ifndef RICHHPDIMAGESUMMARY_H
#define RICHHPDIMAGESUMMARY_H 1

// Base class
#include "RichHPDImageAnalysis/RichHPDImageSummaryBase.h"

namespace Rich
{

  /** @namespace HPDImage
   *
   *  General namespace for RICH HPD image analysis code
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   02/03/0211
   */
  namespace HPDImage
  {

    /** @class Summary RichHPDImageSummary.h
     *
     *  Monitors the HPD image movements within an HPD
     *
     *  @author Thomas BLAKE
     *  @date   2010-03-16
     */
    class Summary : public SummaryBase
    {

    public:

      /// Standard constructor
      Summary( const std::string& name, ISvcLocator* pSvcLocator );

      /// Destructor
      virtual ~Summary( ); 

    };

  } // namespae Mon

} // namespace Rich

//=============================================================================

#endif // RICHHPDIMAGEMOVEMENT_H
