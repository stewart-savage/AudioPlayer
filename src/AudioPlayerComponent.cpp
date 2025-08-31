#include "AudioPlayerComponent.h"

AudioPlayerComponent::AudioPlayerComponent() : m_eState(eStopped)
{
    addAndMakeVisible(&m_openButton);
    m_openButton.setButtonText("Open file...");
    m_openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible(&m_playButton);
    m_playButton.setButtonText("Play");
    m_playButton.setColour(TextButton::buttonColourId, Colours::green);
    m_playButton.onClick = [this] { playButtonClicked(); };
    m_playButton.setEnabled(false);

    addAndMakeVisible(&m_stopButton);
    m_stopButton.setButtonText("Stop");
    m_stopButton.onClick = [this] { stopButtonClicked(); };
    m_stopButton.setEnabled(false);

    addAndMakeVisible(&m_timerLabel);
    m_timerLabel.setText("0:00", dontSendNotification);

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
    if (m_pFormatReaderSource == nullptr)
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

    m_timerLabel.setBounds(10, 100, getWidth() - 20, 20);
}

void AudioPlayerComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &m_transportSource)
    {
        if (m_transportSource.isPlaying())
        {
            changeState(ePlaying);
            this->startTimer(0);
        }
        else if (m_eState == eStopping || m_eState == ePlaying)
        {
            changeState(eStopped);
        }
        else if (m_eState == ePausing)
        {
            changeState(ePaused);
        }
    }
}

void AudioPlayerComponent::changeState(const TransportState eChangedState)
{
    if (m_eState != eChangedState)
    {
        m_eState = eChangedState;

        switch (m_eState)
        {
            case eStopped:
                m_playButton.setButtonText("Play");
                m_playButton.setColour(TextButton::buttonColourId, Colours::green);

                m_stopButton.setButtonText("Stop");
                m_stopButton.setEnabled(false);

                m_transportSource.setPosition(0.0);
                break;

            case eStarting:
                m_transportSource.start();
                break;

            case ePlaying:
                m_playButton.setButtonText("Pause");
                m_playButton.removeColour(TextButton::buttonColourId);

                m_stopButton.setButtonText("Stop");
                m_stopButton.setColour(TextButton::buttonColourId, Colours::red);
                m_stopButton.setEnabled(true);
                break;

            case ePausing:
                m_transportSource.stop();
                break;

            case ePaused:
                m_playButton.setButtonText("Resume");
                m_playButton.setColour(TextButton::buttonColourId, Colours::green);
                m_stopButton.setButtonText("Reset");
                break;

            case eStopping:
                m_transportSource.stop();
                break;
        }
    }
}

void AudioPlayerComponent::openButtonClicked()
{
    m_fileChooser = std::make_unique<FileChooser>("Select .mp3/.wav file", File{}, "*.mp3;*.wav");

    constexpr int chooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

    // When a file is selected, the following is executed:
    const std::function<void(const FileChooser&)> callback = [&](const FileChooser& fc) -> void
    {
        if (const File file = fc.getResult(); file != File{})
        {
            if (AudioFormatReader* pReader = m_formatManager.createReaderFor(file); pReader != nullptr)
            {
                auto pAudioFormatReaderSource = std::make_unique<AudioFormatReaderSource>(pReader, true);
                m_transportSource.setSource(pAudioFormatReaderSource.get(), 0, nullptr, pReader->sampleRate);
                m_playButton.setEnabled(true);
                m_pFormatReaderSource = std::move(pAudioFormatReaderSource);
            }
        }
    };

    m_fileChooser->launchAsync(chooserFlags, callback);
}

void AudioPlayerComponent::playButtonClicked()
{
    if (m_eState == eStopped || m_eState == ePaused)
    {
        changeState(eStarting);
    }
    else if (m_eState == ePlaying)
    {
        changeState(ePausing);
    }
}

void AudioPlayerComponent::stopButtonClicked()
{
    m_eState == ePaused ? changeState(eStopped) : changeState(eStopping);
}

void AudioPlayerComponent::timerCallback()
{
    const int totalSeconds = static_cast<int>(m_transportSource.getCurrentPosition());
    const int mins         = totalSeconds / 60;
    const int seconds      = totalSeconds % 60;

    const String time{seconds < 10 ? std::format("{}:0{}", mins, seconds)
                                   : std::format("{}:{}", mins, seconds)};

    m_timerLabel.setText(time, dontSendNotification);
}
