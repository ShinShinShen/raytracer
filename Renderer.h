#ifndef RAYTRACER_RENDERER_H
#define RAYTRACER_RENDERER_H

#include <fstream>
#include "Camera.h"
#include "Color3.h"
#include "Scene.h"

inline float clamp(float x, float min, float max) {
    return x < min ? min : (x > max ? max : x);
}

inline float randomFloat() {
    return rand() / (RAND_MAX + 1.0f);
}



class RendererParameters {
public:
    static RendererParameters defaultParameters() { return RendererParameters(); }
private:
    //int imageWidth_{ 512 };
    //int imageHeight_{ 512 };
    //int samplesPerPixel_{ 10 };
    //int maximumRecursionDepth_{ 25 };
    //Color3 backgroundColor_{ 0.0, 0.0, 0.0 };
    //std::string fileName_{ "image.ppm" };          //not using this as i decided to use SDL2 window instead
};

class Renderer {
public:
    inline Renderer(const Scene& scene, const Camera& camera)
        : camera_(camera), world_(scene) {
    }

    inline void render(const Scene& scene, const Camera& camera, uint32_t* pixels, int width, int height) {
        const int samplesPerPixel = 4;   //Anti-aliasing value, increase/decrease for more/less jaggles (beware also makes it load muuuuuch slower)
        Vector3 lightDirection = Vector3(1, 1, -1).unitVector();
        Color3 lightColor = Color3(1.0, 1.0, 1.0);

        std::cout << "Starting render with anti-aliasing...\n";

        for (int y = 0; y < height; ++y) {
            if (y % 50 == 0) std::cout << "Scanline " << y << "/" << height << "\n"; // progress output

            for (int x = 0; x < width; ++x) {
                Color3 color(0, 0, 0);

                for (int s = 0; s < samplesPerPixel; ++s) {
                    float u = (x + randomFloat()) / (width - 1);
                    float v = 1.0f - (y + randomFloat()) / (height - 1);

                    Ray ray = camera.getRay(u, v);

                    auto hit = scene.rayHit(ray, Interval(0.001, infinity));
                    if (hit) {
                        
                        Vector3 normal = hit->surfaceNormal_;
                        Vector3 lightDir = Vector3(5, 1, -1).unitVector();  // Light direction
                        Vector3 viewDir = (-ray.direction()).unitVector();  // View (camera) direction

                        // Reflect light around normal
                        Vector3 reflectDir = (2 * normal.dot(lightDir) * normal - lightDir).unitVector();

                        // Material properties
                        Color3 objectColor = Color3(0.0, 0.0, 0.1);   // (R, G, B)
                        Color3 lightColor = Color3(10.0, 10.0, 10.0);    // White light
                        float k_d = 0.8f;    // Diffuse coefficient
                        float k_s = 0.2f;    // Specular coefficient
                        float shininess = 300.0f;  // Gloss factor

                        // Diffuse shading
                        double diffuse = std::max(0.0, normal.dot(lightDir));

                        // Specular highlight
                        double specular = std::pow(std::max(0.0, viewDir.dot(reflectDir)), shininess);

                        // Final shaded color
                        color = k_d * diffuse * objectColor * lightColor + k_s * specular * lightColor;
                    }
                    else {
                        double t = (-0.5 - ray.origin().y()) / ray.direction().y();
                        if (t > 0) {
                            Point3 hitPoint = ray.at(t);

                            int checkX = static_cast<int>(std::floor(hitPoint.x()));
                            int checkZ = static_cast<int>(std::floor(hitPoint.z()));
                            bool isEven = (checkX + checkZ) % 2 == 0;

                            Color3 baseColor = isEven ? Color3(0.9, 0.9, 0.9) : Color3(0.1, 0.1, 0.1);
                            Vector3 normal = Vector3(0, 1, 0);
                            float diffuse = std::max(0.0, normal.dot(lightDirection));
                            color += diffuse * baseColor;
                        }
                        else {
                            Vector3 unitDirection = ray.direction().unitVector();
                            float t = 0.5f * (unitDirection.y() + 1.0f);
                            color += (1.0f - t) * Color3(1.0, 1.0, 1.0) + t * Color3(0.5, 0.7, 1.0);
                        }
                    }
                }

                // Average and gamma correct
                color /= samplesPerPixel;
                color = Color3(std::sqrt(color.x()), std::sqrt(color.y()), std::sqrt(color.z()));

                uint8_t r8 = static_cast<uint8_t>(255.999 * clamp(color.x(), 0.0f, 1.0f));
                uint8_t g8 = static_cast<uint8_t>(255.999 * clamp(color.y(), 0.0f, 1.0f));
                uint8_t b8 = static_cast<uint8_t>(255.999 * clamp(color.z(), 0.0f, 1.0f));
                pixels[y * width + x] = (255 << 24) | (r8 << 16) | (g8 << 8) | b8;
            }
        }

        std::cout << "Rendering complete.\n";
    }

private:
    RendererParameters rendererParams_{};
    Camera camera_;
    Scene world_;
    std::ofstream outFile_;

    Color3 rayColor(const Ray& ray, const Scene& scene, int depth) {
        if (depth <= 0)
            return Color3(0, 0, 0);  // Recursion limit hit

        if (auto hit = scene.rayHit(ray, Interval(0.001, infinity))) {
            Vector3 normal = hit->surfaceNormal_;
            Point3 hitPoint = hit->hitPoint_;

            // Calculate perfect mirror reflection
            Vector3 reflectedDir = ray.direction().unitVector().reflectionAboutNormalVector(normal);

            // Offset the ray origin slightly to avoid shadow acne
            Ray reflectedRay(hitPoint + 0.001 * normal, reflectedDir);

            // Recursively trace the reflected ray
            return rayColor(reflectedRay, scene, depth - 1);
        }

        // Background gradient
        Vector3 unitDirection = ray.direction().unitVector();
        float t = 0.5f * (unitDirection.y() + 1.0f);
        return (1.0f - t) * Color3(1.0, 1.0, 1.0) + t * Color3(0.5, 0.7, 1.0);
    }

};

#endif //RAYTRACER_RENDERER_H
