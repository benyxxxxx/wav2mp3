
#if defined(__linux__) 

#include <glob.h>
#include "myglob.h"

void getFiles(std::vector<std::string>& names, std::string& directory, 
	      std::string& pattern) {

  glob_t globbuf;
  
  std::string finalPattern = directory;
  if (finalPattern[finalPattern.size() - 1] != '/') {
    finalPattern += "/";
  }
  finalPattern += pattern;

  int err = glob(finalPattern.c_str(), 0, NULL, &globbuf);
  if(err == 0)
    {
      
      for (size_t i = 0; i < globbuf.gl_pathc; i++)
        {
	  names.push_back(globbuf.gl_pathv[i]);
	}      
      globfree(&globbuf);
    }
}

#else

#include <stdio.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <vector>

void getFiles(std::vector<std::string>& names, std::string& directory, 
	      std::string& pattern) {
  
  WIN32_FIND_DATA fd; 
  std::string finalPattern = directory;
  if (finalPattern[finalPattern.size() - 1] != '\\') {
    finalPattern += "\\";
  }  
  finalPattern += pattern;;
  
  HANDLE hFind = ::FindFirstFile(finalPattern.c_str(), &fd); 
  if(hFind != INVALID_HANDLE_VALUE) 
    { 
      do 
	{ 
	  if (! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  ) {	

	    std::string name(directory);
	    name += "\\";
	    name += fd.cFileName;
	    names.push_back(name);
	  } 
	  
	}while(::FindNextFile(hFind, &fd)); 
      ::FindClose(hFind); 
    } 
}
#endif
