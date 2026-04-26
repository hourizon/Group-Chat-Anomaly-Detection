# Makefile for Anomaly Detection Engine

CXX = g++
# Base flags for C++17, warnings, and local include path
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# --- Core Algorithm Targets (Guaranteed to build) ---
TARGET_CLASSIFIER = bayes_classifier
SRC_CLASSIFIER = src/main.cpp src/bayes.cpp

TARGET_PROFILER = test_profiler
SRC_PROFILER = src/test_profiler.cpp src/profiler.cpp

# --- Experimental Bot Target (Requires specific compiler/libs) ---
# NOTE: The bot_main target is disabled by default due to compiler compatibility
# issues with third-party libraries (httplib, curl) on some MinGW-w64 versions.
# To attempt building it, run "make bot".
TARGET_BOT = bot_main
SRC_BOT = src/bot_main.cpp src/bayes.cpp src/profiler.cpp src/integration.cpp
BOT_LIBS = -lcurl -lpthread
# On some systems, you might need to specify library paths explicitly:
# BOT_LDFLAGS = -LC:/msys64/mingw64/lib
# And add $(BOT_LDFLAGS) to the bot rule.

# Default target: build only the stable components
all: $(TARGET_CLASSIFIER) $(TARGET_PROFILER)

$(TARGET_CLASSIFIER): $(SRC_CLASSIFIER)
	$(CXX) $(CXXFLAGS) -o $(TARGET_CLASSIFIER) $(SRC_CLASSIFIER)

$(TARGET_PROFILER): $(SRC_PROFILER)
	$(CXX) $(CXXFLAGS) -o $(TARGET_PROFILER) $(SRC_PROFILER)

# Experimental target for the bot
bot: $(SRC_BOT)
	@echo "--- Building experimental bot target. This may fail on your system. ---"
	$(CXX) $(CXXFLAGS) -D_WIN32_WINNT=0x0A00 -o $(TARGET_BOT) $(SRC_BOT) $(BOT_LIBS)

clean:
	rm -f $(TARGET_CLASSIFIER) $(TARGET_PROFILER) $(TARGET_BOT) data/*.dat

run-classifier:
	./$(TARGET_CLASSIFIER)

run-profiler:
	./$(TARGET_PROFILER)

.PHONY: all bot clean run-classifier run-profiler
