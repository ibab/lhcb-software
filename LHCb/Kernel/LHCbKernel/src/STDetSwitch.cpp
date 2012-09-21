#include "Kernel/STDetSwitch.h"

void STDetSwitch::flip(std::string aType, std::string& aString){

  std::string::size_type iChar = 0;

  if ( aType == "TT" ) {
    while ((iChar = aString.find("IT")) != std::string::npos) {
      aString.replace(iChar, 2, aType);
    }
    while ((iChar = aString.find("UT")) != std::string::npos) {
      aString.replace(iChar, 2, aType);
    }
  }

  if ( aType == "IT" ) {
    while ((iChar = aString.find("TT")) != std::string::npos) {
      aString.replace(iChar, 2, aType);
    }
    while ((iChar = aString.find("UT")) != std::string::npos) {
      aString.replace(iChar, 2, aType);
    }
  }

  if ( aType == "UT" ) {
    while ((iChar = aString.find("TT")) != std::string::npos) {
      aString.replace(iChar, 2, aType);
    }
    while ((iChar = aString.find("IT")) != std::string::npos) {
      aString.replace(iChar, 2, aType);
    }
  }
}
