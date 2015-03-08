#include <unistd.h>
#include <string>
#include <stream>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdio>
#include <stdlib>
#include <ctime>
#include <TSystem>

void setup(){
  cout << "Private library '" <<   gSystem->Getenv("ROOLIB")  << "' loaded" << endl;
  cout << "Setup completed" << endl;
};
