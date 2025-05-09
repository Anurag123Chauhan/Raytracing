#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>

const double SCREEN_WIDTH = 900;
const double SCREEN_HEIGHT = 600;
const double Number_Of_Rays = 500;

struct Circle
{
    double x, y;
    double radius;
    bool movingUp = false;
};

struct Ray
{
    double x_start, y_start, x_end, y_end;
    double angle;
};

void FillCircle(SDL_Renderer *renderer, struct Circle circle)
{
    for (double i = circle.x - circle.radius; i <= circle.x + circle.radius; i++)
    {
        for (double j = circle.y - circle.radius; j <= circle.y + circle.radius; j++)
        {
            if ((i - circle.x) * (i - circle.x) + (j - circle.y) * (j - circle.y) <= circle.radius * circle.radius)
            {
                SDL_RenderDrawPoint(renderer, i, j);
            }
        }
    }
}
void MoveVertically(Circle &circle, float speed)
{
    if (circle.movingUp)
    {
        circle.y -= speed;
        // Check top boundary
        if (circle.y - circle.radius < 0)
        {
            circle.y = circle.radius; // Clamp to boundary
            circle.movingUp = false;  // Change direction
        }
    }
    else
    {
        circle.y += speed;
        // Check bottom boundary
        if (circle.y + circle.radius > SCREEN_HEIGHT)
        {
            circle.y = SCREEN_HEIGHT - circle.radius; // Clamp to boundary
            circle.movingUp = true;                   // Change direction
        }
    }
}

void FillRays(struct Circle circle, struct Ray rays[], SDL_Renderer *renderer)
{
    for (int i = 0; i < Number_Of_Rays; i++)
    {
        struct Ray ray = rays[i];
        int end_of_screen = 0;
        int object_hit = 0;
        double steps = 1.0;

        while (!end_of_screen && !object_hit)
        {
            double x_draw = ray.x_start + steps * cos(ray.angle);
            double y_draw = ray.y_start + steps * sin(ray.angle);

            SDL_RenderDrawPoint(renderer, (int)x_draw, (int)y_draw);

            if (x_draw < 0 || x_draw > SCREEN_WIDTH || y_draw < 0 || y_draw > SCREEN_HEIGHT)
            {
                end_of_screen = 1;
            }
            else if ((x_draw - circle.x) * (x_draw - circle.x) +
                         (y_draw - circle.y) * (y_draw - circle.y) <=
                     circle.radius * circle.radius)
            {
                object_hit = 1;
            }
            steps += 1.0;
        }
    }
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Ray_Tracing",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window)
    {
        std::cout << "Window Creation Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cout << "Renderer Creation Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    // optimizing the renderer
    const int TARGET_FPS = 1000;
    const int FRAME_DELAY = 1000 / TARGET_FPS;
    Uint32 frameStart;
    int frameTime;

    // Event Handling
    SDL_Event event;
    bool quit = false;

    // Defining the circle
    Circle circle1 = {200, 200, 30};
    Circle circle2 = {400, 400, 90, true};
    float circleSpeed = 2.0f;

    // Initialize rays
    Ray rays[static_cast<int>(Number_Of_Rays)];
    for (int i = 0; i < Number_Of_Rays; i++)
    {
        rays[i] = {
            circle1.x, circle1.y,         // Start position
            0, 0,                         // End position (will be calculated)
            2 * M_PI * i / Number_Of_Rays // Angle
        };
    }

    while (!quit)
    {
        frameStart = SDL_GetTicks();

        // Event Handling
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_LMASK)) // making bit 0/1 for moving the circle
            {
                circle1.x = event.motion.x;
                circle1.y = event.motion.y;
            }
        }
        // Update ray start positions to follow circle1
        for (int i = 0; i < Number_Of_Rays; i++)
        {
            rays[i].x_start = circle1.x;
            rays[i].y_start = circle1.y;
        }
        MoveVertically(circle2, circleSpeed);

        // Making the background black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Drawing the first circle with yellow color
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        FillCircle(renderer, circle1);
        // Drawing the first circle with White color
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        FillCircle(renderer, circle2);
        // Draw rays
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        FillRays(circle2, rays, renderer);
        // Presenting the renderer(Updating the screen)
        SDL_RenderPresent(renderer);

        // Frame Rate Control
        frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY > frameTime)
        {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // Destroying the renderer and window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}