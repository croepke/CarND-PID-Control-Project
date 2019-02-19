#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using nlohmann::json;
using std::string;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID pid;
  // Variables for training; For training mode set training = true
  bool training = false;
  int counter = 0;
  int p_idx = 0;
  double positive_optimization = true;
  double best_error = 10000000;
  std::vector<double> p{ 0.374251, 0.000887792, 6.0 };
  std::vector<double> dp{ 0.00100432, 8.2172e-08, 0.00273893 };
  /**
   * TODO: Initialize the pid variable.
   */
  pid.Init(p[0], p[1], p[2]);

  h.onMessage([&pid, &counter, &p_idx, &best_error, &positive_optimization, &p, &dp, &training](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value;

          // Training
          if(training && counter == 0){
            // If first run -> increase p by dp
            if(positive_optimization) {
              p[p_idx] += dp[p_idx];
            }
            std::cout << "[0] kP: " << p[0] << "[1] kI: " << p[1] << "[2] kD: " << p[2] << std::endl;
            std::cout << "[0] dp: " << dp[0] << "[1] dI: " << dp[1] << "[2] dD: " << dp[2] << std::endl;
          }
          counter++;

          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
          pid.UpdateError(cte);
          double total_error = pid.TotalError();
          //std::cout << total_error << std::endl;
          steer_value = -total_error;
          // DEBUG
          //std::cout << "CTE: " << cte << " Steering Value: " << steer_value
          //          << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";

          // training
          if (training && counter == 10000) {
            double mean_error = pid.calculateCteMeanSquaredSum();
            pid.clearCteHistory();
            if (mean_error < best_error) {
              best_error = mean_error;
              dp[p_idx] *= 1.1;
              counter = 0;
              positive_optimization = true;
              // Move index one further
              p_idx += 1;
              if ( p_idx == 3 ) {
                p_idx = 0;
              }

            }
            else if (positive_optimization) {
              // try step into negative direction and reset counter
              p[p_idx] -= 2*dp[p_idx];
              counter = 0;
              positive_optimization = false;
            }
            // I tried positive and negative optimization, both sucked
            // reset to old situation and reduce step size
            else {
              p[p_idx] += dp[p_idx];
              dp[p_idx] *= 0.9;
              positive_optimization = true;
              counter = 0;
              // Move index one further
              p_idx += 1;
              if ( p_idx == 3 ) {
                p_idx = 0;
              }

            }
            std::cout << "Error " << mean_error << std::endl;
            msg = "42[\"reset\",{}]";
            pid.Update(p[0], p[1], p[2]);
          }
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code,
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }

  h.run();
}
