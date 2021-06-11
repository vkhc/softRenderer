#include "window.h"

int Window::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    m_window = SDL_CreateWindow("Title",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                Screen::WIDTH, Screen::HEIGHT, SDL_WINDOW_SHOWN);
    
    if (m_window == NULL) {
        SDL_Quit();
        return 2;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC);

    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_STATIC, Screen::WIDTH, Screen::HEIGHT);

    if (m_renderer == NULL) {
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 3;
    }
    if (m_texture == NULL) {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 4;
    }
    return 0;
}

void Window::update(Screen& screen) {
    SDL_UpdateTexture(m_texture, NULL, screen.m_buffer.get(), Screen::WIDTH*sizeof(Uint32));
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
    SDL_RenderPresent(m_renderer);
}

int Window::handleEvents() {
    while (SDL_PollEvent(&m_event)) {
        
        switch (m_event.type) {
        case SDL_QUIT:
            return 0;
            break;
        case SDL_KEYDOWN:
            m_keys[m_event.key.keysym.scancode] = true;
            break;
        case SDL_KEYUP:
            m_keys[m_event.key.keysym.scancode] = false;
            break;
        default:
            break;
        }
    }
    return 999;
}

void Window::quit() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyTexture(m_texture);

    SDL_DestroyWindow(m_window);
    SDL_Quit();
}





