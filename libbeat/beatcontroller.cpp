/*
    This file is part of libbeat - a lightweight beat detection library
    Copyright (c) 2011 by Maximilian Güntner <maximilian.guentner@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "beatcontroller.h"

namespace libbeat
{

BeatController::BeatController(QAudioDeviceInfo inputDevice, uint16_t recordSize, uint32_t sampleRate, uint16_t m_bandCount, QObject *parent) : QObject(parent)
{
    m_RecordSize = recordSize;
    m_Buffer = new SoundBuffer(recordSize);
    m_Analyser = new BeatAnalyser(m_bandCount,sampleRate,recordSize);
    m_inputDevice = QAudioDeviceInfo(inputDevice);

    m_FFT = new FFT(recordSize);
    m_FFT->setSoundBuffer(m_Buffer);
    m_Analyser->setFFT(m_FFT);

    QAudioFormat audioFormat;
    audioFormat.setSampleRate(sampleRate);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setCodec("audio/pcm");

    //QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());

    if (!m_inputDevice.isFormatSupported(audioFormat)) {
        qWarning() << "Default format not supported - trying to use nearest";
        audioFormat = m_inputDevice.nearestFormat(audioFormat);
    }

    m_audioInput = new QAudioInput(m_inputDevice, audioFormat, this);
    m_ioDevice = m_audioInput->start();
    connect(m_ioDevice, SIGNAL(readyRead()), this, SLOT(readAudio()));

}

BeatController::~BeatController()
{
    m_ioDevice->close();
    m_audioInput->stop();
    //delete m_FFT;
    //delete m_Buffer;
    //delete m_Analyser;
    //delete m_audioInput;
}

void BeatController::readAudio()
{
    qint64 len = m_audioInput->bytesReady();

    if(len > m_RecordSize*2){
        char data[m_RecordSize*2];
        const qint16 *data16 = reinterpret_cast<const qint16 *>(&data);
        m_ioDevice->read(data, m_RecordSize*2);
        for(int i = 0; i < m_RecordSize; i++){
            qint16 v = data16[i];
            m_Buffer->write(i, v);
        }
        processNewData();
    }
}

void BeatController::processNewData()
{

    m_FFT->processData();
    m_Analyser->processData();
    emit processingDone();
    if(m_Analyser->getDrumBeat())
        emit beatDrum();
    if(m_Analyser->getSnareBeat())
        emit beatSnare();
    //Check for a beat for every frequency in our list
    QSet<uint16_t> myBeats;
    QSetIterator<uint16_t> i(m_customBeats);
    while(i.hasNext())
    {
        if(m_Analyser->getBeatFrequency(i.peekNext()))
            myBeats.insert(i.peekNext());
        i.next();
    }
    if(!myBeats.empty())
        emit beatCustom(myBeats);
}

} //namespace libbeat
