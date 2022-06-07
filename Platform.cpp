#include "Platform.hpp"

void Platform::Chip8(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        textureWidth, textureHeight);
}

void Platform::~Chip8()
{
    // Basically steps of Constructor in reverse
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDl_Quit();
}

void Platform::Update(void const* buffer, int pitch)
{
    // Fetch the new texture
    SDL_UpdateTexture(texture, nullptr, buffer, pitch); 

    // Clear the renderer
    SDL_RenderClear(renderer); 

    // Copy the new texture to the renderer
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // Apply the texture to the renderer
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys)
{
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            // When exit 
            case SDL_QUIT:
            {
                quit = true;
            } break;

            // When user presses the key
            case SDL_KEYDOWN:
            {

            } break;

            // When user releases the key
            case SDL_KEYUP:
            {

            } break;
        }
    }

    return quit;
}