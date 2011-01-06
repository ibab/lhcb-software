#ifndef _STDetSwitch_H
#define _STDetSwitch_H

#include <string>

/** 
* Switch the type around (IT - OT and vice versa)
* @param type to switch to
* @param string to switch
*/
namespace STDetSwitch{

void flip(std::string aType, std::string& aString);

}
#endif
