#ifndef TOOLS_H
#define TOOLS_H


#include "includes.h"
#include "defines.h"

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

using namespace std;


class Vector3d {
private:
	double x_;
	double y_;
	double z_;
	double norm_;

public:	
	Vector3d();
	
	Vector3d(double x__, double y__, double z__);
	
	void set(double x__, double y__, double z__);
	
	double norm();
	Vector3d add(Vector3d v);
	Vector3d sub(Vector3d v);
	Vector3d normalize();
	double x();
	double y();
	double z();
};




bool parseList(char* str, double* list);

double generateRandomNumber(double low, double high);

string getExecDirectory();
string deleteExecName(string path);

#endif


