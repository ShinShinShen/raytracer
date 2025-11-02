#ifndef RAYTRACER_CHECKERMATERIAL_H
#define RAYTRACER_CHECKERMATERIAL_H


#include <cmath>
#include "Scene.h"
#include "Material.h"
class CheckerMaterial : public Material {
public:
    CheckerMaterial(const Color3& c1, const Color3& c2, double scale = 1.0)
        : color1_(c1), color2_(c2), scale_(scale) {
    }

    virtual Color3 shade(const Ray& ray, const HitRecord& rec) const override {
        double s = scale_;
        double x = rec.hitPoint_.x();
        double y = rec.hitPoint_.y();
        double z = rec.hitPoint_.z();

        int check = static_cast<int>(std::floor(x * s) + std::floor(y * s) + std::floor(z * s));
        bool useFirst = (check % 2) == 0;

        Vector3 lightDir = Vector3(1, 1, -1).unitVector();
        float diffuse = std::max(0.0, rec.surfaceNormal_.dot(lightDir));

        return diffuse * (useFirst ? color1_ : color2_);
    }

private:
    Color3 color1_, color2_;
    double scale_;
};

#endif // RAYTRACER_CHECKERMATERIAL_H