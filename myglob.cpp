
#if defined(__linux__) || defined(__CYGWIN__)

#include <glob.h>
#include "myglob.h"

void getFiles(std::vector<std::string>& names, std::string& pattern) {

  glob_t globbuf;
  int err = glob(pattern.c_str(), 0, NULL, &globbuf);
  if(err == 0)
    {
      
      for (size_t i = 0; i < globbuf.gl_pathc; i++)
        {
	  names.push_back(globbuf.gl_pathv[i]);
	}      
      globfree(&globbuf);
    }
}

#elif _WIN32

#include <stdio.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <vector>

void getFiles(std::vector<std::string>& names, std::string& pattern) {
  
  WIN32_FIND_DATA fd; 
  HANDLE hFind = ::FindFirstFile(pattern.c_str(), &fd); 
  if(hFind != INVALID_HANDLE_VALUE) 
    { 
      do 
	{ 
	  if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
	    {
	      
	      names.push_back(fd.cFileName);
	    } 
	}while(::FindNextFile(hFind, &fd)); 
      ::FindClose(hFind); 
    } 
}
#endif
