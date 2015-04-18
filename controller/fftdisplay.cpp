#include "fftdisplay.h"

FFTDisplay::FFTDisplay(libbeat::BeatController *controller, QWidget *parent) :
    QWidget(parent)
{
    m_Controller = controller;
}

void FFTDisplay::paintEvent(QPaintEvent *)
{

    QPainter myPainter(this);
    myPainter.setBrush(Qt::white);
    myPainter.drawRect(QRectF(0,0,width(),height()));

    if(m_Controller == NULL) return;

    myPainter.setPen(Qt::green);
    for(uint16_t i=0;i<4096;i++)
    {
        //Draw the function itself
        myPainter.drawLine(QPoint((double)i/2048*width(),height()-m_Controller->getFFT()->getMagnitude(i)/m_Controller->getFFT()->getMaxMagnitude()*height()),QPoint((double)i/2048*width(),width()));
    }
    uint16_t bands=m_Controller->getAnalyser()->getBands();
    for(uint16_t i=0;i<bands;i++)
    {
        myPainter.setPen(Qt::black);
        if(m_Controller->getAnalyser()->getBeat(i) == true)
            myPainter.setBrush(Qt::red);
        else
            myPainter.setBrush(Qt::NoBrush);
        //Draw the beat indicator for each subband
        QRectF beatRect(QPointF((double)i/bands*width(),0),QPoint((double)(i+1)/bands*width(),0.2*height()));
        myPainter.drawRect(beatRect);
        //Set colors for detection levels
        myPainter.setBrush(Qt::magenta);
        myPainter.setPen(Qt::NoPen);
        //Draw beat detection level
        myPainter.drawRect(QRectF(QPoint((double)i/bands*width(),(height()-m_Controller->getAnalyser()->getBand(i)->getCurrentMaximumRaw()/m_Controller->getFFT()->getMaxMagnitude()*height())),QSizeF(width()*0.005,width()*0.005)));
    }
    //Draw drum beat
    if(m_Controller->getAnalyser()->getDrumBeat())
    {
        myPainter.setBrush(Qt::yellow);
        myPainter.drawEllipse(QPointF(width()*0.30,height()/2),qMin(width()*0.2, height()*0.2), qMin(width()*0.2, height()*0.2));
    }
    //Draw snare beat
    if(m_Controller->getAnalyser()->getSnareBeat())
    {
        myPainter.setBrush(Qt::blue);
        myPainter.drawEllipse(QPointF(width()*0.70,height()/2),qMin(width()*0.2, height()*0.2), qMin(width()*0.2, height()*0.2));
    }
    //Draw volume frame
    myPainter.setBrush(Qt::NoBrush);
    myPainter.setPen(Qt::black);
    myPainter.drawRect(QRectF(0.89*width(), 0.92*height(), 0.07*width(), -0.52*height()));
    //Draw volume
    myPainter.setBrush(Qt::green);
    myPainter.setPen(Qt::black);
    myPainter.drawRect(QRectF(0.90*width(), 0.90*height(), 0.05*width(), (log(m_Controller->getBuffer()->average_pwr())/log(65536))*-0.50*height()));
}

void FFTDisplay::drawNewData()
{
    update();
}
