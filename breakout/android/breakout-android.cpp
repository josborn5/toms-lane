#include "../../application/src/tl-application.hpp"
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/looper.h>
#include <android_native_app_glue.h>
#include <cstring>
#include <ctime>

// ─── Internal state ───────────────────────────────────────────────────────────

static ANativeWindow*   g_window        = nullptr;
static AInputQueue*     g_inputQueue    = nullptr;
static bool             g_running       = false;
static bool             g_windowReady   = false;
static int              g_clientX       = 0;
static int              g_clientY       = 0;
static android_app*     g_app           = nullptr;

// ─── android_app command handler ─────────────────────────────────────────────

static void HandleAppCommand(android_app* app, int32_t cmd)
{
    switch (cmd)
    {
        case APP_CMD_INIT_WINDOW:
            if (app->window) {
                g_window      = app->window;
                g_windowReady = true;
            }
            break;

        case APP_CMD_TERM_WINDOW:
            g_windowReady = false;
            g_window      = nullptr;
            break;

        case APP_CMD_DESTROY:
        case APP_CMD_STOP:
            g_running = false;
            break;

        default:
            break;
    }
}

// ─── tl::OpenWindow ──────────────────────────────────────────────────────────

namespace tl
{

int OpenWindow(const WindowSettings& settings)
{
    int dummyX = 0, dummyY = 0;
    return OpenWindow(settings, dummyX, dummyY);
}

int OpenWindow(const WindowSettings& settings, int& outClientX, int& outClientY)
{
    if (!g_app) return -1;   // android_main must set g_app first

    g_app->onAppCmd = HandleAppCommand;

    // Pump events until the surface is ready
    while (!g_windowReady)
    {
        int events;
        android_poll_source* source;
        if (ALooper_pollOnce(0, nullptr, &events,
                            reinterpret_cast<void**>(&source)) >= 0)
        {
            if (source) source->process(g_app, source);
        }
        if (g_app->destroyRequested) return -1;
    }

    // Resize to requested dimensions (best-effort on Android)
    ANativeWindow_setBuffersGeometry(g_window,
                                     settings.width,
                                     settings.height,
                                     WINDOW_FORMAT_RGBX_8888);

    outClientX = g_clientX = ANativeWindow_getWidth(g_window);
    outClientY = g_clientY = ANativeWindow_getHeight(g_window);

    g_running = true;
    return 0;
}

// ─── Input helpers ────────────────────────────────────────────────────────────

static void ProcessInputEvents(Input& input)
{
    // Reset per-frame transient state

    AInputEvent* event = nullptr;
    while (g_app->inputQueue &&
           AInputQueue_getEvent(g_app->inputQueue, &event) >= 0)
    {
        if (AInputQueue_preDispatchEvent(g_app->inputQueue, event)) continue;

        int32_t handled = 0;
        int32_t type    = AInputEvent_getType(event);

        if (type == AINPUT_EVENT_TYPE_MOTION)
        {
            input.mouse.x = static_cast<int>(AMotionEvent_getX(event, 0));
            input.mouse.y = static_cast<int>(AMotionEvent_getY(event, 0));

            int32_t action = AMotionEvent_getAction(event)
                             & AMOTION_EVENT_ACTION_MASK;

            if (action == AMOTION_EVENT_ACTION_DOWN) {
//                input.mouse.leftButton.keyDown    = true;
//                input.mouse.leftButton.isPressed  = true;
            } else if (action == AMOTION_EVENT_ACTION_UP) {
//                input.mouse.leftButton.isPressed  = false;
            }
            handled = 1;
        }
        else if (type == AINPUT_EVENT_TYPE_KEY)
        {
            // Map Android keycodes → your KeyboardInput as needed
            handled = 1;
        }

        AInputQueue_finishEvent(g_app->inputQueue, event, handled);
    }
}

// ─── tl::RunWindowUpdateLoop ──────────────────────────────────────────────────

int RunWindowUpdateLoop(int targetFPS, UpdateWindowCallback updateWindowCallback)
{
    if (!g_window || !updateWindowCallback) return -1;

    const long targetFrameMs = (targetFPS > 0) ? (1000 / targetFPS) : 16;

    Input         input{};
    RenderBuffer  renderBuffer{};

    // Allocate a CPU-side pixel buffer
    renderBuffer.width  = ANativeWindow_getWidth(g_window);
    renderBuffer.height = ANativeWindow_getHeight(g_window);
    renderBuffer.pixels = new uint32_t[renderBuffer.width * renderBuffer.height];

    struct timespec tPrev, tNow;
    clock_gettime(CLOCK_MONOTONIC, &tPrev);

    while (g_running)
    {
        // ── Pump OS events ──────────────────────────────────────────
        int events;
        android_poll_source* source;
        while (ALooper_pollOnce(0, nullptr, &events,
                               reinterpret_cast<void**>(&source)) >= 0)
        {
            if (source) source->process(g_app, source);
        }
        if (g_app->destroyRequested) break;
        if (!g_windowReady) continue;

        // ── Timing ──────────────────────────────────────────────────
        clock_gettime(CLOCK_MONOTONIC, &tNow);
        int dtMs = static_cast<int>(
            (tNow.tv_sec  - tPrev.tv_sec)  * 1000 +
            (tNow.tv_nsec - tPrev.tv_nsec) / 1'000'000);
        tPrev = tNow;

        // ── Input ───────────────────────────────────────────────────
        ProcessInputEvents(input);

        // ── Game / app update ────────────────────────────────────────
        int result = updateWindowCallback(input, dtMs, renderBuffer);
        if (result != 0) break;

        // ── Blit renderBuffer → ANativeWindow ────────────────────────
        ANativeWindow_Buffer surface{};
        if (ANativeWindow_lock(g_window, &surface, nullptr) == 0)
        {
            auto* dst = static_cast<uint32_t*>(surface.bits);
            auto* src = renderBuffer.pixels;

            for (int row = 0; row < surface.height; ++row)
            {
                std::memcpy(dst + row * surface.stride,
                            src + row * renderBuffer.width,
                            renderBuffer.width * sizeof(uint32_t));
            }
            ANativeWindow_unlockAndPost(g_window);
        }

        // ── Frame cap ───────────────────────────────────────────────
        clock_gettime(CLOCK_MONOTONIC, &tNow);
        long elapsedMs =
            (tNow.tv_sec  - tPrev.tv_sec)  * 1000 +
            (tNow.tv_nsec - tPrev.tv_nsec) / 1'000'000;
        if (elapsedMs < targetFrameMs)
        {
            struct timespec ts{};
            ts.tv_nsec = (targetFrameMs - elapsedMs) * 1'000'000L;
            nanosleep(&ts, nullptr);
        }
    }

    delete[] renderBuffer.pixels;
    return 0;
}

} // namespace tl

// ─── Android entry point ──────────────────────────────────────────────────────

// Your existing app entry point — just set g_app before calling your own init
extern "C" void android_main(android_app* app)
{
    g_app = app;

    tl::WindowSettings settings{ 1280, 720, (char*)"My App" };
    int clientW = 0, clientH = 0;

    if (tl::OpenWindow(settings, clientW, clientH) != 0) return;

    tl::RunWindowUpdateLoop(60, [](const tl::Input& input,
                                   int dtMs,
                                   tl::RenderBuffer& rb) -> int
    {
        // Clear to dark blue as a smoke test
        for (int i = 0; i < rb.width * rb.height; ++i)
            rb.pixels[i] = 0xFF00008B;
        return 0;   // return non-zero to quit
    });
}
