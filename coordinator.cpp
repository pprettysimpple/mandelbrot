#include "coordinator.h"
#include "mainwindow.h"
#include <iostream>
#include <QPainter>
#include <QtConcurrent/QtConcurrent>

static const size_t THREADS = QThread::idealThreadCount();

coordinator_t::coordinator_t(QObject *parent)
    : QObject(parent)
{

}

coordinator_t::~coordinator_t()
{
    cancel();
}

void coordinator_t::run_job_iters(double center_x, double center_y, double scale, QSize size, size_t iters) {
    auto result_img = std::make_shared<QImage>(size, QImage::Format_RGB32);
//    result_img->fill(qRgb(0, 0, 0));

    auto cnt_in_process = std::make_shared<std::atomic<size_t>>(THREADS);

    auto mutex_result_img = std::make_shared<std::mutex>();

    static uint64_t unique_op_id = 0; // for cancelling operation
    unique_op_id++;

    auto op_id_copy = unique_op_id;
    auto cancel_check = [this, op_id_copy]() -> bool // not shared 'cause can be copied to calc func
    {
        std::lock_guard lg(this->mutex_running);
        return !this->running.count(op_id_copy);
    };

    {
        std::lock_guard lg(mutex_running);
        running.insert(op_id_copy);
    }

    int step = size.width() / THREADS;
    for (size_t i = 0; i < THREADS; i++)
    {
        int cur_size = step;
        if (i + 1 == THREADS)
            cur_size = size.width() - i * step;

        QtConcurrent::run(
                    calculator_t::calc<decltype(cancel_check)>,
                    center_x - (i * step - size.width() / 2.) * scale,
                    center_y,
                    scale,
                    QSize(
                        cur_size,
                        size.height()),
                    cancel_check,
                    iters
        ).then([=](std::optional<QImage> cur_img_opt)
        {
            if (cur_img_opt.has_value()) // otherwise operation is cancelled
            {
                auto cur_img = *cur_img_opt;
                {
                    std::lock_guard lg(*mutex_result_img);
                    QPainter copy_image_painter(&*result_img);
                    copy_image_painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                    copy_image_painter.drawImage(size.width() / THREADS * i, 0, cur_img);
                }
                if (cnt_in_process->fetch_add(-1) == 1u)
                {
                    {
                        std::lock_guard lg(this->mutex_running);
                        this->running.erase(op_id_copy);
                    }

                    emit output_ready(*result_img);
                }
            }
        });
    }
}

void coordinator_t::set_new_job(double center_x, double center_y, double scale, QSize size)
{
    cancel();

//    run_job_iters(center_x, center_y, scale, size, 20);
//    run_job_iters(center_x, center_y, scale, size, 50);
//    run_job_iters(center_x, center_y, scale, size, 200);
    for (size_t k = 50; k <= 150; k += 10) {
        run_job_iters(center_x, center_y, scale, size, k);
    }
}

void coordinator_t::cancel()
{
    std::lock_guard lg(mutex_running);
    running.clear();
}
