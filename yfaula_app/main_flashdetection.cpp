/*
main of flash detection project
*/

#include <iostream>
#include <string>

#include "opencv.hpp"
#include "flash_workflow.h"


using namespace std;
using namespace cv;

void FlashDetectionApp(int argc, char** argv)
{
  Params params;
  string image_name;
  bool with_details;

  const string KEYS = "{help h usage ? || print this message}\
                        {default D|| get the default parameters to a file in the current directory}\
                        {@filename | <none> | image to evaluate}\
                        {p parameters| | parameters file to use. no use the defaults parameters}\
                        { output_dir | . | output directory after processing the image}\
                        { with_details || save more details about the detection}";


cv::CommandLineParser parser(argc, argv, KEYS);


  if (parser.get<bool>("help"))
  {
    parser.printMessage();
    return;
  }

  if (parser.get<bool>("default"))
  {
    Params params = flash::getDefaultParams();
    string txt_params = getStringParams(params);

    ofstream file_params;
    file_params.open("flash_default_parameters.txt");
    file_params << txt_params;
    file_params.close();

    cout << "Default parameters for FLASH detection has been saved!!\n\n";

    return;
  }

  image_name = parser.get<string>("@filename");

  if (image_name.empty())
  {

    parser.printMessage();
    cerr << "Cannot access to image path!!\n\n";
    return;
  }

  with_details = parser.get<bool>("with_details");
  string output_directory = parser.get<string>("output_dir");

  
  string parameters_filename = parser.get<string>("parameters");
  if (parameters_filename.empty())
  {
    cout << "Default parameters are used\n";
    params = flash::getDefaultParams();
  }
  else
  {
    ifstream paramaters_fstream;
    paramaters_fstream.open(parameters_filename);
    string parameters_string;

    if (paramaters_fstream)
    {
      while (!paramaters_fstream.eof())
      {
        string line;
        getline(paramaters_fstream, line);
        parameters_string += line;
        parameters_string += "\n";
      }
      cout << "Parameters used for flash detection\n\n";
      cout << parameters_string << endl;
      params = setParams(parameters_string);
    }
    else
    {
      cout << "Default parameters are used. Cannot access the parameters file\n";
      params = flash::getDefaultParams();
    }
  }
  
  flash_detection_workflow(params, output_directory, image_name);
}


int main(int argc, char** argv)
{
  FlashDetectionApp(argc, argv);

  return 0;
}
