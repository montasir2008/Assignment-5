#include "ToneMatrix.h"
#include "AudioSystem.h"
#include "GUI/MiniGUI.h"
#include "DrawRectangle.h"
#include "gthread.h"
#include "ginteractors.h"
#include "map.h"
#include <algorithm>
using namespace std;

namespace {
    const Color kBackgroundColor(0x10, 0x10, 0x10);

    /* Minimum size of a light. */
    const int kMinSize = 5;

    /* Window border padding. */
    const double kWindowBorderPadding = 20;

    const int kGridSizes[] = {4, 6, 8, 9, 12, 16, 18};

    class GUI: public ProblemHandler {
    public:
        GUI(GWindow& window) : ProblemHandler(window) {
            /* Used to ensure each instance of the GUI has its own distinct
             * radio buttons.
             */
            static int groupIndex = 0;
            groupIndex++;

            /* List of sizes; default is 16. */
            for (int size: kGridSizes) {
                auto* button = new GRadioButton(to_string(size) + " × " + to_string(size), "group-" + to_string(groupIndex), size == 16);
                sizeMap[button] = size;
                add(button);
            }

            /* This is a standard high-fidelity sample rate. */
            AudioSystem::setSampleRate(44100);

            /* Figure out the size of the biggest square that
             * (1) fits into the window and
             * (2) has a size that's a multiple of the grid size.
             */
            int minDimension = min(window.getCanvasWidth(), window.getCanvasHeight()) - 2 * kWindowBorderPadding;

            /* Initially use the default 16 x 16 grid. */
            gridSize = 16;

            /* Size the lights so the largest possible grid size will fit. */
            cellSize = max(kMinSize, minDimension / *max_element(begin(kGridSizes), end(kGridSizes)));
            matrix = new ToneMatrix(gridSize, cellSize);

            baseX = kWindowBorderPadding + (window.getCanvasWidth()  - 2 * kWindowBorderPadding - cellSize * gridSize) / 2;
            baseY = kWindowBorderPadding + (window.getCanvasHeight() - 2 * kWindowBorderPadding - cellSize * gridSize) / 2;

            /* Hook ToneMatrix::draw() into the graphics system. */
            setDrawFunction([&](Rectangle bounds, Color color) {
                window.setColor(color.toRGB());
                window.fillRect(bounds.x + baseX, bounds.y + baseY, bounds.width, bounds.height);

                window.setColor(Color(color.red() / 2, color.green() / 2, color.blue() / 2).toRGB());
                window.drawRect(bounds.x + baseX, bounds.y + baseY, bounds.width, bounds.height);
            });

            /* Hook it into the audio system as well. */
            AudioSystem::play([=](double* buffer, int toRead) {
                for (int i = 0; i < toRead; i++) {
                    buffer[i] = matrix->nextSample();
                }
            });
        }

        ~GUI() {
            AudioSystem::stop();
            delete matrix;
            setDrawFunction(nullptr);
        }

        /* Forward mouse presses to the Tone Matrix. */
        void mousePressed(double x, double y) override {
            int mouseX = x - baseX;
            int mouseY = y - baseY;
            if (mouseX >= 0 && mouseY >= 0 && mouseX < cellSize * gridSize && mouseY < cellSize * gridSize) {
                GThread::runOnQtGuiThread([&] {
                    matrix->mousePressed(mouseX, mouseY);
                });
                requestRepaint();
            }
        }

        /* Forward mouse movement to the Tone Matrix. */
        void mouseDragged(double x, double y) override {
            int mouseX = x - baseX;
            int mouseY = y - baseY;
            if (mouseX >= 0 && mouseY >= 0 && mouseX < cellSize * gridSize && mouseY < cellSize * gridSize) {
                GThread::runOnQtGuiThread([&] {
                    matrix->mouseDragged(mouseX, mouseY);
                });
                requestRepaint();
            }
        }

        /* Use the ToneMatrix::draw() to render to the canvas. */
        void repaint() override {
            window().clearCanvas();
            window().setColor(kBackgroundColor.toRGB());
            window().fillRect(canvasBounds());
            matrix->draw();
        }

        /* Allow the user to change the dimensions. */
        void changeOccurredIn(GObservable* source) override {
            if (sizeMap.containsKey(source)) {
                GThread::runOnQtGuiThread([&] {
                    gridSize = sizeMap[source];
                    baseX = kWindowBorderPadding + (window().getCanvasWidth()  - 2 * kWindowBorderPadding - cellSize * gridSize) / 2;
                    baseY = kWindowBorderPadding + (window().getCanvasHeight() - 2 * kWindowBorderPadding - cellSize * gridSize) / 2;
                    matrix->resize(gridSize);
                    requestRepaint();
                });
            }
        }

    private:
        int baseX, baseY, cellSize, gridSize;

        ToneMatrix* matrix;
        Map<GObservable*, int> sizeMap;
    };
}

GRAPHICS_HANDLER("Tone Matrix", GWindow& window) {
    return make_shared<GUI>(window);
}
