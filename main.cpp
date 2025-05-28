#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

class MainFrame {
private:
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    const float PASSAGE_WIDTH = 1200.f;
    const float GAP = 60.f; 
    sf::RenderWindow window;
    sf::Font font;
    sf::Text passageText;
    sf::Text inputText;
    sf::Text startButton;
    sf::Text accuracyText;
    std::string input;
    std::string passage;
    std::vector<std::string> passages = {
        "The quick brown fox jumps over the lazy dog. This is a test passage to check your typing speed and accuracy. Keep practicing to improve your skills. Good luck!",
        "So wake me up when it's all over. When I'm wiser and I'm older. All this time I was finding myself, and I didn't know I was lost. I tried carrying the weight of the world.",
        "Never face each other. One bed, different covers. We don't care anymore. Two hearts still beating. On with different rhythms. Maybe we should let this go.",
        "We'll go down in history. Worst the world has ever seen. But wasn't if I'm tearing everything up. Oh what a mess we made. Made some toxic memories."
    };
    bool testStarted = false;
    std::chrono::time_point<std::chrono::system_clock> startTime;
    std::chrono::time_point<std::chrono::system_clock> endTime;
    int correctChars = 0;
    int totalCharsTyped = 0;

    std::string wrapText(const std::string& str, sf::Font& font, unsigned int charSize, float maxWidth) {
        std::istringstream iss(str);
        std::string word, line, result;
        sf::Text tempText("", font, charSize);

        while (iss >> word) {
            std::string testLine = line.empty() ? word : line + " " + word;
            tempText.setString(testLine);
            if (tempText.getLocalBounds().width > maxWidth) {
                if (!result.empty()) result += "\n";
                result += line;
                line = word;
            }
            else {
                line = testLine;
            }
        }
        if (!line.empty()) {
            if (!result.empty()) result += "\n";
            result += line;
        }
        return result;
    }

    void centerText(sf::Text& text, float y) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2, bounds.top);
        text.setPosition(SCREEN_WIDTH / 2.f, y);
    }

public:
    MainFrame() :
        window(sf::VideoMode({ static_cast<unsigned int>(SCREEN_WIDTH), static_cast<unsigned int>(SCREEN_HEIGHT) }), "Typing Test", sf::Style::Close),
        passageText(),
        inputText(),
        startButton(),
        accuracyText()
    {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load font\n";
            return;
        }

        passageText.setFont(font);
        inputText.setFont(font);
        startButton.setFont(font);
        accuracyText.setFont(font);

        passageText.setCharacterSize(20);
        passageText.setFillColor(sf::Color::White);

        inputText.setCharacterSize(20);

        startButton.setString("Start");
        startButton.setCharacterSize(24);
        startButton.setFillColor(sf::Color::Yellow);

        accuracyText.setCharacterSize(16);
        accuracyText.setFillColor(sf::Color::Cyan);

        // Initial centering
        centerText(startButton, SCREEN_HEIGHT - 50.f);
        centerText(accuracyText, 10.f);
    }

    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::MouseButtonPressed) {
                    if (startButton.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                        startTest();
                    }
                }
                else if (testStarted && event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\b' && !input.empty()) {
                        input.pop_back();
                    }
                    else if (event.text.unicode >= 32 && event.text.unicode <= 126) {
                        input += static_cast<char>(event.text.unicode);
                        totalCharsTyped++; 
                    }
                    updateInputText();
                    if (input.length() >= passage.length()) {
                        endTest();
                    }
                }
            }

            window.clear(sf::Color::Black);
            window.draw(passageText);
            window.draw(inputText);
            window.draw(accuracyText);
            if (!testStarted) {
                window.draw(startButton);
            }
            window.display();
        }
    }

private:
    void startTest() {
        testStarted = true;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, static_cast<int>(passages.size() - 1));
        passage = passages[dis(gen)];
        std::string wrapped = wrapText(passage, font, 20, PASSAGE_WIDTH);
        passageText.setString(wrapped);
        centerText(passageText, 60.f);

        input.clear();
        inputText.setString("");
        inputText.setFillColor(sf::Color::Green);
        float inputY = passageText.getPosition().y + passageText.getLocalBounds().height + GAP;
        std::string wrappedInput = wrapText(input, font, 20, PASSAGE_WIDTH);
        inputText.setString(wrappedInput);
        centerText(inputText, inputY);

        startTime = std::chrono::system_clock::now();
        correctChars = 0;
        totalCharsTyped = 0;
        updateAccuracy();
    }

    void updateInputText() {
        correctChars = 0;
        bool allCorrect = true;
        for (size_t i = 0; i < input.length(); ++i) {
            if (i < passage.length() && input[i] == passage[i]) {
                correctChars++;
            }
            else {
                allCorrect = false;
            }
        }
        std::string wrappedInput = wrapText(input, font, 20, PASSAGE_WIDTH);
        inputText.setString(wrappedInput);
        inputText.setFillColor(allCorrect ? sf::Color::Green : sf::Color::Red);
        float inputY = passageText.getPosition().y + passageText.getLocalBounds().height + GAP;
        centerText(inputText, inputY);
        updateAccuracy();
    }

    void updateAccuracy() {
        std::ostringstream oss;
        if (totalCharsTyped > 0) {
            double accuracy = static_cast<double>(correctChars) / totalCharsTyped * 100.0;
            oss << "Accuracy: " << std::fixed << std::setprecision(2) << accuracy << "%";
        }
        else {
            oss << "Accuracy: 0.00%";
        }
        accuracyText.setString(oss.str());
        centerText(accuracyText, 10.f);
    }

    void endTest() {
        testStarted = false;
        endTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;
        double seconds = elapsed.count();
        int wordCount = static_cast<int>(correctChars / 5.0); 
        double wpm = (wordCount / seconds) * 60;
        double accuracy = 0.0;
        if (totalCharsTyped > 0) {
            accuracy = static_cast<double>(correctChars) / totalCharsTyped * 100.0;
        }

        std::ostringstream oss;
        oss << "Your typing speed: " << static_cast<int>(wpm) << " WPM\n";
        oss << "Accuracy: " << std::fixed << std::setprecision(2) << accuracy << "%\n";
        if (wpm < 40 && accuracy > 80) {
            oss << "You are an Average Typist.";
        }
        else if (wpm < 60 && accuracy > 90) {
            oss << "You are a Good Typist!";
        }
        else if (wpm < 80 && accuracy == 100) {
            oss << "You are an Excellent Typist!";
        }
        passageText.setString(oss.str());
        centerText(passageText, SCREEN_HEIGHT / 2.f - 50.f);
        inputText.setString("");
    }
};

int main() {
    MainFrame app;
    app.run();
    return 0;
}