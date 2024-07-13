//#include "BVH.hpp"
//
//using namespace std;
//
//shared_ptr<BVHNode> buildBVH(vector<vec3>& vertices, vector<unsigned int> indices) {
//    AABB bounds;
//
//    for (const vec3& vertex : vertices) {
//        bounds.extend(vertex);
//    }
//
//    vector<Triangle> triangles;
//    triangles.reserve(indices.size() / 3);
//
//    for (int i = 0; i < indices.size(); ++i) {
//        vec3 a = vertices[indices[i + 0]];
//        vec3 b = vertices[indices[i + 1]];
//        vec3 c = vertices[indices[i + 2]];
//        triangles.push_back(Triangle(a, b, c));
//    }
//
//    auto root = make_shared<BVHNode>(0);
//
//    vector<shared_ptr<BVHNode>> allNodes;
//    allNodes.push_back(root);
//
//    vector<shared_ptr<Triangle>> allTriangles;
//
////    split(root, 0, allNodes, allTriangles);
//
//    return root;
//}
//
////GPU friendly
//void split(const shared_ptr<BVHNode>& parent,
//           int depth,
//           vector<shared_ptr<BVHNode>>& allNodes,
//           vector<shared_ptr<Triangle>>& allTriangles) {
//
//    if (depth == BVH_MAX_DEPTH)
//        return;
//
//    auto [splitAxis, splitPos] = chooseSplit(parent);
//
//    parent->childIndex = allNodes.size();
//    auto childA = make_shared<BVHNode>(parent->triangleIndex);
//    auto childB = make_shared<BVHNode>(parent->triangleIndex);
//    allNodes.push_back(childA);
//    allNodes.push_back(childB);
//
//    for (int i = parent->triangleIndex; i < parent->triangleIndex + parent->triangleCount; ++i) {
//        bool isSideA = allTriangles[i]->center[splitAxis] < splitPos;
//        auto child = isSideA ? childA : childB;
//        child->aabb.extend(allTriangles[i]->posA);
//        child->aabb.extend(allTriangles[i]->posB);
//        child->aabb.extend(allTriangles[i]->posC);
//
//        child->triangleCount++;
//
//        if (isSideA) {
//            // Ensure that the triangles of each child node are grouped together.
//            // This allows the node to 'store' the triangles with an index and count.
//            int swapIdx = child->triangleIndex + child->triangleCount - 1;
//            swap(allTriangles[i], allTriangles[swapIdx]);
////            (allTriangles[i], allTriangles[swap]) = (allTriangles[swap], allTriangles[i]);
//            childB->triangleIndex++; //triIndex of childB increases each time we add to childA
//        }
//    }
//
////    split(childA, depth + 1, allNodes, allTriangles);
////    split(childB, depth + 1, allNodes, allTriangles);
//}
//
//pair<int, float> chooseSplit(const unique_ptr<BVHNode>& node) {
//    vec3 size = node->aabb.size();
//    int splitAxis = size.x > max(size.y, size.z) ? 0 : size.y > size.z ? 1 : 2;
//    float splitPos = node->aabb.center()[splitAxis];
//    return {splitAxis, splitPos};
//}
//
//
////void split(const unique_ptr<BVHNode>& parent, int depth) {
////    if (depth == BVH_MAX_DEPTH)
////        return;
////
////    auto [splitAxis, splitPos] = ChooseSplit(parent);
////
////    parent.childIndex = g_AllNodes.Count;
////    Node childA = new () {triangleIndex = parent.triangleIndex};
////    Node childb = new () {triangleIndex = parent.triangleIndex};
////    g_AllNodes.add(childA);
////    g_AllNodes.add(childB);
////
////    for (int i = parent.triangleIndex; i < parent.triangleIndex + parent.triangleCount; ++i) {
////        bool isSideA = g_AllTriangles[i].centre[splitAxis] < splitPos;
////        Node child = isSideA ? childA : child B;
////        child.bounds.growToInclude(g_AllTriangles[i]);
////        child.TriangleCount++;
////
////        if (isSideA) {
////            // Ensure that the triangles of each child node are grouped together.
////            // This allows the node to 'store' the triangles with an index and count.
////            int swap = child.TriangleIndex + child.TriangleCount - 1;
////            (g_AllTriangles[i], g_AllTriangles[swap]) = (g_AllTriangles[swap], g_AllTriangles[i]);
////            childB.triangleIndex++; //triIndex of childB increases each time we add to childA
////        }
////    }
////
////    Split(childA, depth + 1);
////    Split(childB, depth + 1);
////}
