#ifndef UPI_UPICC_H
#define UPI_UPICC_H

#include "UPI/upidef.h"

#ifdef __cplusplus
#include <vector>

namespace UPI {  struct upicc_result {    int r0,r1,r2,r3,r4,t5;  };  }

extern "C" {

  UPI::upicc_result upicc_get_input_with_index();
  UPI::upicc_result upicc_get_input();
  UPI::upicc_result upicc_get_message_window();
}

#endif /* ___cplusplus */
#endif /* UPI_UPICC_H  */
