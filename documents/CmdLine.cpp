// CmdLine.cpp : main project file.
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <chrono>
#include <windows.h>

#define CONF_TOKEN std::string("-conf")
#define BROWSE_DURATION (5*1000)   //5 seconds in milliseconds

void moveToEnd(int* argc, char * argv[], int positionToMoveToEnd);
void devSwap(std::string& line, std::string&device);

int main(int argc, char *argv[])
{
	std::string name;
	std::string conf;
	for (int i = 1; i < argc; i++){
		if (argv[i][0] != '-'){
			name = std::string(argv[i]);
			//move this element to the end of the argv char * array and backfill the void by moving up other elements
			moveToEnd(&argc, argv, i);
			i--;
		}
		else if (strncmp(argv[i], CONF_TOKEN.c_str(), CONF_TOKEN.length()) == 0){
			conf = std::string(argv[i]);
			conf = conf.substr(CONF_TOKEN.length()+1);
			moveToEnd(&argc, argv, i);
			i--;
		}
	}
	printf("instance name = %s\n", name.c_str());
	printf("conf = %s\n", conf.c_str());
	printf("%d gstreamer args:\n",argc-1);
	for (int k = 1; k < argc; k++){
		printf("\t%d: %s\n", k,argv[k]);
	}

	printf("dev swap test\n");

	std::string lidar("lidarsrc device=/dev/ttyHS2 simulate=false ! lidarimg ! video/x-raw,width=640,height=480,format=GRAY8,framerate=5/1 ");
	std::string cam("v4l2src device=/dev/video3 ! video/x-raw\(memory:NVMM\), width=\(int\)640, height=\(int\)480, format=\(string\)I420, framerate=\(fraction\)30/1 ! nvvidconv flip-method=2 ! video/x-raw,width=640,height=480,framerate=30/1,format=RGBA");
	printf("Before:\n\t%s\n\t%s\n", lidar.c_str(), cam.c_str());

	devSwap(lidar, std::string("/dev/ttyUSB0"));
	devSwap(cam, std::string("/dev/video0"));

	printf("After:\n\t%s\n\t%s\n", lidar.c_str(), cam.c_str());
	printf("\n\n");

	printf("test timeout\n");

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	bool done = false;
	while (!done){
		Sleep(1000);
		end = std::chrono::system_clock::now();		
		long  millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		printf("millis = %d\n", millis);
		if (millis > BROWSE_DURATION) done = true;
	}
	printf("timeout done\n");
	return 0;
}

void moveToEnd(int* argc, char * argv[], int positionToMoveToEnd){
	//printf("argc=%d, position=%d \n",*argc, positionToMoveToEnd);
	char * temp = argv[positionToMoveToEnd];
	for (int j = positionToMoveToEnd + 1; j < *argc; j++){
		argv[j - 1] = argv[j];
	}
	argv[*argc - 1] = temp;
	//printf("results of move \n ------------------------\n");
	//for (int k = 0; k < *argc; k++){
	//	printf("argv[%d]=%s\n", k, argv[k]);
	//}
	//printf("------------------------\n");
	*argc = *argc-1;
}

#define DEVICE_TOKEN std::string("/dev/")

void devSwap(std::string& line, std::string&device){
	int pos = -1;
	pos = line.find(DEVICE_TOKEN);
	if (pos>=0){
		std::string result;
		int posEnd = line.find(" ", pos);
		result.append(line.substr(0, pos));
		result.append(device);
		result.append(line.substr(posEnd));
		line.clear();
		line.append(result);
	}
}
