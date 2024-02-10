#include "params_helper.h"
#include <algorithm>


string getStringParams(Params params) {
  string output;
  Params::iterator it_end = params.end();
  Params::iterator it;

  for (it = params.begin(); it != it_end; ++it)
  {
    string number;
    number = to_string(it->second);
    output += it->first;
    output += "  =  ";
    output += number;
    output += "\n";
  }
  return output;
}

Params setParams(string params_str) {
  Params params_output;

  //delete all space characters
  params_str.erase(remove_if(params_str.begin(), params_str.end(), 
    [](const char c) { return (c == ' '); }),
    params_str.end());

  auto global_pos = params_str.find('\n');
  string line(params_str, 0, global_pos);

  while (!line.empty() || global_pos != string::npos)
  {
    auto pos = line.find('=', 0);
    if (pos != string::npos)
    {
      string p = line.substr(0, pos);
      string value_string = line.substr(pos + 1);
      float value = stof(value_string);

      params_output[p] = value;
    }
    auto global_pos2 = params_str.find('\n', global_pos + 1);
    line = params_str.substr(global_pos + 1, global_pos2 - (global_pos + 1));
    global_pos = global_pos2;
  }

  return params_output;
}