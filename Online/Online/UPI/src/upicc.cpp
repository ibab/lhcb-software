#include "UPI/upicc.h"

UPI::upicc_result upicc_get_message_window() {
  UPI::upicc_result result;
  upic_get_message_window(&result.r0,&result.r1,&result.r2,&result.r3);
  return result;
}

UPI::upicc_result upicc_get_input_with_index() {
  UPI::upicc_result result;
  upic_get_input_with_index(&result.r0,&result.r1,&result.r2,&result.r3);
  return result;
}

UPI::upicc_result upicc_get_input() {
  UPI::upicc_result result;
  upic_get_input(&result.r0,&result.r1,&result.r2);
  return result;
}
