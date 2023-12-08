#include "simplebarfbwin.h"
#include "qpainter.h"

SimpleBarFBWin::SimpleBarFBWin(QWidget *parent)
    : QWidget{parent}
{


    barHeight = 10;
}



void SimpleBarFBWin::setBarHeight(float height)
{
    barHeight = 0;
    /*
    int int_height = int(height);
    if (int_height != barHeight)
    {
        barHeight = int_height;
        update(); // Repaint the widget
    }*/
    //int signal = int(height/255.0);
    float orig = height/2.0;

    if (orig < 0) {
        orig = 0;
    }

    if (orig> 1) {
        orig = 1;
    }

    signal = static_cast<int>(orig * 235);
    signal += 20;

    QString hclr = QString("%1").arg(signal, 2, 16, QLatin1Char('0'));

    // Ensure the length is at least 2 characters by prepending '0' if necessary
    if (hclr.length() < 2) {
        hclr = "0" + hclr;
    }


    QString color = "background-color: #" + hclr.repeated(3) + ";";
    //this->setStyleSheet(color);
   // qDebug() <<color;



    update();
    //update();
    //this->update();
}


//public:
    //BarChart(QWidget *parent = nullptr)
    //    : QWidget(parent), barHeight(50) {}

    // Set the height of the bar and repaint



void SimpleBarFBWin::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // Define the rectangle for the bar chart
    QRectF barRect(0, 0, width(),height());

    // Fill the bar with a color (e.g., blue)
    painter.setBrush(QColor(signal, signal, signal)); // Blue color
    painter.drawRect(barRect);
}
