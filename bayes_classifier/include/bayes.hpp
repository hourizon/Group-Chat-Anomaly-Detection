#pragma once

#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>

namespace bayes {

class BayesModel {
public:
    void train(const std::string& doc, const std::string& category);

private:
    std::map<std::string, int> spam_word_counts_;
    std::map<std::string, int> ham_word_counts_;
    int total_spam_words_ = 0;
    int total_ham_words_ = 0;
    int total_spam_docs_ = 0;
    int total_ham_docs_ = 0;

    friend class Classifier;
};

class Classifier {
public:
    Classifier() = default;
    Classifier(const BayesModel& model);
    double classify(const std::string& doc) const;

private:
    const BayesModel* model_ = nullptr;
    double prior_spam_ = 0.5;
    double prior_ham_ = 0.5;

    double calc_prob(const std::string& doc, bool is_spam) const;
};

class Trainer {
public:
    void train(const std::string& filename, const std::string& category);
    Classifier getClassifier() const;

private:
    BayesModel model_;
};

} // namespace bayes
