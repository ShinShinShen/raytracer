
#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include "HelperFunctions.h"
#include "Interval.h"
#include "Ray.h"
#include "Vector3.h"
#include "Material.h"

class HitRecord {
public:
    Point3 hitPoint_;
    Vector3 surfaceNormal_;
    double distanceAlongRay_{ infinity };
    bool frontFace_{ true };
    //Material surfacematerial

    Point3 hitPoint() const { return hitPoint_; }
    Vector3 surfaceNormal() const { return surfaceNormal_; }
    double distanceAlongRay() const { return distanceAlongRay_; }
    bool frontFace() const { return frontFace_; }

};


class Object {
public:
    virtual std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const = 0;
};


class Sphere : public Object {
public:
    Sphere(Point3 center, double radius) : center_(center), radius_(radius) {}

    std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const override {

        Vector3 oc = ray.origin() - center_;
        auto a = ray.direction().length_squared();
        auto half_b = oc.dot(ray.direction());
        auto c = oc.length_squared() - radius_ * radius_;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant < 0) return std::nullopt;
        auto sqrtD = std::sqrt(discriminant);

        // Try the nearest root in the acceptable range.
        auto root = (-half_b - sqrtD) / a;

        if (!rayInterval.surrounds(root)) {
            root = (-half_b + sqrtD) / a;
            if (!rayInterval.surrounds(root))
                return std::nullopt;
        }

        HitRecord rec;  
        rec.distanceAlongRay_ = root;
        rec.hitPoint_ = ray.at(rec.distanceAlongRay_);
        Vector3 outwardNormal = (rec.hitPoint_ - center_) / radius_;
        setFaceNormal(rec, ray, outwardNormal);

        return rec;
    }

private:
    Point3 center_;
    double radius_;
    const Material* materialPtr_;

    static void setFaceNormal(HitRecord& rec, const Ray& r, const Vector3& outwardNormal) {
        rec.frontFace_ = r.direction().dot(outwardNormal) < 0;
        rec.surfaceNormal_ = rec.frontFace_ ? outwardNormal : -outwardNormal;
    }
};


class Cone : public Object {
public:
    Cone(Point3 apex, double height, double radius)
        : apex_(apex), height_(height), radius_(radius) {
    }

    std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const override {
        Vector3 co = ray.origin() - apex_;

        // Slope factor for cone: tan^2(theta)
        double k = (radius_ / height_) * (radius_ / height_);

        Vector3 d = ray.direction();
        double dx = d.x(), dy = d.y(), dz = d.z();
        double ox = co.x(), oy = co.y(), oz = co.z();

        double a = dx * dx + dz * dz - k * dy * dy;
        double b = 2 * (dx * ox + dz * oz - k * dy * oy);
        double c = ox * ox + oz * oz - k * oy * oy;

        double discriminant = b * b - 4 * a * c;
        if (discriminant < 0) return std::nullopt;

        double sqrtD = std::sqrt(discriminant);
        double root = (-b - sqrtD) / (2 * a);
        if (!rayInterval.surrounds(root)) {
            root = (-b + sqrtD) / (2 * a);
            if (!rayInterval.surrounds(root)) return std::nullopt;
        }

        Point3 hitPoint = ray.at(root);
        double localY = apex_.y() - hitPoint.y();
        if (localY < 0 || localY > height_) return std::nullopt;

        HitRecord rec;
        rec.distanceAlongRay_ = root;
        rec.hitPoint_ = hitPoint;

        // Compute normal
        Vector3 tmp = hitPoint - apex_;
        double slantHeight = std::sqrt(tmp.x() * tmp.x() + tmp.z() * tmp.z());
        Vector3 outwardNormal(tmp.x(), slantHeight * (radius_ / height_), tmp.z());
        outwardNormal = outwardNormal.unitVector();

        setFaceNormal(rec, ray, outwardNormal);

        return rec;
    }

private:
    Point3 apex_;
    double height_;
    double radius_;

    static void setFaceNormal(HitRecord& rec, const Ray& r, const Vector3& outwardNormal) {
        rec.frontFace_ = r.direction().dot(outwardNormal) < 0;
        rec.surfaceNormal_ = rec.frontFace_ ? outwardNormal : -outwardNormal;
    }
};




class Plane : public Object {
public:
    Plane(Point3 point, Vector3 normal)
        : point_(point), normal_(normal.unitVector()) {
    }

    std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const override {
        auto denom = normal_.dot(ray.direction());
        if (std::abs(denom) < 1e-6) {
            // Ray is parallel to the plane
            return std::nullopt;
        }

        double t = (point_ - ray.origin()).dot(normal_) / denom;
        if (!rayInterval.surrounds(t)) return std::nullopt;

        HitRecord rec;
        rec.distanceAlongRay_ = t;
        rec.hitPoint_ = ray.at(t);
        rec.frontFace_ = denom < 0;
        rec.surfaceNormal_ = rec.frontFace_ ? normal_ : -normal_;
        return rec;
    }

private:
    Point3 point_;     // A point on the plane
    Vector3 normal_;   // The normal vector of the plane
};


class Scene : public Object {
public:
    Scene() = default;
    Scene(Object* o) {
        add(o);
    }

    void add(Object* o) {
        objects_.push_back(o);
    }

    void clear() {
        objects_.clear();
    }

    std::optional<HitRecord> rayHit(const Ray& ray, Interval rayInterval) const override {
        HitRecord tempHitRecord;
        bool hitAnything = false;
        double closestSoFar = rayInterval.max();

        for (const auto& o : objects_) {
            if (auto tempHit = o->rayHit(ray, Interval(rayInterval.min(), closestSoFar))) {
                hitAnything = true;
                closestSoFar = tempHit->distanceAlongRay();
                tempHitRecord = *tempHit;
            }
        }

        return hitAnything ? std::optional<HitRecord>{tempHitRecord} : std::nullopt;
    }
private:
    std::vector<Object*> objects_{};
};

#endif //RAYTRACER_SCENE_H
