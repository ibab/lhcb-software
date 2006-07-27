#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Utils/DereferencingOstreamIterator.h"
#include <iterator>

int main() {

  {
    typedef std::vector<const std::string *> PVec;
    PVec pVec;
    const std::string s1 = "Hello";
    const std::string s2 = "World";
    pVec.push_back(&s1);
    pVec.push_back(&s2);
    
    std::cout<<std::endl;
    std::cout << "USING std::ostream_iterator<const std::string *>(std::cout , " ") : " <<std::endl;
    copy(pVec.begin(), pVec.end(), std::ostream_iterator<const std::string *>(std::cout , " "));
    std::cout<<std::endl;
    
    std::cout<<std::endl;
    
    std::cout << "USING DereferencingOstreamIterator<const std::string *>(std::cout , \" \") : " <<std::endl;
    copy(pVec.begin(), pVec.end(), DereferencingOstreamIterator<const std::string *,-1>(std::cout , " "));
    std::cout<<std::endl;
    
    std::cout<<std::endl;
    
  }


  {
    typedef std::vector<const std::string * *> PPVec;
    PPVec ppVec;
    const std::string s1 = "Hello";
    const std::string s2 = "World";
    const std::string * p1 = &s1;
    const std::string * p2 = &s2;
    ppVec.push_back(&p1);
    ppVec.push_back(&p2);
    
    std::cout << "s1,s2 @ " << &s1 << " " << &s2 << std::endl;
    std::cout << "p1,p2 @ " << &p1 << " " << &p2 << std::endl;
    std::cout << std::endl;

    std::cout << "DEFAULTLY DEREFERENCED: " << std::endl;
    copy(ppVec.begin(), ppVec.end(), DereferencingOstreamIterator<const std::string **>(std::cout , " "));
    std::cout << std::endl;
    std::cout << std::endl;
    
    std::cout << "MAXIMALLY DEREFERENCED: " << std::endl;
    copy(ppVec.begin(), ppVec.end(), DereferencingOstreamIterator<const std::string **, -1>(std::cout , " "));
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "0 times DEREFERENCED: " << std::endl;
    copy(ppVec.begin(), ppVec.end(), DereferencingOstreamIterator<const std::string **, 0>(std::cout , " "));
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "1 times DEREFERENCED: " << std::endl;
    copy(ppVec.begin(), ppVec.end(), DereferencingOstreamIterator<const std::string **, 1>(std::cout , " "));
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "2 times DEREFERENCED: " << std::endl;
    copy(ppVec.begin(), ppVec.end(), DereferencingOstreamIterator<const std::string **, 2>(std::cout , " "));
    std::cout << std::endl;
    std::cout << std::endl;
    };


  return 0;
};
