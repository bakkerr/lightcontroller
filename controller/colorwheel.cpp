#include "colorwheel.h"

#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QtCore/qmath.h>
#include <QDebug>

ColorWheel::ColorWheel(QWidget *parent) :
    QWidget(parent),
    initSize(100,100),
    mouseDown(false),
    margin(0),
    wheelWidth(30),
    current(Qt::blue),
    innerColor(Qt::blue),
    inWheel(false)
{
    //resize(initSize);
    current = current.toHsv();
    //setMinimumSize(200,200);
    setCursor(Qt::CrossCursor);
}

QColor ColorWheel::color()
{
    return innerColor;
}

void ColorWheel::setColor(const QColor &color)
{
    setColor(color.hue());
}

void ColorWheel::setInnerColor(const QColor &color)
{
    innerColor = color;
    repaint();
}

void ColorWheel::changeColor(const QColor &color)
{
    setColor(color);
    emit colorChange(color);
}

void ColorWheel::setColor(const int &hue)
{
    if( hue<0 ||hue>359)return;
    int s = current.saturation();
    int v = current.value();
    current.setHsv(hue, s, v);
    innerColor = current;
    if(!isVisible()) return;
    repaint();
}

QColor ColorWheel::posColor(const QPoint &point)
{
    if( ! wheel.rect().contains(point) ) return QColor();
    if(inWheel){
        qreal hue = 0;
        int r = qMin(width(), height()) / 2;
        if( point.x() > r ){
            if(point.y() < r ){
                //1
                hue = 90 - (qAtan2( (point.x() - r) , (r - point.y()) )  / 3.14 / 2 * 360);
            }else{
                //4
                hue = 270 + (qAtan2( (point.x() - r) , (point.y() - r ) )  / 3.14 / 2 * 360);
            }
        }else{
            if(point.y() < r ){
                //2
                hue =  90 + (qAtan2( (r - point.x()) , (r - point.y()) )  / 3.14 / 2 * 360);
            }else{
                //3
                hue =  270 - (qAtan2( (r - point.x()) , (point.y() - r ))  / 3.14 / 2 * 360);
            }
        }
        hue = hue>359?359:hue;
        hue = hue<0?0:hue;
        return QColor::fromHsv(hue,
                               current.saturation(),
                               current.value());
    }

    return QColor();
}

QSize ColorWheel::sizeHint () const
{
    return QSize(height(),height());
}

QSize ColorWheel::minimumSizeHint () const
{
    return initSize;
}


void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if(wheelRegion.contains(lastPos)){
        inWheel = true;
        QColor color = posColor(lastPos);
        hueChanged(color.hue());
    }

    mouseDown = true;
}

void ColorWheel::mouseMoveEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if( !mouseDown ) return;
    if(wheelRegion.contains(lastPos) && inWheel){
        QColor color = posColor(lastPos);
        hueChanged(color.hue());
    }else{
        // TODO: due with cursor out of region after press
        //        int length = qMin(width(), height());
        //        QPoint center(length/2, length/2);
        //        int R = qSqrt(qPow(qAbs(lastPos.x()), 2)
        //                      + qPow(qAbs(lastPos.y()), 2));
        //        if(inWheel){
        //            int r =  length / 2;
        //            r += qSqrt(qPow(center.x(), 2) + qPow(center.y(), 2));
        //            int x0 = r/R * qAbs(lastPos.x());
        //            int y0 = r/R * qAbs(lastPos.y());
        //            QColor color = posColor(QPoint(x0, y0));
        //            hueChanged(color.hue());
        //        }else if(inSquare){
        //            //
        //        }
    }
}

void ColorWheel::mouseReleaseEvent(QMouseEvent *)
{
    mouseDown = false;
    inWheel = false;
}

void ColorWheel::resizeEvent(QResizeEvent *event)
{
    wheel = QPixmap(event->size());
    wheel.fill(palette().background().color());
    drawWheelImage(event->size());
    update();
}

void ColorWheel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOption opt;
    opt.initFrom(this);
    composeWheel();
    painter.drawPixmap(0,0,wheel);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ColorWheel::drawWheelImage(const QSize &newSize)
{

    int r = qMin(newSize.width(), newSize.height());
    int wOff = 0; //(newSize.width() - r) / 2;
    int hOff = 0; //(newSize.height() - r) / 2;

    QStyleOption option;
    option.initFrom(this);

    QBrush background = option.palette.window();

    wheelImage = QImage(newSize, QImage::Format_ARGB32_Premultiplied);
    wheelImage.fill(background.color());
    QPainter painter(&wheelImage);
    painter.setRenderHint(QPainter::Antialiasing);

    QConicalGradient conicalGradient(0, 0, 0);
    conicalGradient.setColorAt(0.0, Qt::red);
    conicalGradient.setColorAt(60.0/360.0, Qt::yellow);
    conicalGradient.setColorAt(120.0/360.0, Qt::green);
    conicalGradient.setColorAt(180.0/360.0, Qt::cyan);
    conicalGradient.setColorAt(240.0/360.0, Qt::blue);
    conicalGradient.setColorAt(300.0/360.0, Qt::magenta);
    conicalGradient.setColorAt(1.0, Qt::red);

    /* outer circle */
    painter.translate(r / 2 + wOff, r / 2 + hOff);

    QBrush brush(conicalGradient);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(0,0),r/2-margin,r/2-margin);
    /* inner circle */
    painter.setBrush(background);
    painter.drawEllipse(QPoint(0,0),r/2-margin-wheelWidth,r/2-margin-wheelWidth);

    //caculate wheel region
    wheelRegion = QRegion(r/2, r/2, r-2*margin, r-2*margin, QRegion::Ellipse);
    wheelRegion.translate(-(r-2*margin)/2, -(r-2*margin)/2);

    int tmp = 2*(margin+wheelWidth);
    QRegion subRe( r/2, r/2, r-tmp, r-tmp, QRegion::Ellipse );
    subRe.translate(-(r-tmp)/2, -(r-tmp)/2);
    wheelRegion -= subRe;
}

void ColorWheel::drawColor(QColor color){
    QPainter painter(&wheel);

    int r = qMin(width(), height());
    int wOff = 0; //(width() - r) / 2;
    int hOff = 0; //(height() - r) / 2;

    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(r / 2 + wOff, r / 2 + hOff);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(color));
    painter.drawEllipse(QPoint(0,0),r/2-margin-wheelWidth-10,r/2-margin-wheelWidth-10);
}

void ColorWheel::drawIndicator(const int &hue)
{
    QPainter painter(&wheel);
    painter.setRenderHint(QPainter::Antialiasing);
    if(hue > 20 && hue < 200){
        painter.setPen(Qt::black);
    }else{
        painter.setPen(Qt::white);
    }
    painter.setBrush(Qt::NoBrush);

    QPen pen = painter.pen();
    pen.setWidth(3);
    painter.setPen(pen);
    qreal r = qMin(height(), width()) / 2.0;
    painter.translate(r, r);
    painter.rotate( -hue );
    r = qMin(height(), width()) / 2.0 - margin - wheelWidth/2;
    painter.drawEllipse(QPointF(r,0.0),5,5);
}

void ColorWheel::composeWheel()
{
    QPainter composePainter(&wheel);
    composePainter.drawImage(0, 0, wheelImage);
    composePainter.end();
    drawIndicator(current.hue());
    drawColor(innerColor);
}

void ColorWheel::hueChanged(const int &hue)
{
    setColor(hue);
    emit colorChange(current);
}
