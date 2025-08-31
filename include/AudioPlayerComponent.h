#pragma once

#include <JuceHeader.h>

class AudioPlayerComponent final : public AudioAppComponent, public ChangeListener, public Timer
{
public:
    AudioPlayerComponent();
    ~AudioPlayerComponent() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayerComponent)

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void resized() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;

private:
    enum TransportState
    {
        eStopped,
        eStarting,
        ePlaying,
        ePausing,
        ePaused,
        eStopping
    };

    void changeState(TransportState eChangedState);
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();

public:
    void timerCallback() override;

private:
    TextButton m_openButton;
    TextButton m_playButton;
    TextButton m_stopButton;

    Label m_timerLabel;

    std::unique_ptr<FileChooser> m_fileChooser;

    AudioFormatManager m_formatManager;
    std::unique_ptr<AudioFormatReaderSource> m_pFormatReaderSource;

    AudioTransportSource m_transportSource;
    TransportState m_eState;
};
