#ifndef TOYRENDERER_BVH_HPP
#define TOYRENDERER_BVH_HPP

#include "Model.hpp"
#include "Vector.hpp"
#include <vector>
#include <memory>
#include <iostream>


#define ULL unsigned long long

using namespace std;

constexpr int BVH_MAX_DEPTH = 11;

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
    BVHNode(int triangleIndex_) {
        triangleIndex = triangleIndex_;
    }

    AABB aabb;
    ULL triangleIndex = 0;
    ULL triangleCount = 0;
    ULL childIndex = 0;
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

    auto childA = BVHNode(parent.triangleIndex);
    auto childB = BVHNode(parent.triangleIndex);
    outAllNodes.push_back(childA);
    outAllNodes.push_back(childB);

    for (ULL i = parent.triangleIndex; i < parent.triangleIndex + parent.triangleCount; ++i) {
        bool isSideA = outAllTriangles[i].center[splitAxis] < splitPos;
        auto child = isSideA ? childA : childB;

        //Add Triangle
        child.aabb.extend(outAllTriangles[i].posA);
        child.aabb.extend(outAllTriangles[i].posB);
        child.aabb.extend(outAllTriangles[i].posC);
        child.triangleCount++;

        if (isSideA) {
            ULL swapIdx = child.triangleIndex + child.triangleCount - 1;
            swap(outAllTriangles[i], outAllTriangles[swapIdx]);
            childB.triangleIndex++;
        }
    }

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

        outAllTriangles.push_back(Triangle(posA, posB, posC, NA, NB, NC));
        aabb.extend(posA);
        aabb.extend(posB);
        aabb.extend(posC);
    }

    auto root = BVHNode(0);
    root.aabb = aabb;
    root.triangleCount = outAllTriangles.size();

    outAllNodes.push_back(root);

    split(root, 0, outAllNodes, outAllTriangles);
}

#endif //TOYRENDERER_BVH_HPP
