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
    const float PASSAGE_WIDTH = 1000.f; 
    const float GAP = 60.f;

    sf::RenderWindow window;
    sf::Font font;
    sf::Text passageText;
    sf::Text inputText;
    sf::Text startButton;
    sf::Text accuracyText;
    sf::Text playAgainButton; 

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

    sf::RectangleShape startButtonBg;
    sf::RectangleShape playAgainButtonBg; 
    sf::VertexArray background; 

    sf::RectangleShape cursor;
    sf::Clock cursorBlinkClock;
    bool cursorVisible = true;

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
        accuracyText(),
        playAgainButton()
    {
        if (!font.loadFromFile("Montserrat-Regular.ttf")) { 
            std::cerr << "Failed to load font. Falling back to Arial.\n";
            if (!font.loadFromFile("arial.ttf")) {
                std::cerr << "Failed to load Arial font. Program will exit.\n";
                exit(1);
            }
        }

        startButtonBg.setSize(sf::Vector2f(180, 60)); 
        startButtonBg.setFillColor(sf::Color(50, 50, 100)); 
        startButtonBg.setOutlineThickness(2);
        startButtonBg.setOutlineColor(sf::Color(100, 100, 200));
        startButtonBg.setOrigin(startButtonBg.getLocalBounds().width / 2, startButtonBg.getLocalBounds().height / 2); 

        playAgainButtonBg = startButtonBg; 
        playAgainButtonBg.setFillColor(sf::Color(50, 100, 50));
        playAgainButtonBg.setOutlineColor(sf::Color(100, 200, 100));

        background.setPrimitiveType(sf::Quads);
        background.resize(4);
        background[0].position = sf::Vector2f(0, 0);
        background[1].position = sf::Vector2f(SCREEN_WIDTH, 0);
        background[2].position = sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT);
        background[3].position = sf::Vector2f(0, SCREEN_HEIGHT);
        background[0].color = sf::Color(25, 25, 50); 
        background[1].color = sf::Color(45, 45, 80); 
        background[2].color = sf::Color(15, 15, 30); 
        background[3].color = sf::Color(25, 25, 50); 

        passageText.setFont(font);
        inputText.setFont(font);
        startButton.setFont(font);
        accuracyText.setFont(font);
        playAgainButton.setFont(font);

        passageText.setCharacterSize(22); 
        passageText.setFillColor(sf::Color(200, 200, 255)); 

        inputText.setCharacterSize(22);
        inputText.setFillColor(sf::Color(120, 255, 120)); 

        startButton.setString("Start Test"); 
        startButton.setCharacterSize(28); 
        startButton.setFillColor(sf::Color::White); 

        playAgainButton.setString("Play Again");
        playAgainButton.setCharacterSize(28);
        playAgainButton.setFillColor(sf::Color::White);

        accuracyText.setCharacterSize(18); 
        accuracyText.setFillColor(sf::Color(100, 255, 255)); 

        startButtonBg.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT - 80.f);
        centerText(startButton, startButtonBg.getPosition().y - startButton.getLocalBounds().height / 2); 

        cursor.setSize(sf::Vector2f(2, inputText.getCharacterSize() * 1.2f));
        cursor.setFillColor(sf::Color::Yellow);
    }

    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    if (!testStarted && startButtonBg.getGlobalBounds().contains(mousePos)) {
                        startTest();
                    }
                    else if (!testStarted && playAgainButtonBg.getGlobalBounds().contains(mousePos)) { 
                        startTest(); 
                    }
                }
                else if (testStarted && event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\b' && !input.empty()) { 
                        input.pop_back();
                        if (totalCharsTyped > 0) totalCharsTyped--; 
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

            if (!testStarted) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (startButtonBg.getGlobalBounds().contains(mousePos)) {
                    startButtonBg.setFillColor(sf::Color(70, 70, 120));
                    startButtonBg.setOutlineColor(sf::Color(120, 120, 255));
                }
                else {
                    startButtonBg.setFillColor(sf::Color(50, 50, 100)); 
                    startButtonBg.setOutlineColor(sf::Color(100, 100, 200));
                }

                if (!testStarted && passageText.getString() != "Your typing speed: 0 WPM\nAccuracy: 0.00%\n" && playAgainButtonBg.getGlobalBounds().contains(mousePos)) { // Only show hover if test ended
                    playAgainButtonBg.setFillColor(sf::Color(70, 120, 70));
                    playAgainButtonBg.setOutlineColor(sf::Color(120, 255, 120));
                }
                else {
                    playAgainButtonBg.setFillColor(sf::Color(50, 100, 50));
                    playAgainButtonBg.setOutlineColor(sf::Color(100, 200, 100));
                }
            }


            window.clear(sf::Color::Black);
            window.draw(background); 

            if (testStarted) {
                window.draw(passageText);
                window.draw(inputText);
                window.draw(accuracyText);

                if (cursorBlinkClock.getElapsedTime().asMilliseconds() > 500) {
                    cursorVisible = !cursorVisible;
                    cursorBlinkClock.restart();
                }
                if (cursorVisible) {
                    sf::FloatRect inputBounds = inputText.getLocalBounds();
                    sf::Vector2f cursorRenderPos;
                    if (!input.empty()) {
                        cursorRenderPos = inputText.findCharacterPos(input.length() - 1); 
                        cursorRenderPos.x += inputText.getCharacterSize() * 0.5f; 
                    }
                    else {
                        cursorRenderPos = inputText.findCharacterPos(0); 
                    }
                    cursor.setPosition(cursorRenderPos.x, cursorRenderPos.y);
                    window.draw(cursor);
                }
            }
            else {
                window.draw(startButtonBg);
                window.draw(startButton);

                if (passageText.getString().find("Your typing speed:") != std::string::npos) {
                    window.draw(passageText);
                    window.draw(playAgainButtonBg);
                    window.draw(playAgainButton);
                }
                else {
					// *********For later**********...
                }
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
        std::string wrapped = wrapText(passage, font, passageText.getCharacterSize(), PASSAGE_WIDTH);
        passageText.setString(wrapped);
        centerText(passageText, SCREEN_HEIGHT / 4.f);

        input.clear();
        inputText.setString("");
        inputText.setFillColor(sf::Color(120, 255, 120)); 

        sf::FloatRect passageBounds = passageText.getLocalBounds();
        float inputY = passageText.getPosition().y + passageBounds.height + GAP;
        std::string wrappedInput = wrapText(input, font, inputText.getCharacterSize(), PASSAGE_WIDTH);
        inputText.setString(wrappedInput);
        centerText(inputText, inputY);

        startTime = std::chrono::system_clock::now();
        correctChars = 0;
        totalCharsTyped = 0;
        updateAccuracy();
        cursorBlinkClock.restart(); 
        cursorVisible = true;
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
                break;
            }
        }

        if (input.length() > passage.length() || !allCorrect) {
            inputText.setFillColor(sf::Color(255, 100, 100));
        }
        else {
            inputText.setFillColor(sf::Color(120, 255, 120)); 
        }

        std::string wrappedInput = wrapText(input, font, inputText.getCharacterSize(), PASSAGE_WIDTH);
        inputText.setString(wrappedInput);

        sf::FloatRect passageBounds = passageText.getLocalBounds();
        float inputY = passageText.getPosition().y + passageBounds.height + GAP;
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
            oss << "Accuracy: 100.00%"; 
        }
        accuracyText.setString(oss.str());
        centerText(accuracyText, 20.f); 
    }

    void endTest() {
        testStarted = false;
        endTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;
        double seconds = elapsed.count();

        if (seconds < 0.001) seconds = 0.001;

        int wordCount = static_cast<int>(correctChars / 5.0);
        double wpm = (wordCount / seconds) * 60;
        double accuracy = 0.0;
        if (totalCharsTyped > 0) {
            accuracy = static_cast<double>(correctChars) / totalCharsTyped * 100.0;
        }

        std::ostringstream oss;
        oss << "Your typing speed: " << static_cast<int>(wpm) << " WPM\n";
        oss << "Accuracy: " << std::fixed << std::setprecision(2) << accuracy << "%\n\n";

        if (wpm < 30 && accuracy < 85) {
            oss << "Keep practicing, you'll get there!";
        }
        else if (wpm >= 30 && wpm < 50 && accuracy >= 85) {
            oss << "You are an Average Typist. Good job!";
        }
        else if (wpm >= 50 && wpm < 70 && accuracy >= 90) {
            oss << "You are a Good Typist! Keep it up!";
        }
        else if (wpm >= 70 && wpm < 90 && accuracy >= 95) {
            oss << "You are an Excellent Typist! Impressive!";
        }
        else if (wpm >= 90 && accuracy == 100) {
            oss << "You are a Master Typist! Flawless!";
        }
        else {
            oss << "Nice typing!"; 
        }

        passageText.setString(oss.str());
        passageText.setFillColor(sf::Color(255, 255, 150)); 
        passageText.setCharacterSize(24); 
        centerText(passageText, SCREEN_HEIGHT / 2.f - passageText.getLocalBounds().height / 2); 

        inputText.setString(""); 
        accuracyText.setString(""); 

        playAgainButtonBg.setPosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT - 80.f);
        centerText(playAgainButton, playAgainButtonBg.getPosition().y - playAgainButton.getLocalBounds().height / 2);
    }
};

int main() {
    MainFrame app;
    app.run();
    return 0;
}