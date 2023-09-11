#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <set>
#include <regex>

class StringProcessor {
public:
    std::set<std::string> modules;

    std::vector<std::string> tokenize(const std::string& str) {
        std::vector<std::string> tokens;
        std::istringstream iss(str);
        std::string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        return tokens;
    }

    void detectAndAddModule(const std::string& letter, const std::string& moduleName) {
        if (letter.find(moduleName) != std::string::npos) {
            modules.insert(moduleName);
        }
    }

    std::regex pattern{ R"(\w+\s*\([^)]*\)\s*)" };

    void processAndPrint(const std::string& segment) {
        static const std::vector<std::string> moduleNames = {
            "sin", "cos", "abs", "round", "smoothstep", "fract", "length", "mix"
        };

        for (const auto& moduleName : moduleNames) {
            detectAndAddModule(segment, moduleName);
        }

        std::vector<std::string> words = tokenize(segment);

        std::smatch match;
        std::regex pattern1(R"(\w+\()");
        std::regex pattern2(R"(\)[ \t]*$)");
        std::regex pattern3(R"(\{)");

        for (std::size_t i = 0; i + 2 < words.size(); ++i) {
            if (std::regex_search(words[i], match, pattern1) &&
                std::regex_search(words[i + 1], match, pattern2) &&
                std::regex_search(words[i + 2], match, pattern3)) {
                return;
            }
        }

        if (words.size() < 2) return;

        if (words[0] == "const" && words.size() >= 5) {
            std::string keyword = (words[1] == "float" || words[1] == "int") ? "number" : "";
            std::cout << words[0] << " " << words[2] << " : " << keyword << " " << words[3] << " " << words[4] << '\n';
        }
        else if (words[0] == "float" && words.size() >= 4) {
            std::string keyword = "number";
            std::string end = words[2] + " " + words[3];
            for (std::size_t i = 4; i < words.size(); ++i) {
                end += " " + words[i];
            }
            std::cout << "local " << words[1] << " : " << keyword << " " << end << '\n';
        }
        else if (words[0] == "vec2" && segment.find("=") != std::string::npos) {
            std::string end = words[2] + " " + words[3];
            for (std::size_t i = 4; i < words.size(); ++i) {
                end += " " + words[i];
            }
            std::cout << "local " << words[1] << " : " << words[0] << " " << end << '\n';
        }
    }
};

int main() {
    std::ifstream file("script.txt");

    if (!file.is_open()) {
        std::cerr << "Failed to open the file.\n";
        return 1;
    }

    StringProcessor processor;
    std::string line;

    while (std::getline(file, line)) {
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));

        if (line.substr(0, 2) == "//") continue;

        std::istringstream iss(line);
        std::string segment;

        while (std::getline(iss, segment, ';')) {
            processor.processAndPrint(segment);
        }
    }

    return 0;
}
