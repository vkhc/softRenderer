#ifndef WINDOW_H_
#define WINDOW_H_

#include <SDL.h>
#include <memory>

#include "screen.h"

using std::unique_ptr;
using std::make_unique;


class Window {
private:
    SDL_Window* m_window{nullptr};
    SDL_Renderer* m_renderer{nullptr};
    SDL_Texture* m_texture{nullptr};
    SDL_Event m_event;
    


public:
    unique_ptr<bool[]> m_keys{new bool[350]()};
    int init();
    void update(Screen& screen);
    int handleEvents();
    void quit();
};


#endif