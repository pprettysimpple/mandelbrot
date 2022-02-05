# Mandelbrot Set Visualization

Mandelbrot set CPU rendering using Qt6. Here is some explanation about sources:

- __calculator_t::calc__ function is dealing with calculations. It can cancel the operation and return empty optional.
To check that operation is canceled it requires a functor object as an argument and calls it whenever it wants.

- __coordinator_t__ class is dealing with tasks management. The interface is simple.
One function sets new parameters for the new frame and exits immediately.
After the frame is fully rendered, the _output_ready_ signal is emitted with a picture as an argument.
Be careful, the signal might be emitted more than one time,
for example, first time with pre-rendered picture, the second time with fully rendered

- __mainwindow__ class is dealing with processing UI events like wheel scroll for zooming, etc.
