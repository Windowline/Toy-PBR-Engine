#ifndef TOYRENDERER_BVH_HPP
#define TOYRENDERER_BVH_HPP

#include "Vector.hpp"
#include <vector>
#include <memory>

using namespace std;

constexpr int BVH_MAX_DEPTH = 10;

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
    BVHNode(int triangleIndex_) {
        triangleIndex = triangleIndex_;
    }

    AABB aabb;
    int triangleIndex;
    int triangleCount;
    int childIndex;
};




pair<int, float> chooseSplit(shared_ptr<BVHNode>& node) {
    vec3 size = node->aabb.size();
    int splitAxis = size.x > max(size.y, size.z) ? 0 : size.y > size.z ? 1 : 2;
    float splitPos = node->aabb.center()[splitAxis];
    return {splitAxis, splitPos};
}

//GPU friendly
void split(shared_ptr<BVHNode>& parent,
           int depth,
           vector<shared_ptr<BVHNode>>& allNodes,
           vector<shared_ptr<Triangle>>& allTriangles) {

    if (depth == BVH_MAX_DEPTH)
        return;

    auto [splitAxis, splitPos] = chooseSplit(parent);

    parent->childIndex = allNodes.size();
    auto childA = make_shared<BVHNode>(parent->triangleIndex);
    auto childB = make_shared<BVHNode>(parent->triangleIndex);
    allNodes.push_back(childA);
    allNodes.push_back(childB);

    for (int i = parent->triangleIndex; i < parent->triangleIndex + parent->triangleCount; ++i) {
        bool isSideA = allTriangles[i]->center[splitAxis] < splitPos;
        auto child = isSideA ? childA : childB;
        child->aabb.extend(allTriangles[i]->posA);
        child->aabb.extend(allTriangles[i]->posB);
        child->aabb.extend(allTriangles[i]->posC);

        child->triangleCount++;

        if (isSideA) {
            // Ensure that the triangles of each child node are grouped together.
            // This allows the node to 'store' the triangles with an index and count.
            int swapIdx = child->triangleIndex + child->triangleCount - 1;
            swap(allTriangles[i], allTriangles[swapIdx]);
//            (allTriangles[i], allTriangles[swap]) = (allTriangles[swap], allTriangles[i]);
            childB->triangleIndex++; //triIndex of childB increases each time we add to childA
        }
    }

    split(childA, depth + 1, allNodes, allTriangles);
    split(childB, depth + 1, allNodes, allTriangles);
}


shared_ptr<BVHNode> buildBVH(const vector<vec3>& vertices, const vector<unsigned int>& indices) {
    AABB bounds;

    for (const vec3& vertex : vertices) {
        bounds.extend(vertex);
    }

    vector<shared_ptr<Triangle>> allTriangles;
    allTriangles.reserve(indices.size() / 3);

    for (int i = 0; i < indices.size(); i += 3) {
        vec3 a = vertices[indices[i + 0]];
        vec3 b = vertices[indices[i + 1]];
        vec3 c = vertices[indices[i + 2]];
        allTriangles.push_back(make_shared<Triangle>(a, b, c));
    }

    auto root = make_shared<BVHNode>(0);

    vector<shared_ptr<BVHNode>> allNodes;
    allNodes.push_back(root);

    split(root, 0, allNodes, allTriangles);

    return root;
}

#endif //TOYRENDERER_BVH_HPP
