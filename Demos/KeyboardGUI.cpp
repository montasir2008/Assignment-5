#include "GUI/MiniGUI.h"
#include "StringInstrument.h"
#include "AudioSystem.h"
#include "GUI/Color.h"
#include "vector.h"
#include "map.h"
#include "set.h"
#include "gtimer.h"
#include "gthread.h"
#include <fstream>
using namespace std;
using namespace MiniGUI;

namespace {
    /* Middle C. */
    const double kBaseFrequency = 440.0 * pow(2, 3.0 / 12.0);

    /* Where to find the key positions. */
    constexpr char kKeyboardFile[] = "res/keyboard.txt";

    /* Font for labeling the keys. */
    const Font kKeyFont(FontFamily::SANS_SERIF, FontStyle::BOLD, 18, Color::fromHex(0x808080));

    /* Font for instructions. */
    const Font kInstructionFont(FontFamily::SERIF, FontStyle::NORMAL, 16, Color::BLACK());

    /* Vertical space for the instructions. */
    const double kInstructionHeight = 24;

    struct Key {
        GRectangle bounds;
        bool isWhite;
        string label;

        int stepsUp;
        StringInstrument instrument;
    };

    const Color kWhiteKeyColor  = Color::WHITE();
    const Color kBlackKeyColor  = Color::BLACK();
    const Color kActiveKeyColor = Color::GREEN();

    class GUI: public ProblemHandler {
    public:
        GUI(GWindow& window): ProblemHandler(window) {
            AudioSystem::setSampleRate(44100);

            /* Load the keys from the key file. */
            loadKeys();

            /* Calculate bounding rectangle of the keys. */
            computeBounds();

            /* Center everything. */
            recomputeCenter();

            /* Install a KeyListener. */
            window.setKeyListener([this](GEvent e) {
                if (!GThread::iAmRunningOnTheQtGuiThread()) {
                    error("Oops!");
                }

                keyEvent(e);
            });

            timer = new GTimer(1);
            timer->start();

            /* Connect to the audio system. */
            AudioSystem::play([&](double* buffer, int toRead) {
                for (int i = 0; i < toRead; i++) {
                    Sample result = 0.0;
                    for (int j = 0; j < keys.size(); j++) {
                        result += keys[j]->instrument.nextSample();
                    }
                    buffer[i] = result;
                }
            });
        }

        ~GUI() {
            AudioSystem::stop();

            /* Disable key event processing. Do this on the Qt GUI thread to prevent races;
             * after doing this, no one is going to come looking for us.
             */
            GThread::runOnQtGuiThread([this] {
                window().setKeyListener([](GEvent){});
                timer->stop();
            });
        }

        void timerFired() override {
            if (shouldRepaint) {
                shouldRepaint = false;
                requestRepaint();
            }
        }

        void repaint() override {
            window().clearCanvas();

            if (mode == Mode::ACTIVE) {
                drawKeyboard();
            } else {
                drawWaitingMessage();
            }
        }

        void keyEvent(GEvent e) {
            if (!GThread::iAmRunningOnTheQtGuiThread()) {
                error("Oops! This shouldn't happen. Contact htiek@cs.stanford.edu.");
            }

            int keyCode = e.getKeyCode();
            if (letterToKey.containsKey(keyCode)) {
                if (e.getEventType() == KEY_PRESSED) {
                    letterToKey[keyCode]->instrument.pluck();
                    active.add(letterToKey[keyCode]->label);
                    shouldRepaint = true;
                } else if (e.getEventType() == KEY_RELEASED) {
                    active.remove(letterToKey[keyCode]->label);
                    shouldRepaint = true;
                }
            }
        }

        void windowResized() override {
            recomputeCenter();
            requestRepaint();
        }

        void mousePressed(double, double) override {
            if (mode == Mode::WAITING) {
                mode = Mode::ACTIVE;
                requestRepaint();
            }
        }

