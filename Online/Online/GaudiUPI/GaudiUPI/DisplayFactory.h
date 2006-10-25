#ifndef CPP_DISPLAYFACTORY__H
#define CPP_DISPLAYFACTORY__H 1

#include <string>
class AbstractDisplayFactory;

class DisplayFactory   {
 public:
   DisplayFactory (const std::string& type);
   virtual ~DisplayFactory() {}
   static AbstractDisplayFactory* factory();
};
#endif  // CPP_DISPLAYFACTORY__H
