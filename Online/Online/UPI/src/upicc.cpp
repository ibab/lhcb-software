#include "UPI/upicc.h"

std::vector<int> upicc_get_input_with_index() {
  std::vector<int> result(4,0);
  upic_get_input_with_index(&result[0],&result[1],&result[2],&result[3]);
  return result;  
}

std::vector<int> upicc_get_input() {
  std::vector<int> result(3,0);
  upic_get_input(&result[0],&result[1],&result[2]);
  return result;  
}
