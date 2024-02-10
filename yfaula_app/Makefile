#Makefile de l'application FlashDetection
#créer un dossier build
#et l'executable
#Yannick Faula 

.PHONY:clean

CC= g++
#Possible flags with the compiler : 
#-Wdouble-promotion #Wfloat-equal
CFLAGS= -O2 -s -Wall -std=c++11
SRCS_FLASH= $(wildcard *.cpp)
OBJS_DIR=obj

ECHO= @echo
# A modifier éventuellement
OPENCV_PREFIX = ../../opencv/build/install_folder
OPENCV4_INC= $(OPENCV_PREFIX)/include/opencv4
BUILD_DIR = build
LIBS = -L$(OPENCV_PREFIX)/lib -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_ml -lopencv_stitching -lopencv_objdetect -lopencv_photo -lopencv_gapi -lopencv_video -lopencv_calib3d -lopencv_features2d -lopencv_dnn -lopencv_imgproc -lopencv_flann -lopencv_core



all:mkdirectories flash

mkdirectories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJS_DIR)

flash:$(SRCS_FLASH)
	$(ECHO) "Compiling and Linking of <$@>"
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/flash $(SRCS_FLASH) $(LIBS) -I$(OPENCV4_INC) -ldl -lm -lpthread -lrt

clean:
	@rm -f obj/*.o $(EXE) *.o
