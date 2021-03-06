#include <string>
#include <iostream>
#include <thread>
#include <algorithm>

#include "thpool.h"
#include "myglob.h"
#include "lame_wrapper.h"


void converter(const char* waveFile) {
  
  std::string fileIn(waveFile);
  std::string fileOut = fileIn;
  fileOut.replace(fileIn.size() - 3,  3, "mp3");
  
  if (wavToMp3(fileIn, fileOut) < 0) {
    std::cout << "----> Failed to process " << fileIn << std::endl;
  } else {
    std::cout << "----> Processed  " << fileIn << std::endl;
  }  
}


int main(int argc, char* argv[]) {
  
  if (argc != 2) {
    std::cout << "Please specify *wav files directory" << std::endl;
    return -1;
  }

  std::string dir(argv[1]);
  std::vector<std::string> fileNames;  

  std::string pattern("*.wav");

  //lookup file list
  getFiles(fileNames, dir, pattern);
  
  if (fileNames.size() == 0) {
    std::cout << "No wav files found" << std::endl;
    return -1;
  }
  
  std::cout << "Found " << fileNames.size() 
	    << " wav files, Processing..." << std::endl;
  
  //create thread pool
  unsigned coresNumber = std::thread::hardware_concurrency();
  unsigned poolSize = std::min(coresNumber, fileNames.size());
  threadpool thpool = thpool_init(poolSize);

  for (size_t i = 0; i < fileNames.size(); i++) {
    
    //add jobs
    thpool_add_work(thpool, (void (*)(void*))converter, (void*)fileNames[i].c_str());
  }
  
  //wait for jobs to finish
  thpool_wait(thpool);
  
  std::cout << "Finishing!!!" << std::endl;
  thpool_destroy(thpool);
  
  return 0;
}
