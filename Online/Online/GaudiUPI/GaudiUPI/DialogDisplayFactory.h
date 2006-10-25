#ifndef GAUDIUPI_DIALOGDISPLAY_FACTORY_H__
#define GAUDIUPI_DIALOGDISPLAY_FACTORY_H__

#include "GaudiUPI/AbstractDisplay.h"

class DialogDisplayFactory : public AbstractDisplayFactory {
protected:
public:
  DialogDisplayFactory();
   virtual ~DialogDisplayFactory();
  AbstractMainDisplay* createMainDisplay(const AbstractDisplay &);
  AbstractSubDisplay* createSubDisplay(const AbstractDisplay &);
};
#endif  // GAUDIUPI_DIALOGDISPLAY_FACTORY_H__
