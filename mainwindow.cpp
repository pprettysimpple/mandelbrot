#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "coordinator.h"
#include <QPainter>
#include <QImage>
#include <QPaintEvent>
#include <QColor>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , coordinator(this)
{
    ui->setupUi(this);
    connect(&coordinator, &coordinator_t::output_ready,
            this, &MainWindow::update_widget);
    resize(500, 500);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::call_painting()
{
    coordinator.set_new_job(center_x, center_y, scale, size());
}

void MainWindow::update_widget(const QImage &img)
{
    this->img_to_paint = img;
    QWidget::update();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QMainWindow::paintEvent(event);

    QPainter painter(this);
    painter.setClipRegion(event->region());
    painter.drawImage(0, 0, img_to_paint);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    call_painting();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    int count_degrees = event->angleDelta().y() / 8;
    double count_steps = count_degrees / 15.;
    double zoom_factor = std::pow(this->zoom_scale_per_step, count_steps);
    double dx = (event->position().x() - QWidget::width() / 2) * scale;
    double dy = (event->position().y() - QWidget::height() / 2) * scale;

    center_x -= dx - dx * zoom_factor;
    center_y -= dy - dy * zoom_factor;

    scale *= zoom_factor;
    call_painting();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton)
    {
        prev_mouse_pos = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint offset = event->pos() - prev_mouse_pos;
        center_x += offset.x() * scale;
        center_y += offset.y() * scale;
        prev_mouse_pos = event->pos();
        call_painting();
    }
}
