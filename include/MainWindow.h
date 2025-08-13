#pragma once

#include <JuceHeader.h>

class MainWindow final : public DocumentWindow
{
public:
    MainWindow(const String& strName, Component* pComponent, JUCEApplication& app);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)

    void closeButtonPressed() override;

private:
    JUCEApplication& m_app;
};
