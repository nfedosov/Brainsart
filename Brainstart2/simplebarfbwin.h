#ifndef SIMPLEBARFBWIN_H
#define SIMPLEBARFBWIN_H


#include <QWidget>

class SimpleBarFBWin : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleBarFBWin(QWidget *parent = nullptr);

    int barHeight;
    int signal;
    void setBarHeight(float height);


protected:
    void paintEvent(QPaintEvent *event) override;

signals:

};

#endif // SIMPLEBARFBWIN_H
