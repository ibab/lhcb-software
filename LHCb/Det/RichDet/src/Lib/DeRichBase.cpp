
// Gaudi
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "RichDet/DeRichBase.h"
#include "RichDet/DeRichSystem.h"

// Access DeRichSystem on demand
DeRichSystem * DeRichBase::deRichSys() const
{
  if ( !m_deRichS )
  {
    // find the RichSystem
    SmartDataPtr<DeRichSystem> deRichS( dataSvc(), DeRichLocations::RichSystem );
    if ( !deRichS )
    {
      throw GaudiException( "Could not load DeRichSystem at " + 
                            DeRichLocations::RichSystem,
                            "DeRichBase::deRichSys()", StatusCode::FAILURE );
    }
    m_deRichS = deRichS;
  }
  return m_deRichS;
}
