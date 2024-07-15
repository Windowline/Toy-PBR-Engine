#ifndef TOYRENDERER_BVH_HPP
#define TOYRENDERER_BVH_HPP

#include "Model.hpp"
#include "Vector.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

using namespace std;

constexpr int BVH_MAX_DEPTH = 10;

struct Triangle {
    Triangle(vec3 pa, vec3 pb, vec3 pc, vec3 na, vec3 nb, vec3 nc) {
        posA = pa;
        posB = pb;
        posC = pc;

        NA = na;
        NB = nb;
        NC = nc;

        center = (pa + pb + pc) / 3.0;
    }

    vec3 center;
    vec3 posA;
    vec3 posB;
    vec3 posC;
    vec3 NA;
    vec3 NB;
    vec3 NC;
};

struct BVHNode {
    AABB aabb;
    int triangleStartIdx = 0;
    int triangleEndIdx = 0;
    int triangleCnt = 0;
    int nodeIdx = 0;
};


pair<int, float> chooseSplit(BVHNode& node) {
    vec3 size = node.aabb.size();
    int splitAxis = size.x > max(size.y, size.z) ? 0 : size.y > size.z ? 1 : 2;
    float splitPos = node.aabb.center()[splitAxis];
    return {splitAxis, splitPos};
}

void extendAABBFromTriangle(AABB& input, const Triangle& triangle) {
    input.extend(triangle.posA);
    input.extend(triangle.posB);
    input.extend(triangle.posC);
}

//GPU friendly
void split(BVHNode& current,
           int depth,
           vector<BVHNode>& outAllNodes,
           vector<Triangle>& outAllTriangles) {

    if (depth == BVH_MAX_DEPTH)
        return;

    cout << "depth: " << depth << endl;
    cout << "node : " << current.nodeIdx << " triRange : " << current.triangleStartIdx  << " ~ " << current.triangleEndIdx << endl;
    cout << "boundsX : " << current.aabb.boundsMin.x << " ~ " << current.aabb.boundsMax.x  << endl;
    cout << "========================================" << endl;

    auto [splitAxis, splitPos] = chooseSplit(current);
    int start = current.triangleStartIdx;
    int end = current.triangleEndIdx;
    int mid = start + (current.triangleCnt / 2);

    sort(outAllTriangles.begin() + start,
         outAllTriangles.begin() + end,
         [axis = splitAxis](const Triangle& A, const Triangle& B) {
            if (axis == 0)
                return A.center.x < B.center.x;
            else if (axis == 1)
                return A.center.y < B.center.y;
            else
                return A.center.z < B.center.z;
    });

    auto childA = BVHNode();
    childA.nodeIdx = current.nodeIdx * 2;
    childA.triangleStartIdx = start;
    childA.triangleEndIdx = mid;
    childA.triangleCnt = mid - start + 1;
    for (int k = start; k < mid; ++k)
        extendAABBFromTriangle(childA.aabb, outAllTriangles[k]);

    auto childB = BVHNode();
    childB.nodeIdx = current.nodeIdx * 2 + 1;
    childB.triangleStartIdx = mid;
    childB.triangleEndIdx = end;
    childB.triangleCnt = end - mid + 1;
    for (int k = mid; k <= end; ++k)
        extendAABBFromTriangle(childB.aabb, outAllTriangles[k]);

    outAllNodes.push_back(childA);
    outAllNodes.push_back(childB);

    split(childA, depth + 1, outAllNodes, outAllTriangles);
    split(childB, depth + 1, outAllNodes, outAllTriangles);
}


void buildBVH(const vector<Vertex>& vertices, const vector<unsigned int>& indices,
              vector<BVHNode>& outAllNodes, vector<Triangle>& outAllTriangles) {

    AABB aabb;
    outAllTriangles.reserve(indices.size() / 3);

    for (int i = 0; i < indices.size(); i += 3) {
        vec3 posA = vertices[indices[i + 0]].Position;
        vec3 posB = vertices[indices[i + 1]].Position;
        vec3 posC = vertices[indices[i + 2]].Position;

        vec3 NA = vertices[indices[i + 0]].Normal;
        vec3 NB = vertices[indices[i + 1]].Normal;
        vec3 NC = vertices[indices[i + 2]].Normal;

        Triangle triangle = Triangle(posA, posB, posC, NA, NB, NC);
        outAllTriangles.push_back(triangle);
        extendAABBFromTriangle(aabb,triangle);
    }

    auto root = BVHNode();
    root.triangleStartIdx = 0;
    root.triangleEndIdx = outAllTriangles.size() - 1;
    root.triangleCnt = outAllTriangles.size();
    root.nodeIdx = 1;
    root.aabb = aabb;

    outAllNodes.push_back(root);

    split(root, 0, outAllNodes, outAllTriangles);
}

Triangle searchTriangle(Ray ray, BVHNode& root, vector<Triangle>& outAllTriangles) {



    return tri;
}

#endif //TOYRENDERER_BVH_HPP
