#include "bayes.hpp"
#include <iostream>
#include <iomanip>
#include <filesystem>

using namespace bayes;
using namespace std::filesystem;

void createSampleData(const path& data_dir) {
    if (!exists(data_dir)) {
        std::cout << "Creating data directory..." << std::endl;
        create_directory(data_dir);
    }
    path spam_path = data_dir / "spam.txt";
    path ham_path = data_dir / "ham.txt";

    std::ofstream spam(spam_path);
    spam << "buy now limited offer free money cash prize winner click here\n";
    spam << "congratulations you won free gift act now special discount 50% off\n";
    spam << "make money fast easy work from home business opportunity\n";
    spam << "cheap pills online pharmacy prescription free delivery\n";
    spam << "adult content webcam nude girls click now free access\n";
    spam << "lottery winner claim your prize money transfer now\n";
    spam << "investment opportunity high return zero risk double your money\n";
    spam << "urgent action required verify your account password now\n";
    spam.close();

    std::ofstream ham(ham_path);
    ham << "hello how are you doing today\n";
    ham << "meeting scheduled for tomorrow at 3pm\n";
    ham << "please review the attached document\n";
    ham << "thanks for your help with the project\n";
    ham << "can we schedule a call next week\n";
    ham << "the report is ready for review\n";
    ham << "happy birthday wishes to you\n";
    ham << "see you at the conference tomorrow\n";
    ham.close();

    std::cout << "Sample data created." << std::endl;
}

int main(int argc, char* argv[]) {
    path data_dir = "data";

    std::cout << "=== Naive Bayes Classifier Test ===" << std::endl;

    createSampleData(data_dir);

    std::cout << "\n[1] Training model..." << std::endl;
    Trainer trainer;
    trainer.train((data_dir / "spam.txt").string(), "spam");
    trainer.train((data_dir / "ham.txt").string(), "ham");
    std::cout << "Training completed." << std::endl;

    std::cout << "\n[2] Creating classifier..." << std::endl;
    Classifier classifier = trainer.getClassifier();
    std::cout << "Classifier created." << std::endl;

    std::cout << "\n[3] Testing classification..." << std::endl;

    std::vector<std::string> test_messages = {
        "click here for free money",
        "buy now cheap pills online",
        "hello how are you",
        "meeting tomorrow at 3pm",
        "urgent verify your account password"
    };

    std::cout << std::fixed << std::setprecision(4);
    for (const auto& msg : test_messages) {
        double prob = classifier.classify(msg);
        std::cout << "  \"" << msg << "\"" << std::endl;
        std::cout << "    -> Spam probability: " << prob;
        std::cout << " [" << (prob > 0.95 ? "SPAM" : "HAM") << "]" << std::endl;
    }

    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}