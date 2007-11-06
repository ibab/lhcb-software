#ifndef UPI_UPICC_H
#define UPI_UPICC_H

#include "UPI/upidef.h"

#ifdef __cplusplus
#include <vector>

extern "C" {
  std::vector<int> upicc_get_input_with_index();
  std::vector<int> upicc_get_input();
}

#endif /* ___cplusplus */
#endif /* UPI_UPICC_H  */
