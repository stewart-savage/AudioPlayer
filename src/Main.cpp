#include "Application.h"

/// Standard Windows entry point for GUI application.
/// JUCE handles the loop, and the Application class initialises the actual MainWindow
int WinMain()
{
    // For some reason, JUCE registers a static instance within itself and then calls into it via Application::main()
    // I guess this lets them keep the same init code for apps across different OS's, interesting.
    Application::createInstance = []() -> JUCEApplicationBase*
    {
        return new Application();
    };

    return Application::main();
}
