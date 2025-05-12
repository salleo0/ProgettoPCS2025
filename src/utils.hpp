#pragma once
#include <sstream>
#include "Polyhedron.hpp"

using namespace std;

bool ImportCell0Ds(Polyhedron& polyhedron, const string& InputFile);
bool ImportCell1Ds(Polyhedron& polyhedron, const string& InputFile);
bool ImportCell2Ds(Polyhedron& polyhedron, const string& InputFile);
bool ImportPolyhedron(Polyhedron& polyhedron, const string& InputFile);