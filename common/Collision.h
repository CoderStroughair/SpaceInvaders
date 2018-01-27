#pragma once
#include "Utilities.h"

glm::vec3 getClosestPointTriangle(glm::vec3 triangle[], glm::vec3 p0);
glm::vec3 getClosestPointLine(glm::vec3 line[], glm::vec3 p0);
glm::vec3 getClosestPointPlane(glm::vec3 normal, glm::vec3 p1, glm::vec3 p0);
float getDistance(glm::vec3 v0, glm::vec3 v1);
float getClosestTriangleTriangle(const glm::vec3 first[], const glm::vec3 second[], glm::vec3 &point1, glm::vec3 &point2);
int getIndexOfClosestPoint(const glm::vec3 vertices[], const int &count, const glm::vec3 &d);

glm::vec3 getClosestPointTriangle(glm::vec3 triangle[], glm::vec3 p0)
{
	glm::vec3 p1 = triangle[0];
	glm::vec3 p2 = triangle[1];
	glm::vec3 p3 = triangle[2];

	//Check Each Voronoi Region

	//Vertex Regions:

	//p1
	if (glm::dot(p0 - p1, p3 - p1) <= 0 && glm::dot(p0 - p1, p2 - p1) <= 0)
	{
		return p1;
	}
	//p2
	if (glm::dot(p0 - p2, p3 - p2) <= 0 && glm::dot(p0 - p2, p1 - p2) <= 0)
	{
		return p2;
	}
	//p3
	if (glm::dot(p0 - p3, p2 - p3) <= 0 && glm::dot(p0 - p3, p1 - p3) <= 0)
	{
		return p3;
	}

	//Edge Regions:

	//<p1, p2>
	if (glm::dot(glm::cross(glm::cross(p3 - p2, p1 - p2), p1 - p2), p0 - p2) >= 0 && glm::dot(p0 - p1, p2 - p1) >= 0 && glm::dot(p0 - p2, p1 - p2) >= 0)
	{
		glm::vec3 line[] = { p1, p2 };
		return getClosestPointLine(line, p0);
	}
	// <p2, p3>
	if (glm::dot(glm::cross(glm::cross(p1 - p3, p2 - p3), p2 - p3), p0 - p3) >= 0 && glm::dot(p0 - p2, p3 - p2) >= 0 && glm::dot(p0 - p3, p2 - p3) >= 0)
	{
		glm::vec3 line[] = { p2, p3 };
		return getClosestPointLine(line, p0);
	}
	// <p3, p1>
	if (glm::dot(glm::cross(glm::cross(p2 - p1, p3 - p1), p3 - p1), p0 - p1) >= 0 && glm::dot(p0 - p3, p1 - p3) >= 0 && glm::dot(p0 - p1, p3 - p1) >= 0)
	{
		glm::vec3 line[] = { p3, p1 };
		return getClosestPointLine(line, p0);
	}

	//It must be the Face Region
	return getClosestPointPlane(glm::cross(p2, p1), p1, p0);
}
glm::vec3 getClosestPointLine(glm::vec3 line[], glm::vec3 p0)
{
	glm::vec3 u = line[1] - line[0];
	return line[0] + glm::normalize(u)*(glm::dot(p0 - line[0], glm::normalize(u)));
}
glm::vec3 getClosestPointPlane(glm::vec3 normal, glm::vec3 p1, glm::vec3 p0)
{
	normal = glm::normalize(normal);
	return (p0 - normal*glm::dot((p0 - p1), normal));
}
float getDistance(glm::vec3 v0, glm::vec3 v1)
{
	float x_sq = (v0.x - v1.x) * (v0.x - v1.x);
	float y_sq = (v0.y - v1.y) * (v0.y - v1.y);
	float z_sq = (v0.z - v1.z) * (v0.z - v1.z);
	return sqrt(x_sq + y_sq + z_sq);
}

//float getClosestTriangleTriangle(const glm::vec3 first[], const glm::vec3 second[], glm::vec3 &point1, glm::vec3 &point2)
//{
//	float distance = FLT_MAX;
//	for (int i = 0; i < 3; i++)
//	{
//		for (int j = 0; j < 3; j++)
//		{
//			if (glm::length(first[i] - second[j]) < distance)
//			{
//				distance = glm::length(first[i] - second[j]);
//				point1 = first[i];
//				point2 = second[j];
//			}
//		}
//	}
//
//	return distance;
//}

