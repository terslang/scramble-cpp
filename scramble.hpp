// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

namespace Scramble {

namespace {

class TransitionData {
  public:
    std::string from;
    std::string to;
    int start;
    int end;
    std::string cr;
};

const std::string CLEARLINE = "\33[2K\r";
const std::string chars = "!<>-_\\/[]{}|=+*^?#()______";
std::string curText = "";
std::vector<TransitionData> queue;
int frame = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrrc(0, chars.length());
std::uniform_int_distribution<> distr40(0, 40 + 1);
std::uniform_int_distribution<> distr99(0, 100);

inline std::string randomChar() { return chars.substr(distrrc(gen), 1); }

inline void displayString(std::string newText) {
    if (newText.length() == 0) {
        return;
    }
    std::string oldText = curText;

    std::cout << CLEARLINE << newText
              << std::flush; // clears current line and prints new text

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    curText = newText;
}

inline void update() {
    std::string output = "";
    int complete = 0;
    for (int i = 0, n = queue.size(); i < n; i++) {
        auto transitionData = queue[i];

        if (frame >= transitionData.end) {
            complete++;
            output += transitionData.to;
        } else if (frame >= transitionData.start) {
            if (transitionData.cr == "" || distr99(gen) < chars.length()) {
                std::string ranChar = randomChar();
                queue[i].cr = ranChar;
            }
            output += queue[i].cr;
        } else {
            output += transitionData.from;
        }
    }

    displayString(output);
    if (complete == queue.size()) {
        return;
    } else {
        frame++;
        update();
    }
}

inline void setText(std::string newText) {
    std::string oldText = curText;
    int length = std::max(oldText.length(), newText.length());
    queue.clear();
    for (int i = 0; i < length; i++) {
        TransitionData transitionData;
        transitionData.from = i < oldText.length() ? oldText.substr(i, 1) : "";
        transitionData.to = i < newText.length() ? newText.substr(i, 1) : "";
        transitionData.start = distr40(gen);
        transitionData.end = distr40(gen);
        transitionData.cr = "";
        queue.push_back(transitionData);
    }
    frame = 0;
    update();
}

inline bool hasWhitespace(const std::string &str) {
    for (char ch : str) {
        if (ch != ' ' && std::isspace(ch)) {
            return true;
        }
    }
    return false;
}

} // namespace

inline void scramble(std::vector<std::string> texts, bool clearAll = true) {
    for (auto text : texts) {
        if (hasWhitespace(text)) {
            std::cerr
                << "String: \"" << text
                << "\" contains a white space. No white spaces other than "
                   "space ' ' are allowed."
                << std::endl;
            std::exit(1);
        }
    }

    for (auto text : texts) {
        setText(text);
        std::fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (clearAll)
            std::cout << CLEARLINE;
    }
}

} // namespace Scramble
