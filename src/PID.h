#ifndef PID_H
#define PID_H

#include <vector>

class PID {
 public:
  /**
   * Constructor
   */
  PID();

  /**
   * Destructor.
   */
  virtual ~PID();

  /**
   * Initialize PID.
   * @param (Kp_, Ki_, Kd_) The initial PID coefficients
   */
  void Init(double Kp_, double Ki_, double Kd_);

  void Update(double Kp_, double Ki_, double Kd_);

  /**
   * Update the PID error variables given cross track error.
   * @param cte The current cross track error
   */
  void UpdateError(double cte);

  /**
   * Calculate the total PID error.
   * @output The total PID error
   */
  double TotalError();

  /**
   * Update the history of CTE values
   * @param cte CTE value to be added
   */
  void UpdateCteHistory(double cte);
  /**
   * Get CTE history
   */
  std::vector<double> getCteHistory();

  /**
   * Calculate the total sum of CTE values
   */
  double calculateCteSum();

  /**
   * Calculcate mean squared error for given CTE history
   */
  double calculateCteMeanSquaredSum();

  /**
   * Check if CTE history is empty
   */
  bool isCteHistoryEmpty();

  /**
   * Reset CTE history
   */
  void clearCteHistory();

 private:
  /**
   * PID Errors
   */
  double p_error;
  double i_error;
  double d_error;

  /**
   * PID Coefficients
   */
  double Kp;
  double Ki;
  double Kd;

  /**
   * History of CTE values
   */
  std::vector<double> cte_hist;
};

#endif  // PID_H
