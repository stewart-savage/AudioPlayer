#pragma once

#include <JuceHeader.h>

class AudioPlayerComponent final : public AudioAppComponent, public ChangeListener
{
public:
    AudioPlayerComponent();
    ~AudioPlayerComponent() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayerComponent);

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
        eStopping
    };

    void changeState(TransportState eTransportState);
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();

    TextButton m_openButton;
    TextButton m_playButton;
    TextButton m_stopButton;

    std::unique_ptr<FileChooser> m_fileChooser;

    AudioFormatManager m_formatManager;
    std::unique_ptr<AudioFormatReaderSource> m_formatReaderSource;

    AudioTransportSource m_transportSource;
    TransportState m_eState;
};
