#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <QWidget>

class ColorWheel : public QWidget
{
    Q_OBJECT

public:
    explicit ColorWheel(QWidget *parent = 0);

    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;
    QColor color();

signals:
    void colorChange(const QColor &color);

public slots:
    void setColor(const int &hue);
    void setColor(const QColor &color);
    void changeColor(const QColor &color);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *);

private:
    QSize initSize;
    QImage wheelImage;
    QPixmap wheel;
    bool mouseDown;
    QPoint lastPos;
    int margin;
    int wheelWidth;
    QRegion wheelRegion;
    QColor current;
    QColor innercolor;
    bool inWheel;
    QColor posColor(const QPoint &point);
    void drawWheelImage(const QSize &newSize);
    void drawIndicator(const int &hue);
    void drawColor(QColor color);
    void composeWheel();

private slots:
    void hueChanged(const int &hue);

};

#endif // COLORWHEEL_H
