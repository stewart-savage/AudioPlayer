#include "MainWindow.h"

MainWindow::MainWindow(const String& strName, Component* pComponent, JUCEApplication& app) :
    DocumentWindow(strName, Desktop::getInstance().getDefaultLookAndFeel().findColour(backgroundColourId), allButtons),
    m_app(app)
{
    setUsingNativeTitleBar(true);
    setContentOwned(pComponent, true);
    setResizable(true, false);
    setResizeLimits(300, 250, 10000, 10000);
    centreWithSize(getWidth(), getHeight());
    Component::setVisible(true);
}

void MainWindow::closeButtonPressed()
{
    m_app.systemRequestedQuit();
}
