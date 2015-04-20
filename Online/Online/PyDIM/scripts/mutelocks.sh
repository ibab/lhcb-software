#!/bin/bash
sed  -i "s|gstate = |//gstate = |" src/*.cpp 
sed  -i "s|PyGILState_Release(gstate)|// PyGILState_Release(gstate)|" src/*.cpp
 
