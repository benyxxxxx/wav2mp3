#include <string>
#include <iostream>
#include <thread>
#include <algorithm>

#include "thpool.h"
#include "myglob.h"
#include "lame_wrapper.h"


void converter(const char* fileBase) {
  
  std::string fileIn(fileBase);
  std::string fileOut = fileIn;
  fileOut.replace(fileIn.size() - 3,  3, "mp3");
  
  if (wavToMp3(fileIn.c_str(), fileOut.c_str()) < 0) {
    std::cout << "----> Failed to process " << fileIn << std::endl;
  } else {
    std::cout << "----> Processed  " << fileIn << std::endl;
  }  
}


int main(int argc, char* argv[]) {
  
  if (argc < 2) {
    std::cout << "Please specify *wav files directory" << std::endl;
    return -1;
  }

  std::string dir(argv[1]);
  std::vector<std::string> fileNames;  

  getFiles(fileNames, dir, true);
  if (fileNames.size() < 1) {
    std::cout << "\""<<dir << "\" is not a directory." << std::endl;
    return -1;
  }
  
  std::string pattern = fileNames[0] + "*.wav";
  fileNames.resize(0);

  //lookup file list
  getFiles(fileNames, pattern, false);
  
  std::cout << "Found " << fileNames.size() 
	    << " wav files, Processing..." << std::endl;
  
  //create thread pool
  unsigned coresNumber = std::thread::hardware_concurrency();
  unsigned poolSize = std::min(coresNumber, fileNames.size());
  threadpool thpool = thpool_init(std::stoi(/*argv[2])*/poolSize);

  for (size_t i = 0; i < fileNames.size(); i++)
    {
      thpool_add_work(thpool, (void (*)(void*))converter, (void*)fileNames[i].c_str());
    }
  
  //wait for jobs to finish
  thpool_wait(thpool);
  
  std::cout << "Finishing!!!" << std::endl;
  thpool_destroy(thpool);
  
  return 0;
}
