#pragma once

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include <QPaintEvent>
#include <QDebug>
#include "calculator.h"
#include "coordinator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void call_painting();

public slots:
    void update_widget(QImage const& img);

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

private:
    Ui::MainWindow *ui;

    coordinator_t coordinator;

    QImage img_to_paint;

    QPoint prev_mouse_pos;
    double zoom_scale_per_step = 0.8;
    double scale = 0.005;
    double center_x = 0; // pixels * scale
    double center_y = 0;
};
