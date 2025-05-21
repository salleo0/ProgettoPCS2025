#pragma once

#include <iostream>

#include <gtest/gtest.h>
#include "utils.hpp"

const int NUMSEGMENTS = 5;

TEST(TestGeodeticPolyhedron, TestTetrahedronType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Tetraedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	GenerateGeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, NUMSEGMENTS);
	
	int T = NUMSEGMENTS*NUMSEGMENTS;
	int ExpectedVertices = 2*T + 2;
	int ExpectedEdges = 6*T;
	int ExpectedFaces = 4*T;
	
	EXPECT_EQ(GeodeticPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(GeodeticPolyhedron.NumCell1Ds, ExpectedEdges);
	EXPECT_EQ(GeodeticPolyhedron.NumCell2Ds, ExpectedFaces);
}

TEST(TestGeodeticPolyhedron, TestOctahedronType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Ottaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	GenerateGeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, NUMSEGMENTS);
	
	int T = NUMSEGMENTS*NUMSEGMENTS;
	int ExpectedVertices = 4*T + 2;
	int ExpectedEdges = 12*T;
	int ExpectedFaces = 8*T;
	
	EXPECT_EQ(GeodeticPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(GeodeticPolyhedron.NumCell1Ds, ExpectedEdges);
	EXPECT_EQ(GeodeticPolyhedron.NumCell2Ds, ExpectedFaces);
}

TEST(TestGeodeticPolyhedron, TestIcosahedronType1)
{
	PolyhedronMesh PlatonicPolyhedron;
	if (!ImportPolyhedronMesh(PlatonicPolyhedron, "../SolidiPlatonici/Icosaedro/"))
		FAIL() << "Something went wrong during the creation of the platonic polyhedron mesh";

	PolyhedronMesh GeodeticPolyhedron;
	GenerateGeodeticSolidType1(PlatonicPolyhedron, GeodeticPolyhedron, NUMSEGMENTS);
	
	int T = NUMSEGMENTS*NUMSEGMENTS;
	int ExpectedVertices = 10*T + 2;
	int ExpectedEdges = 30*T;
	int ExpectedFaces = 20*T;
	
	EXPECT_EQ(GeodeticPolyhedron.NumCell0Ds, ExpectedVertices);
	EXPECT_EQ(GeodeticPolyhedron.NumCell1Ds, ExpectedEdges);
	EXPECT_EQ(GeodeticPolyhedron.NumCell2Ds, ExpectedFaces);
}