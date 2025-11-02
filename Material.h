#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include "Color3.h"
#include "Ray.h"
#include "Vector3.h"

class Scene;
class HitRecord;

class Material {
public:
    virtual ~Material() = default;

    // Basic shading (used for simple materials like Lambert)
    virtual Color3 shade(const Ray& ray, const HitRecord& rec) const {
        return Color3(0, 0, 0); // Default implementation
    }

    // Optional override: for materials that need access to the scene (e.g. reflection)
    virtual Color3 shade(const Ray& ray, const HitRecord& rec, const Scene& scene) const {
        return shade(ray, rec); // Fallback to simpler version
    }
};

#endif //RAYTRACER_MATERIAL_H