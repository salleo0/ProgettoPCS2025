#pragma once
#include <sstream>
#include "PolyhedronMesh.hpp"

using namespace std;


bool ImportCell0Ds(PolyhedronMesh& polyhedron, 
					const string& InputFile);

bool ImportCell1Ds(PolyhedronMesh& polyhedron, 
					const string& InputFile);

bool ImportCell2Ds(PolyhedronMesh& polyhedron, 
					const string& InputFile);

bool ImportPolyhedronMesh(PolyhedronMesh& polyhedron, 
						const string& InputFile);