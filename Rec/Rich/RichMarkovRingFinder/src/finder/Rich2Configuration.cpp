
#include "Rich2Configuration.h"

namespace Lester {

  Rich2Configuration globalRich2LConfiguration(Rich2Configuration:: leftPane); //definition (allocates storage)
  Rich2Configuration globalRich2RConfiguration(Rich2Configuration::rightPane); //definition (allocates storage)

  /// Initialise the theta prior distribution stuff
  Rich2Configuration::Rich2Configuration( const Pane pane ) 
    :  RichConfiguration( thetaDirDeJour, thetaFileR2DeJour ),
       m_pane(pane) {}
};
