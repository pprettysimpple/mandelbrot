#pragma once

#include <QImage>
#include <QSize>
#include <QThread>
#include <atomic>
#include <complex>
#include <condition_variable>
#include <optional>

struct calculator_t : QObject
{
    Q_OBJECT

public:
    calculator_t(QObject * parent = nullptr);
    ~calculator_t() override;

    // iters < 256 is required
    template <typename Func>
    static std::optional<QImage> calc(double center_x, double center_y, double scale, QSize size, Func cancel_check, size_t iters);
private:
    static double value(double x, double y, size_t steps)
    {
        double zx = x, zy = y;
        for (size_t k = 0; k < steps; ++k)
        {
            auto zx2 = zx * zx; // computation optimization
            auto zy2 = zy * zy; // do not calculate squares two times, but only once
            if (zx2 + zy2 > 4.0)
            {
                return double(k) / steps;
            }
            // square
            zy = 2 * zx * zy;
            zx = zx2 - zy2;
            // add start
            zx += x;
            zy += y;
        }
        return 0;
    }
};

template<typename Func>
std::optional<QImage> calculator_t::calc(double center_x, double center_y, double scale, QSize size, Func cancel_check, size_t iters)
{
    auto img = QImage(size, QImage::Format_RGB32);

    for (int y = 0; y < size.height(); y++)
    {
        if (y % 1000 == 0 && cancel_check()) {
            return std::nullopt;
        }

        auto scan_line = reinterpret_cast<uint*>(img.scanLine(y));
        for (int x = 0; x < size.width(); x++)
        {
            double x_calc = (x - size.width() / 2.) * scale - center_x;
            double y_calc = (y - size.height() / 2.) * scale - center_y;
            double val = calculator_t::value(x_calc, y_calc, iters);
            *scan_line++ = qRgb(0, int(val * 255), 0);
        }
    }
    return img;
}
