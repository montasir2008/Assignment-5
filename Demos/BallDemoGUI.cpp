#include "Ball.h"
#include "GUI/MiniGUI.h"
#include "ginteractors.h"
using namespace MiniGUI;
using namespace std;

namespace {
    static GCanvas* theCanvas = nullptr;

    class GUI: public ProblemHandler {
    public:
        GUI(GWindow& window): ProblemHandler(window) {
            theCanvas = window.getCanvas();
            add(goButton = new GButton("Start"));
        }

        void actionPerformed(GObservable* source) {
            if (source == goButton) {
                setDemoOptionsEnabled(false);
                goButton->setEnabled(false);

                Rectangle bounceBounds = {
                    0, 0, int(window().getCanvasWidth()), int(window().getCanvasHeight())
                };

                bounceBalls(10, 200, bounceBounds);

                setDemoOptionsEnabled(true);
                goButton->setEnabled(true);

                requestRepaint();
            }
        }

        void repaint() {
            clearDisplay(window(), Color::WHITE());
        }

    private:
        GButton* goButton;
    };
}

void drawCenteredText(double x, double y, double width, double height, const string& text) {
    if (theCanvas) {
        Font font(FontFamily::SERIF, FontStyle::BOLD, 18, Color::WHITE());

        auto render = TextRender::construct(text, { x, y, width, height }, font);
        render->alignCenterHorizontally();
        render->alignCenterVertically();
        render->draw(theCanvas);
    }
}

void drawOval(int x, int y, int width, int height) {
    if (theCanvas) {
        theCanvas->setColor("blue");
        theCanvas->fillOval(x, y, width, height);
    }
}

void clearWindow() {
    if (theCanvas) {
        clearDisplay(theCanvas, Color::WHITE());
    }
}
void updateWindow() {
    if (theCanvas) {
        theCanvas->repaint();
    }
}

GRAPHICS_HANDLER("Bouncing Balls", GWindow& window) {
    return make_shared<GUI>(window);
}