float getClosestTriangleTriangle(const glm::vec3 first[], const glm::vec3 second[], glm::vec3 &point1, glm::vec3 &point2)
{
	//Initialise a Simplex set Q
	//Calculate C
	glm::vec3 d = { 1.0f, 1.0f, 0.0f };
	int C_indexes[2];
	C_indexes[0] = getIndexOfClosestPoint(first, 3, d);
	C_indexes[1] = getIndexOfClosestPoint(second, 3, -d);
	glm::vec3 C_a = first[C_indexes[0]];
	glm::vec3 C_b = second[C_indexes[1]];
	glm::vec3 C = C_a - C_b;
	//Calculate B
	d = -d;
	int B_indexes[2];
	B_indexes[0] = getIndexOfClosestPoint(first, 3, d);
	B_indexes[1] = getIndexOfClosestPoint(second, 3, -d);
	glm::vec3 B_a = first[B_indexes[0]];
	glm::vec3 B_b = second[B_indexes[1]];
	glm::vec3 B = B_a - B_b;

	glm::vec3 CB = B - C;
	glm::vec3 CO = -C;
	//check if B is beyond Origin as seen from C, meaning that the origin could concievably be within a triangle with these two.
	if (glm::dot(CO, B) < 0)
		return -1;

	//Calculate A

	//Normally done by vector triple product (CB x CO x CB), but can be expanded out to make fewer calculations (a x (b x c) = b(a · c) - c(a · b))
	d = (CO * glm::dot(CB, CB)) - (CB * glm::dot(CB, CO));
	int A_indexes[2];
	A_indexes[0] = getIndexOfClosestPoint(first, 3, d);
	A_indexes[1] = getIndexOfClosestPoint(second, 3, -d);
	glm::vec3 A_a = first[A_indexes[0]];
	glm::vec3 A_b = second[A_indexes[1]];
	glm::vec3 A = A_a - A_b;

	glm::vec3 triangle[3] = { A, B, C };
	glm::vec3 closest = getClosestPointTriangle(triangle, glm::vec3(0.0, 0.0, 0.0));

	if (closest == glm::vec3(0.0, 0.0, 0.0))
	{
		point1 = glm::vec3();
		point2 = glm::vec3();
		return 0;
	}
	if (closest == A)
	{
		point1 = first[A_indexes[0]];
		point2 = second[A_indexes[1]];
		return glm::length(A);
	}
	if (closest == B)
	{
		point1 = first[B_indexes[0]];
		point2 = second[B_indexes[1]];
		return glm::length(B);
	}
	if (closest == C)
	{
		point1 = first[C_indexes[0]];
		point2 = second[C_indexes[1]];
		return glm::length(C);
	}
	else
	{
		//<A, B>
		if (glm::dot(glm::cross(glm::cross(C - B, A - B), A - B), -B) >= 0 && glm::dot(-A, B - A) >= 0 && glm::dot(-B, A - B) >= 0)
		{
			point1 = (first[A_indexes[0]] + first[B_indexes[0]]) / 2.0f;
			point2 = (first[A_indexes[1]] + first[B_indexes[1]]) / 2.0f;
			return glm::length(point1 - point2);
		}
		// <B, C>
		else if (glm::dot(glm::cross(glm::cross(A - C, B - C), B - C), -C) >= 0 && glm::dot(-B, C - B) >= 0 && glm::dot(-C, B - C) >= 0)
		{
			point1 = (first[B_indexes[0]] + first[C_indexes[0]]) / 2.0f;
			point2 = (first[B_indexes[1]] + first[C_indexes[1]]) / 2.0f;
			return glm::length(point1 - point2);
		}
		// <C, A>
		else if (glm::dot(glm::cross(glm::cross(B - A, C - A), C - A), -A) >= 0 && glm::dot(-C, A - C) >= 0 && glm::dot(-A, C - A) >= 0)
		{
			point1 = (first[C_indexes[0]] + first[A_indexes[0]]) / 2.0f;
			point2 = (first[C_indexes[1]] + first[A_indexes[1]]) / 2.0f;
			return glm::length(point1 - point2);
		}
	}
	cout << "Error - Collision Function" << endl;
	return -1;

	//Compute P of minimum Norm in Q's convex hull

	//if P is the origin, return

	//Else, reduce Q to Q' by removing all points that don't contribute to the calculation of P

	//V = Support Mapping of (Minkowski Difference (a,b)(-P)) = Support Mapping of(A(-P)) - Support Mapping of (B(P))

	//If V isn't any closer to origin than P, stop and return no intersection.

	//Else, add V to Q and go back to step 2.

	return 0;
}

int getIndexOfClosestPoint(const glm::vec3 vertices[], const int &count, const glm::vec3 &d)
{
	int index = 0;
	float maxProduct = glm::dot(d, vertices[index]);
	for (int i = 1; i < count; i++) {
		float product = glm::dot(d, vertices[i]); // may be negative
		if (product > maxProduct) {
			maxProduct = product;
			index = i;
		}
	}
	return index;
}



