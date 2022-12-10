#ifndef MPGMapProcessor_h
#define MPGMapProcessor_h

#include "Geometry.h"
#include <string>



class MPGMapProcessor {
    const std::string &_path;
public:
    MPGMapProcessor(const std::string &path);
};

#endif
