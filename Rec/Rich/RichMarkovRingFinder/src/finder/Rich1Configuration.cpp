
#include "Rich1Configuration.h"

namespace Lester {

  Rich1Configuration globalRich1LConfiguration(Rich1Configuration::lowerPane); //definition (allocates storage)
  Rich1Configuration globalRich1UConfiguration(Rich1Configuration::upperPane); //definition (allocates storage)

  /// Initialise the theta prior distribution stuff
  Rich1Configuration::Rich1Configuration( const Pane pane ) 
    :  RichConfiguration( thetaDirDeJour, thetaFileR1DeJour ),
       m_pane(pane) {}

};
