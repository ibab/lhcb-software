
#include "RichDet/RichParameters.h"
#include "RichDet/Configuration.h"

Configuration::Configuration()
{
  // we'll get the nrOfRiches from XML, but for now...
  for (Rich::RichID id = Rich::Rich1; id<=Rich::Rich2; id++) {
    rich_.push_back( new Rich(id) );
  }  
}


Configuration::~Configuration()
{
  for(vector<Rich*>::iterator rich = rich_.begin();
      rich != rich_.end();
      ++rich) {
    delete *rich;
  }
}

void Configuration::update (const RichParameters &parameter)
{
  for(vector<Rich*>::iterator rich = rich_.begin();
      rich != rich_.end();
      ++rich) {
    assert( *rich != NULL );
    (*rich)->update(parameter);
  }
}


