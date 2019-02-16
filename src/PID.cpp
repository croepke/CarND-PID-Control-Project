#include "PID.h"
#include <vector>

/**
 * TODO: Complete the PID class. You may add any additional desired functions.
 */

using std::vector;

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp_, double Ki_, double Kd_) {
  /**
   * TODO: Initialize PID coefficients (and errors, if needed)
   */

   /**
    * PID Coefficients
    */
   Kp = Kp_;
   Ki = Ki_;
   Kd = Kd_;
}

void PID::UpdateError(double cte) {
  /**
   * TODO: Update PID errors based on cte.
   */

   double cte_diff;
   double cte_int;

   if (isCteHistoryEmpty()) {
     cte_diff = 0.0;
   }
   else {
     cte_diff = cte - cte_hist.back();
   }
   UpdateCteHistory(cte);

   cte_int = calculateCteSum();

   // Perform actual updates
   p_error = cte * Kp;
   i_error = cte_int * Ki;
   d_error = cte_diff * Kd;
}

double PID::TotalError() {
  /**
   * TODO: Calculate and return the total error
   */
  return (p_error + i_error + d_error);  // TODO: Add your total error calc here!
}

void PID::UpdateCteHistory(double cte) {
  cte_hist.push_back(cte);
}

vector<double> PID::getCteHistory() {
  return cte_hist;
}

bool PID::isCteHistoryEmpty() {
  return cte_hist.size() == 0;
}

double PID::calculateCteSum() {
  double sum = 0.0;
  for (int i=0; i<cte_hist.size(); ++i) {
    sum += cte_hist[i];
  }
  return sum;
}
