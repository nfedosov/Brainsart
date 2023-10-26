#include "simplebarfbwin.h"
#include "qpainter.h"

SimpleBarFBWin::SimpleBarFBWin(QWidget *parent)
    : QWidget{parent}
{


    barHeight = 10;
}



void SimpleBarFBWin::setBarHeight(double height)
{
    int int_height = int(height);
    if (int_height != barHeight)
    {
        barHeight = int_height;
        update(); // Repaint the widget
    }
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
    QRectF barRect(20, height() - barHeight, 50, barHeight);

    // Fill the bar with a color (e.g., blue)
    painter.setBrush(QColor(0, 0, 255)); // Blue color
    painter.drawRect(barRect);
}
