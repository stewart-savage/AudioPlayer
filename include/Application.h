#pragma once

#include "MainWindow.h"

class Application final : public JUCEApplication
{
public:
    Application();

    const String getApplicationName() override;
    const String getApplicationVersion() override;

    void initialise([[maybe_unused]] const String& commandLineParameters) override;
    void shutdown() override;

private:
    std::unique_ptr<MainWindow> m_mainWindow;
};
