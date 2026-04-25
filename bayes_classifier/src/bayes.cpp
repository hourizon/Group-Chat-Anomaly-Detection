#include "bayes.hpp"

namespace bayes {

// --- BayesModel Implementation ---
void BayesModel::train(const std::string& doc, const std::string& category) {
    std::stringstream ss(doc);
    std::string word;
    if (category == "spam") {
        total_spam_docs_++;
        while (ss >> word) {
            spam_word_counts_[word]++;
            total_spam_words_++;
        }
    } else {
        total_ham_docs_++;
        while (ss >> word) {
            ham_word_counts_[word]++;
            total_ham_words_++;
        }
    }
}

// --- Classifier Implementation ---
Classifier::Classifier(const BayesModel& model) : model_(&model) {
    int total_docs = model_->total_spam_docs_ + model_->total_ham_docs_;
    if (total_docs > 0) {
        prior_spam_ = static_cast<double>(model_->total_spam_docs_) / total_docs;
        prior_ham_ = static_cast<double>(model_->total_ham_docs_) / total_docs;
    }
}

double Classifier::calc_prob(const std::string& doc, bool is_spam) const {
    double log_prob = is_spam ? std::log(prior_spam_) : std::log(prior_ham_);
    const auto& word_counts = is_spam ? model_->spam_word_counts_ : model_->ham_word_counts_;
    int total_words = is_spam ? model_->total_spam_words_ : model_->total_ham_words_;
    
    std::stringstream ss(doc);
    std::string word;
    while (ss >> word) {
        int count = 0;
        auto it = word_counts.find(word);
        if (it != word_counts.end()) {
            count = it->second;
        }
        // 拉普拉斯平滑
        log_prob += std::log(static_cast<double>(count + 1) / (total_words + word_counts.size()));
    }
    return log_prob;
}

double Classifier::classify(const std::string& doc) const {
    if (!model_) return 0.5;
    double log_prob_spam = calc_prob(doc, true);
    double log_prob_ham = calc_prob(doc, false);

    // 为了防止数值下溢，比较log概率
    // P(Spam|Doc) = 1 / (1 + exp(logP(Ham|Doc) - logP(Spam|Doc)))
    double spam_prob = 1.0 / (1.0 + std::exp(log_prob_ham - log_prob_spam));
    return spam_prob;
}

// --- Trainer Implementation ---
void Trainer::train(const std::string& filename, const std::string& category) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开训练文件: " + filename);
    }
    std::string line;
    while (std::getline(file, line)) {
        model_.train(line, category);
    }
}

Classifier Trainer::getClassifier() const {
    return Classifier(model_);
}

} // namespace bayes
