#ifndef FLASHDETECTION_FLASH_POINTS_H_
#define FLASHDETECTION_FLASH_POINTS_H_

#include "flash_engine.h"
#include "params_helper.h"


void flash_detection_workflow(Params params, string output_directory, string source_name);

namespace flash {
  Params getDefaultParams();
}
	


#endif // !FLASHDETECTION_FLASH_POINTS_H_