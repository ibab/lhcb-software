#ifndef Adder_Tim_H
#define Adder_Tim_H 1

#include <dis.hxx>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Adder.h"
#include <vector>

using namespace std;

class Tim: public DimTimer {
public: 
   //constructor
   Tim(int time, std::vector<std::vector<DimInfoHistos*> > info);
   void timerHandler();

public:
 std::vector<std::vector<DimInfoHistos*> > info;
 int time;
};   


#endif    // Adder_Tim_H
