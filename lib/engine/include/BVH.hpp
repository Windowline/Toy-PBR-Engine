#ifndef TOYRENDERER_BVH_HPP
#define TOYRENDERER_BVH_HPP

#include "Vector.hpp"
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

constexpr int BVH_MAX_DEPTH = 12;

struct Triangle {
    Triangle(vec3 a, vec3 b, vec3 c) {
        posA = a;
        posB = b;
        posC = c;
        center = (a + b + c) / 3.0;
    }

    vec3 center;
    vec3 posA;
    vec3 posB;
    vec3 posC;
//    vec3 NA;
//    vec3 NB;
//    vec3 NC;
};

struct BVHNode {
    BVHNode(int triangleIndex_, AABB aabb_) {
        triangleIndex = triangleIndex_;
        aabb = aabb_;
    }

    AABB aabb;
    int triangleIndex;
    int triangleCount;
    int childIndex;
};


pair<int, float> chooseSplit(BVHNode& node) {
    vec3 size = node.aabb.size();
    int splitAxis = size.x > max(size.y, size.z) ? 0 : size.y > size.z ? 1 : 2;
    float splitPos = node.aabb.center()[splitAxis];
    return {splitAxis, splitPos};
}

//GPU friendly
void split(BVHNode& parent,
           int depth,
           vector<BVHNode>& outAllNodes,
           vector<Triangle>& outAllTriangles) {

    if (depth == BVH_MAX_DEPTH)
        return;

    auto [splitAxis, splitPos] = chooseSplit(parent);
    parent.childIndex = outAllNodes.size();

    auto childA = BVHNode(parent.triangleIndex, parent.aabb);
    auto childB = BVHNode(parent.triangleIndex, parent.aabb);
    outAllNodes.push_back(childA);
    outAllNodes.push_back(childB);

    for (int i = parent.triangleIndex; i < parent.triangleIndex + parent.triangleCount; ++i) {
        bool isSideA = outAllTriangles[i].center[splitAxis] < splitPos;
        auto child = isSideA ? childA : childB;

        child.aabb.extend(outAllTriangles[i].posA);
        child.aabb.extend(outAllTriangles[i].posB);
        child.aabb.extend(outAllTriangles[i].posC);
        child.triangleCount++;

        if (isSideA) {
            // Ensure that the triangles of each child node are grouped together.
            // This allows the node to 'store' the triangles with an index and count.
            int swapIdx = child.triangleIndex + child.triangleCount - 1;
            swap(outAllTriangles[i], outAllTriangles[swapIdx]);
            childB.triangleIndex++; //triIndex of childB increases each time we add to childA
        }
    }

    split(childA, depth + 1, outAllNodes, outAllTriangles);
    split(childB, depth + 1, outAllNodes, outAllTriangles);
}


void buildBVH(const vector<vec3>& vertices, const vector<unsigned int>& indices,
              vector<BVHNode>& outAllNodes, vector<Triangle>& outAllTriangles) {

    AABB aabb;
    outAllTriangles.reserve(indices.size() / 3);

    for (int i = 0; i < indices.size(); i += 3) {
        vec3 a = vertices[indices[i + 0]];
        vec3 b = vertices[indices[i + 1]];
        vec3 c = vertices[indices[i + 2]];

        outAllTriangles.push_back(Triangle(a, b, c));
        aabb.extend(a);
        aabb.extend(b);
        aabb.extend(c);
    }

    auto root = BVHNode(0, aabb);

    outAllNodes.push_back(root);

    split(root, 0, outAllNodes, outAllTriangles);
}

#endif //TOYRENDERER_BVH_HPP
