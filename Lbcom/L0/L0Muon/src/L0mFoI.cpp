#define L0MFOI_CPP

#include <iostream>
#include "L0mFoI.h"
#include "MuonPadLayout.h"

L0mFoI::L0mFoI(std::vector<int> xsize, 
               std::vector<int> ysize) {
    
        m_xsize = xsize;
        m_ysize = ysize;
}

L0mFoI::L0mFoI() {
    
    m_xsize[0] = 4;
    m_xsize[1] = 4;	
    m_xsize[2] = 0;
    m_xsize[3] = 2;	
    m_xsize[4] = 2;	

    m_ysize[0] = 0;
    m_ysize[1] = 0;	
    m_ysize[2] = 0;
    m_ysize[3] = 0;	
    m_ysize[4] = 0;	

}

L0mFoI::~L0mFoI() {
}

int L0mFoI::xSize(int st) {    
    return m_xsize[st-1];
}

int L0mFoI::ySize(int st) {    
    return m_ysize[st-1];
}

void L0mFoI::dump() {

    cout << "L0Muon: FoI in x: " << m_xsize[0] << m_xsize[1] << m_xsize[2]  
                                 << m_xsize[3] << m_xsize[4] << endl;
    cout << "L0Muon: FoI in y: " << m_ysize[0] << m_ysize[1] << m_ysize[2]  
                                 << m_ysize[3] << m_ysize[4] << endl;				  
}
