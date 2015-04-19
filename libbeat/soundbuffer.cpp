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
#include "soundbuffer.h"

namespace libbeat
{

SoundBuffer::SoundBuffer(int size)
{
    this->m_size = size;
    m_Buffer.resize(size);
}

int SoundBuffer::average()
{
    int sum = 0;
    for(int i = 0; i < m_size; i++){
        sum += m_Buffer[i];
    }
    sum /= m_size;
    return sum;
}

int SoundBuffer::average_pwr()
{
    int sum = 0;
    for(uint16_t i = 0; i < m_size; i++)
    {
        if(m_Buffer[i] < 0)
            sum += -1*m_Buffer[i];
        else
            sum += m_Buffer[i];
    }
    sum /= m_size;
    return sum;
}

bool SoundBuffer::write(int pos,int16_t value)
{
        if(pos < m_size)
        {
            m_Buffer[pos] = value;
            return true;
        }
        else
            return false;
}

int16_t SoundBuffer::read(int pos)
{
        return m_Buffer.at(pos);
}

int SoundBuffer::size()
{
    return m_size;
}


} //namespace libbeat