    private:
        /* Reads the keyboard data file and constructs notes from it. */
        void loadKeys() {
            ifstream input(kKeyboardFile);
            if (!input) error("Cannot open keyboard file for reading.");

            /* All the fields we read. */
            string keyCh;
            int stepsUp;
            double x, y, width, height;
            bool isWhite;

            while (input >> keyCh >> stepsUp >> x >> y >> width >> height >> boolalpha >> isWhite) {
                /* Convert from number of steps up from C to a note frequency. */
                double frequency = 440.0 * pow(2.0, (3 + stepsUp) / 12.0);

                auto key = make_shared<Key>(Key{{x, y, width, height}, isWhite, keyCh, stepsUp, StringInstrument(frequency)});
                keys += key;
                letterToKey[keyCh[0]] = key;
            }
        }

        /* Calculates the bounding box of the keyboard, used for centering things. */
        void computeBounds() {
            keyboardWidth  = 0;
            keyboardHeight = 0;

            for (auto key: keys) {
                keyboardWidth  = max(keyboardWidth,  key->bounds.x + key->bounds.width);
                keyboardHeight = max(keyboardHeight, key->bounds.y + key->bounds.height);
            }
        }

        /* Centers the keyboard within the window. */
        void recomputeCenter() {
            /* First, figure out where the instructions go. They're flush at the
             * bottom of the screen.
             */
            instructionBounds = {
                0, window().getCanvasHeight() - kInstructionHeight,
                window().getCanvasWidth(),
                kInstructionHeight
            };

            /* The remaining area is for the keyboard. */
            auto bounds = canvasBounds();
            bounds.height -= kInstructionHeight;

            baseX = (bounds.width  - keyboardWidth)  / 2.0;
            baseY = (bounds.height - keyboardHeight) / 2.0;
        }

        /* Renders the actual keyboard. */
        void drawKeyboard() {
            for (const auto& key: keys) {
                Color color;
                if (active.contains(key->label)) {
                    color = kActiveKeyColor;
                } else if (key->isWhite) {
                    color = kWhiteKeyColor;
                } else {
                    color = kBlackKeyColor;
                }

                auto bounds = key->bounds;
                bounds.x += baseX;
                bounds.y += baseY;

                window().setColor(color.toRGB());
                window().fillRect(bounds);
                window().setColor(Color::BLACK().toRGB());
                window().drawRect(bounds);

                auto render = TextRender::construct(key->label, bounds, kKeyFont);
                render->alignCenterHorizontally();
                render->alignBottom();
                render->draw(window());
            }

            /* Draw instructions. */
            auto render = TextRender::construct("Use your keyboard to play the keys!", instructionBounds, kInstructionFont);
            render->alignCenterHorizontally();
            render->alignCenterVertically();
            render->draw(window());
        }

        void drawWaitingMessage() {
            auto render = TextRender::construct("Click here to start the keyboard.", canvasBounds(), kKeyFont);
            render->alignCenterVertically();
            render->alignCenterHorizontally();
            render->draw(window());
        }

        /* Raw keyboard dimensions. */
        double keyboardWidth, keyboardHeight;

        /* Base X and Y coordinates for drawing the keyboard. */
        double baseX, baseY;

        /* Space reserved for instructions label. */
        GRectangle instructionBounds;

        /* The keys themselves. */
        Vector<shared_ptr<Key>> keys;

        /* We need to track whether to repaint in response to the key listener (Qt GUI thread)
         * but the main thread handles repaints. This does the communication between them.
         */
        bool shouldRepaint = false;

        /* Map from keyboard letters to keys to play. */
        Map<char, shared_ptr<Key>> letterToKey;

        /* Which keys are currently pressed. */
        Set<string> active;

        GTimer* timer;

        enum class Mode {
            WAITING, ACTIVE
        } mode = Mode::WAITING;
    };
}

GRAPHICS_HANDLER("StringInstrument", GWindow& window) {
    return make_shared<GUI>(window);
}
