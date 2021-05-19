///////////////////////////////////////////////////////////////////
// Prepared for BCIT ELEX4618 by Craig Hennessey
///////////////////////////////////////////////////////////////////
#pragma once

#define WIN4618
//#define PI4618

#include <iostream>
#include <string>
#include <mutex>
#include <vector>

class Server
{
private:
  bool _server_exit;
  cv::Mat _txim;
  
  std::mutex _image_mutex;
  std::mutex _rx_mutex;
  std::mutex _tx_mutex;
  std::vector<std::string> _cmd_list;
  std::vector<std::string> _send_list;

public:
  Server();
  ~Server();

  // Start server listening (probably best to do in a separate thread)
  void start(int port);
  void stop();

  // Set the image to transmit
  void set_txim (cv::Mat &im);

  // Get a vector of the commands the server has read
  void get_cmd (std::vector<std::string> &cmds);

  // Send a response
  void send_string (std::string send_str);
};