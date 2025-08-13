#include "AudioPlayerComponent.h"

AudioPlayerComponent::AudioPlayerComponent() : m_eState(eStopped)
{
    addAndMakeVisible(&m_openButton);
    m_openButton.setButtonText("Open file...");
    m_openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible(&m_playButton);
    m_playButton.setButtonText("Play");
    m_playButton.onClick = [this] { playButtonClicked(); };
    m_playButton.setEnabled(false);

    addAndMakeVisible(&m_stopButton);
    m_stopButton.setButtonText("Stop");
    m_stopButton.onClick = [this] { stopButtonClicked(); };
    m_stopButton.setEnabled(false);

    setSize(300, 200);

    m_formatManager.registerBasicFormats();
    m_transportSource.addChangeListener(this);

    setAudioChannels(0, 2);
}

AudioPlayerComponent::~AudioPlayerComponent()
{
    shutdownAudio();
}

void AudioPlayerComponent::prepareToPlay(const int samplesPerBlockExpected, const double sampleRate)
{
    m_transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioPlayerComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    if (m_formatReaderSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    m_transportSource.getNextAudioBlock(bufferToFill);
}

void AudioPlayerComponent::releaseResources()
{
    m_transportSource.releaseResources();
}

void AudioPlayerComponent::resized()
{
    m_openButton.setBounds(10, 10, getWidth() - 20, 20);
    m_playButton.setBounds(10, 40, getWidth() - 20, 20);
    m_stopButton.setBounds(10, 70, getWidth() - 20, 20);
}

void AudioPlayerComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &m_transportSource)
    {
        if (m_transportSource.isPlaying())
        {
            changeState(ePlaying);
        }
        else
        {
            changeState(eStopped);
        }
    }
}

void AudioPlayerComponent::changeState(const TransportState eTransportState)
{
    if (m_eState != eTransportState)
    {
        m_eState = eTransportState;

        switch (m_eState)
        {
            case eStopped:
                m_stopButton.setEnabled(false);
                m_playButton.setEnabled(true);
                m_transportSource.setPosition(0.0);
                break;

            case eStarting:
                m_playButton.setEnabled(false);
                m_transportSource.start();
                break;

            case ePlaying:
                m_stopButton.setEnabled(true);
                break;

            case eStopping:
                m_transportSource.stop();
                break;
        }
    }
}

void AudioPlayerComponent::openButtonClicked()
{
    m_fileChooser = std::make_unique<FileChooser>("Select a .wav file to play...", File{}, "*.wav");

    constexpr int chooserFlags = FileBrowserComponent::openMode | FileBrowserComponent:: canSelectFiles;

    // When a file is selected, the following is executed:
    const std::function<void(const FileChooser&)> callback = [&](const FileChooser& fc) -> void
    {
        if (const File file = fc.getResult(); file != File{})
        {
            if (AudioFormatReader* reader = m_formatManager.createReaderFor(file); reader != nullptr)
            {
                auto pAudioFormatReaderSource = std::make_unique<AudioFormatReaderSource>(reader, true);
                m_transportSource.setSource(pAudioFormatReaderSource.get(), 0, nullptr, reader->sampleRate);
                m_playButton.setEnabled(true);
                m_formatReaderSource.reset(pAudioFormatReaderSource.release());
            }
        }
    };

    m_fileChooser->launchAsync(chooserFlags, callback);
}

void AudioPlayerComponent::playButtonClicked()
{
    changeState(eStarting);
}

void AudioPlayerComponent::stopButtonClicked()
{
    changeState(eStopping);
}
