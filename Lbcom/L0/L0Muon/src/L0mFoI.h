// $Header:

#ifndef L0MUON_L0MFOI_H
#define L0MUON_L0MFOI_H 1

#include <vector>

/** @class L0mFoI L0mFoI.h L0mFoI.h 

   Defines a set of Fields of interest to be applied while
   the track finding
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

class L0mFoI {

public:

    /// Constructor taking vectors of fields of interest
    L0mFoI(std::vector<int> xsize, std::vector<int> ysize);
    L0mFoI();
    ~L0mFoI();
    /// accessor to field of interest in X in a station
    int xSize(int st);
    /// accessor to field of interest in Y in a station
    int ySize(int st);
    /// dump the contents to cout
    void dump();
    
private:
    std::vector<int> m_xsize; 
    std::vector<int> m_ysize;   
};

#endif  // L0MUON_L0MFOI_H
