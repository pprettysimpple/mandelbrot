#pragma once

#include <QImage>
#include <QSize>
#include <QThread>
#include <atomic>
#include <complex>
#include <condition_variable>
#include <QObject>
#include <QFutureWatcher>
#include <set>

#include "calculator.h"

struct coordinator_t : QObject
{
    Q_OBJECT

public:
    coordinator_t(QObject * parent = nullptr);
    ~coordinator_t() override;

    void set_new_job(double center_x, double center_y, double scale, QSize size);

    void run_job_iters(double center_x, double center_y, double scale, QSize size, size_t iters);

signals:
    void output_ready(QImage const& image);

private:
    void cancel();

    std::set<uint64_t> running;
    std::mutex mutex_running;
};
