#include <SDL.h>
#include <iostream>
#include "Camera.h"
#include "Scene.h"
#include "Renderer.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Raytracer", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    if (!texture) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Allocate pixel buffer (ARGB format)
    uint32_t* pixels = new uint32_t[WINDOW_WIDTH * WINDOW_HEIGHT];

    // Setup raytracing scene, camera, renderer
    Camera camera(
        Vector3(0, 0, 0.3),     // lookfrom
        Vector3(0, 0, -1),    // lookat 
        WINDOW_HEIGHT,
        double(WINDOW_WIDTH) / WINDOW_HEIGHT
    );
    Scene scene;
    Sphere* sphere = new Sphere(Point3(-0.6, 0.0, -1.8), 0.5);
    Cone* cone = new Cone(Point3(0.6, 0.0, -2.2), 2.0, 0.5);
    scene.add(sphere);
    scene.add(cone);
    //Plane* ground = new Plane(Point3(0, -0.5, 0), Vector3(0, 1, 0));
    //scene.add(ground);
    Renderer raytracer(scene, camera);

    raytracer.render(scene, camera, pixels, WINDOW_WIDTH, WINDOW_HEIGHT);

    SDL_UpdateTexture(texture, nullptr, pixels, WINDOW_WIDTH * sizeof(uint32_t));

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                bool rerender = false;

                if (event.key.keysym.sym == SDLK_LEFT) {
                    camera.rotateYaw(-10);  // Rotate left
                    rerender = true;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT) {
                    camera.rotateYaw(10);   // Rotate right
                    rerender = true;
                }

                if (rerender) {
                    raytracer.render(scene, camera, pixels, WINDOW_WIDTH, WINDOW_HEIGHT);
                    SDL_UpdateTexture(texture, nullptr, pixels, WINDOW_WIDTH * sizeof(uint32_t));
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }


    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
