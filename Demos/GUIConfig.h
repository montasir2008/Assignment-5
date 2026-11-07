RUN_TESTS_MENU_OPTION()

WINDOW_TITLE("Tone Matrix")

MENU_ORDER("BallDemoGUI.cpp", "KeyboardGUI.cpp", "ToneMatrixGUI.cpp")

TEST_ORDER("DebuggerWarmup.cpp", "StringInstrument.cpp", "ToneMatrix.cpp")

TEST_BARRIER("KeyboardGUI.cpp",   "StringInstrument.cpp")
TEST_BARRIER("ToneMatrixGUI.cpp", "ToneMatrix.cpp")
