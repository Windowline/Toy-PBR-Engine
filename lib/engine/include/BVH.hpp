#ifndef TOYRENDERER_BVH_HPP
#define TOYRENDERER_BVH_HPP

#include "Model.hpp"
#include "Vector.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;

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

void visitBVH(int idx, const vector<BVHNode>& nodes, function<void(const BVHNode&)> f, int leafStartIdx, const int leafLastIdx) {
    assert(idx >= 0);
    if (idx > leafLastIdx)
        return;

    BVHNode node = nodes[idx];
    if (f)
        f(node);
    cout << idx << endl;
    cout << nodes[idx].triangleStartIdx << endl;
    cout << nodes[idx].triangleEndIdx << endl;
    cout << "==========================" << endl;

    visitBVH(idx * 2, nodes, f, leafStartIdx, leafLastIdx);
    visitBVH(idx * 2 + 1, nodes, f, leafStartIdx, leafLastIdx);
}

int chooseSplit(BVHNode& node) {
    vec3 size = node.aabb.size();
    int splitAxis = size.x > max(size.y, size.z) ? 0 : size.y > size.z ? 1 : 2;
    return splitAxis;
}

void extendAABBFromTriangle(AABB& input, const Triangle& triangle) {
    input.extend(triangle.posA);
    input.extend(triangle.posB);
    input.extend(triangle.posC);
}

void printBVH(const vector<BVHNode>& bvhNodes) {
    cout << "============================" << endl;
    for (int i = 1; i < bvhNodes.size(); ++i) {
        auto n = bvhNodes[i];
        cout << "num: " << n.nodeIdx << " range: " << n.triangleStartIdx << " ~ " << n.triangleEndIdx << endl;
    }
    cout << "============================" << endl;
}


void split(BVHNode& current,
           int depth,
           const int maxDepth,
           vector<BVHNode>& outAllNodes,
           vector<Triangle>& outAllTriangles) {

    if (depth == maxDepth)
        return;

    float splitAxis = chooseSplit(current);
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
    for (int k = start; k <= mid; ++k)
        extendAABBFromTriangle(childA.aabb, outAllTriangles[k]);

    auto childB = BVHNode();
    childB.nodeIdx = current.nodeIdx * 2 + 1;
    childB.triangleStartIdx = mid;
    childB.triangleEndIdx = end;
    childB.triangleCnt = end - mid + 1;
    for (int k = mid; k <= end; ++k)
        extendAABBFromTriangle(childB.aabb, outAllTriangles[k]);

    outAllNodes[childA.nodeIdx] = childA;
    outAllNodes[childB.nodeIdx] = childB;

    split(childA, depth + 1, maxDepth, outAllNodes, outAllTriangles);
    split(childB, depth + 1, maxDepth, outAllNodes, outAllTriangles);
}


void buildBVH(const vector<Vertex>& vertices,
              const vector<unsigned int>& indices,
              const int maxDepth,
              vector<BVHNode>& outAllNodes,
              vector<Triangle>& outAllTriangles) {

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

    outAllNodes = vector<BVHNode>(1 << maxDepth);
    outAllNodes[1] = (root); //[1]: root

    split(root, 1, maxDepth, outAllNodes, outAllTriangles);

//    printBVH(outAllNodes);
}

#endif //TOYRENDERER_BVH_HPP
