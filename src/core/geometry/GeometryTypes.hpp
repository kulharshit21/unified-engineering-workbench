#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>

namespace Workbench::Core::Geometry {

struct Vector3D {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};

    bool isFinite() const {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
};

struct Triangle {
    Vector3D normal;
    Vector3D v1;
    Vector3D v2;
    Vector3D v3;

    bool isFinite() const {
        return normal.isFinite() && v1.isFinite() && v2.isFinite() && v3.isFinite();
    }
};

struct BoundingBox {
    Vector3D min{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    Vector3D max{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};

    bool isValid() const {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }

    void expand(const Vector3D &p) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
    }

    Vector3D center() const {
        return Vector3D{ (min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f };
    }

    Vector3D extents() const {
        return Vector3D{ max.x - min.x, max.y - min.y, max.z - min.z };
    }

    float maxDimension() const {
        Vector3D d = extents();
        return std::max({d.x, d.y, d.z});
    }
};

struct StlModel {
    std::string solidName;
    std::vector<Triangle> triangles;
    BoundingBox bounds;

    size_t triangleCount() const { return triangles.size(); }
    size_t vertexCount() const { return triangles.size() * 3; }
};

struct TemperatureNode {
    int nodeId{0};
    double x{0.0};
    double y{0.0};
    double z{0.0};
    double temperatureC{0.0};

    bool isFinite() const {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(temperatureC);
    }
};

struct TemperatureDataset {
    std::vector<TemperatureNode> nodes;
    BoundingBox bounds;
    double minTemperature{std::numeric_limits<double>::max()};
    double maxTemperature{std::numeric_limits<double>::lowest()};
    double meanTemperature{0.0};
    int duplicateCount{0};

    bool isEmpty() const { return nodes.empty(); }
    size_t nodeCount() const { return nodes.size(); }
};

} // namespace Workbench::Core::Geometry
