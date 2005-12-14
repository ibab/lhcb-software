#include "Kernel/STDetSwitch.h"

void STDetSwitch::flip(std::string aType, std::string& aString){

  std::string::size_type iChar = 0;
  std::string oldType;
  aType == "TT" ? oldType = "IT" : oldType = "TT";

  while ((iChar = aString.find(oldType)) != std::string::npos) {
    aString.replace(iChar, 2, aType);
  }

}
