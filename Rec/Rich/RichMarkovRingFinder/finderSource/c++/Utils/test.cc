#include <iostream>
#include <string>
#include "Utils/DiscreteProbabilityTree.h"
//#include "Utils/OrderedProbabilitySet.h"
#include "stringToNumber/stringToNumber.h"
#include "Utils/RunningMeanAndVariance.h"

int main() {

  Lester::RunningMeanAndVariance rmv;

  rmv.takeNoteOf(3);
  rmv.takeNoteOf(11);
  rmv.takeNoteOf(64);
  std::cout << "Mean = " << rmv.mean() << " and var = " << rmv.variance() << ".  Right answer is 1099. There were " << rmv.N() << " samples." << std::endl;
  rmv.takeNoteOf(3);
  rmv.takeNoteOf(4);
  rmv.takeNoteOf(5);
  rmv.takeNoteOf(6);
  std::cout << "Mean = " << rmv.mean() << " and var = " << rmv.variance() << ".  Right answer is 499.2380952. There were " << rmv.N() << " samples." << std::endl;
  exit(0);

  /*

  try {

    /*{

      typedef OrderedProbabilitySet<int> OPS;
      OPS ops;
      ops.insertTWithProb(3, 1.3);
      ops.insertTWithProb(5, 1.5);
      ops.insertTWithProb(4, 1.4);
      ops.insertTWithProb(7, 1.7);
      ops.insertTWithProb(9, 1.9);
      ops.insertTWithProb(8, 1.8);
      ops.insertTWithProb(6, 1.6);
      std::cout << "Largest (should be 9) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=9) { abort(); };

      ops.removeT(ops.largestT());
      std::cout << "Largest (should be 8) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=8) { abort(); };
    
      ops.removeT(ops.largestT());
      std::cout << "Largest (should be 7) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=7) { abort(); };
    
      ops.insertTWithProb(27, 2.7);
      ops.insertTWithProb(29, 2.9);
      ops.insertTWithProb(28, 2.8);
      std::cout << "Largest (should be 29) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=29) { abort(); };

      ops.removeT(ops.largestT());
      std::cout << "Largest (should be 28) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=28) { abort(); };

      ops.removeT(ops.largestT());
      std::cout << "Largest (should be 27) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=27) { abort(); };

      ops.removeT(ops.largestT());
      std::cout << "Largest (should be 7) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=7) { abort(); };
    
      ops.removeT(ops.largestT());
      std::cout << "Largest (should be 6) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=6) { abort(); };
    
      ops.removeT(ops.largestT());
      std::cout << "Largest (should be 5) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=5) { abort(); };
    
      ops.removeT(ops.largestT());
      std::cout << "Largest (should be 4) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=4) { abort(); };
    
      ops.removeT(ops.largestT());
      std::cout << "Largest (should be 3) is " << ops.largestT() << std::endl;
      if (ops.largestT()!=3) { abort(); };
    

    };













  DiscreteProbabilityTree<std::string> tree("1",1);
  tree.insertTWithProb("2",2);
  tree.insertTWithProb("3",3);
  tree.insertTWithProb("4",4);
  tree.insertTWithProb("5",5);
  tree.insertTWithProb("6",6);
  tree.insertTWithProb("7",7);
  std::cout << "Before" <<std::endl;
  tree.printMeIndentedTo(std::cout);
  tree.removeT("5");
  std::cout << "After 5 remove" <<std::endl;
  tree.printMeIndentedTo(std::cout);
  tree.removeT("2");
  std::cout << "After 2 remove" <<std::endl;
  tree.printMeIndentedTo(std::cout);
  tree.removeT("1");
  std::cout << "After 1 remove" <<std::endl;
  tree.printMeIndentedTo(std::cout);
  tree.removeT("3");
  std::cout << "After 3 remove" <<std::endl;
  tree.printMeIndentedTo(std::cout);
  tree.removeT("7");
  std::cout << "After 7 remove" <<std::endl;
  tree.printMeIndentedTo(std::cout);
  tree.removeT("4");
  std::cout << "After 4 remove" <<std::endl;
  tree.printMeIndentedTo(std::cout);
  //tree.removeT("6");
  //std::cout << "After 6 remove" <<std::endl;
  //tree.printMeIndentedTo(std::cout);
  for (unsigned int i=10; i>0; --i) {
     tree.insertTWithProb(stringFrom<int>(i),static_cast<double>(i)+0.5);
     std::cout << "After insertion of " <<i<< std::endl;
     //tree.printMeIndentedTo(std::cout);
  
  };
  for (unsigned int i=1; i<=10; ++i) {
     tree.removeT(stringFrom<int>(i));
     std::cout << "After removal of " <<i<< std::endl;
     tree.printMeIndentedTo(std::cout); 
  };
  std::cout << "Finally" << std::endl;
  tree.printMeIndentedTo(std::cout); 
  for (unsigned int i=10; i>0; --i) {
     tree.insertTWithProb(stringFrom<int>(i),static_cast<double>(i)+0.5);
     std::cout << "After second insertion of " <<i<< std::endl;
     //tree.printMeIndentedTo(std::cout);
  
  };
  for (unsigned int i=1; i<6; ++i) {
     tree.removeT(stringFrom<int>(i));
     std::cout << "After second removal of " <<i<< std::endl;
     tree.printMeIndentedTo(std::cout); 
  };
  std::cout << "Second Finally" << std::endl;
  tree.printMeIndentedTo(std::cout); 

  DiscreteProbabilityTree<std::string> symmTree;
  symmTree.insertTWithProb("-2.1", 1);
  symmTree.insertTWithProb("-1.1", 2);
  symmTree.insertTWithProb("-0.1", 10);
  symmTree.insertTWithProb("+0.9", 2);
  symmTree.insertTWithProb("+1.9", 1);
  symmTree.fillTWithProb("-3.1",0.5);
  symmTree.fillTWithProb("+2.9",0.5);
  symmTree.fillTWithProb("+2.9",0.5);

  while(true) {
    std::cout << " SHOT " << tree.sample() << std::endl;
    const std::string & s = symmTree.sample();
    std::cout << " SYMM " << s << " " << symmTree.probabilityOf(s) << " " << symmTree.propToProbabilityOf(s) << std::endl;
  };
  return 0;
  } catch (std::exception & e) {

    std::cout << "std::exception ["<<e.what()<<"] was thrown and reached main." << std::endl;
    throw;

  } catch (...) {

    std::cout << "Unknownd excpetion was thrown and reached main." << std::endl;

    throw;
  };
  */
};
