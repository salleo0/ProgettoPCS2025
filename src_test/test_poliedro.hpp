#pragma once

#include <iostream>
#include <gtest/gtest.h>
#include <algorithm>
#include "utils.hpp"

using namespace TriangulationLibrary;

const int NUMSEGMENTS = 5;

int VertexDegree(int& ExpectedDegree, const std::vector<int>& Vertices, const std::vector<std::vector<int>>& Faces) {
	int NumVertexOfExpectedDegree = 0;
	for (const auto& idV : Vertices) {
		int CurrentDegree = 0;
		for (const auto& listF : Faces )
			if (std::find(listF.begin(), listF.end(), idV) != listF.end())
				CurrentDegree++;
		if (CurrentDegree == ExpectedDegree)
			NumVertexOfExpectedDegree++;
	}
	
	return NumVertexOfExpectedDegree;
}		

TEST(TestGeodeticPolyhedron, TestTetrahedronType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Tetraedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, NUMSEGMENTS);
	
	int T = NUMSEGMENTS*NUMSEGMENTS;
	int ExpectedVertices = 2*T + 2;
	int ExpectedEdges = 6*T;
	int ExpectedFaces = 4*T;
	
	EXPECT_EQ(GeodeticPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(GeodeticPolyhedron.NumCell1Ds, ExpectedEdges);
	EXPECT_EQ(GeodeticPolyhedron.NumCell2Ds, ExpectedFaces);
	
	int Degree1 = 3;
	int Degree2 = 6;
	int ExpectedNumDegree1 = 4;
	int ExpectedNumDegree2 = 2*(T-1);
	
	int NumDegree1 = VertexDegree(Degree1, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	int NumDegree2 = VertexDegree(Degree2, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	
	EXPECT_EQ(ExpectedNumDegree1, NumDegree1);
	EXPECT_EQ(ExpectedNumDegree2, NumDegree2);
}

TEST(TestGeodeticPolyhedron, TestOctahedronType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Ottaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, NUMSEGMENTS);
	
	int T = NUMSEGMENTS*NUMSEGMENTS;
	int ExpectedVertices = 4*T + 2;
	int ExpectedEdges = 12*T;
	int ExpectedFaces = 8*T;
	
	EXPECT_EQ(GeodeticPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(GeodeticPolyhedron.NumCell1Ds, ExpectedEdges);
	EXPECT_EQ(GeodeticPolyhedron.NumCell2Ds, ExpectedFaces);
	
	int Degree1 = 4;
	int Degree2 = 6;
	int ExpectedNumDegree1 = 6;
	int ExpectedNumDegree2 = 4*(T-1);
	
	int NumDegree1 = VertexDegree(Degree1, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	int NumDegree2 = VertexDegree(Degree2, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	
	EXPECT_EQ(ExpectedNumDegree1, NumDegree1);
	EXPECT_EQ(ExpectedNumDegree2, NumDegree2);
}

TEST(TestGeodeticPolyhedron, TestIcosahedronType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Icosaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, NUMSEGMENTS);
	
	int T = NUMSEGMENTS*NUMSEGMENTS;
	int ExpectedVertices = 10*T + 2;
	int ExpectedEdges = 30*T;
	int ExpectedFaces = 20*T;
	
	EXPECT_EQ(GeodeticPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(GeodeticPolyhedron.NumCell1Ds, ExpectedEdges);
	EXPECT_EQ(GeodeticPolyhedron.NumCell2Ds, ExpectedFaces);
	
	int Degree1 = 5;
	int Degree2 = 6;
	int ExpectedNumDegree1 = 12;
	int ExpectedNumDegree2 = 10*(T-1);
	
	int NumDegree1 = VertexDegree(Degree1, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	int NumDegree2 = VertexDegree(Degree2, GeodeticPolyhedron.Cell0DsId, GeodeticPolyhedron.Cell2DsVertices);
	
	EXPECT_EQ(ExpectedNumDegree1, NumDegree1);
	EXPECT_EQ(ExpectedNumDegree2, NumDegree2);
	
}

TEST(TestDualPolyhedron, TestType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!FileManagement::ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Tetraedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	Generation::GeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, NUMSEGMENTS);
	
	PolyhedronMesh DualPolyhedron;
	Generation::Dual(GeodeticPolyhedron, DualPolyhedron);
	
	int T = NUMSEGMENTS*NUMSEGMENTS;
	int ExpectedVertices = 4*T;
	int ExpectedFaces = 2*T+2;
	
	EXPECT_EQ(DualPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(DualPolyhedron.NumCell2Ds, ExpectedFaces);
}
