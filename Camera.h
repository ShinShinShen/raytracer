#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include "Ray.h"
#include "Vector3.h"

class Camera {
public:
    Camera(Vector3 lookfrom, Vector3 lookat, int imageHeight, double aspectRatio)
        : lookfrom(lookfrom), lookat(lookat), imageHeight(imageHeight), aspectRatio(aspectRatio) {
        updateCamera();
    }

    void rotateYaw(double angleDegrees) {
        double angleRadians = angleDegrees * M_PI / 180.0;

        // Offset vector from lookat to lookfrom (camera's position relative to center)
        Vector3 offset = lookfrom - lookat;

        double x = offset.x();
        double z = offset.z();

        // Y-axis rotation (orbiting in XZ plane)
        double rotatedX = std::cos(angleRadians) * x - std::sin(angleRadians) * z;
        double rotatedZ = std::sin(angleRadians) * x + std::cos(angleRadians) * z;

        // Update camera position
        lookfrom = lookat + Vector3(rotatedX, offset.y(), rotatedZ);

        // Make sure the camera is still looking at the target
        updateCamera();
    }

    Ray getRay(double u, double v) const {
        return Ray(lookfrom, lowerLeftCorner + u * horizontal + v * vertical - lookfrom);
    }

    void updateCamera() {
        Vector3 forward = unitVector(lookat - lookfrom); // View direction
        Vector3 worldUp(0, 1, 0);
        Vector3 right = unitVector(forward.cross(worldUp)); // Camera right vector
        Vector3 up = right.cross(forward); // Recomputed camera up vector

        double viewportHeight = 2.0;
        double viewportWidth = aspectRatio * viewportHeight;
        double focalLength = 1.0;

        horizontal = viewportWidth * right;
        vertical = viewportHeight * up;

        lowerLeftCorner = lookfrom + forward * focalLength - horizontal / 2 - vertical / 2;
    }

private:
    Vector3 lookfrom;
    Vector3 lookat;
    Vector3 lowerLeftCorner;
    Vector3 horizontal;
    Vector3 vertical;
    int imageHeight;
    double aspectRatio;
};

#endif // RAYTRACER_CAMERA_H
