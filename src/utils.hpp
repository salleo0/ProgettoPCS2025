#pragma once
#include <sstream>
#include "PolyhedronMesh.hpp"

using namespace std;

bool ImportPolyhedronMesh(PolyhedronMesh& polyhedron, 
						const string& InputFileDirectory);

bool ImportCell0Ds(PolyhedronMesh& polyhedron, 
					const string& InputFile);

bool ImportCell1Ds(PolyhedronMesh& polyhedron, 
					const string& InputFile);

bool ImportCell2Ds(PolyhedronMesh& polyhedron, 
					const string& InputFile);
					
bool GenerateGeodeticSolidType1(const PolyhedronMesh& PlatonicPolyhedron,
							PolyhedronMesh& GeodeticSolid,
							const unsigned int& b);
bool CheckDuplicates(const MatrixXd& mat, const Vector3d& vec, unsigned int matSize);


