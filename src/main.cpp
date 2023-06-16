#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <cmath>
#include <utility>
#include <vector>
#include <set>
#include <optional>
#include <cstring>
#include <sstream>

#define ZERO_LOG 10
#define DEFAULT_MIN_INDEX 5;

namespace {
    double zero_log(size_t n) noexcept {
        return n == 0 ? ZERO_LOG : std::log(n);
    };

    std::string strip_word(const std::string &word) noexcept {
        std::string stripped_word(".");
        for (char c: word) {
            if (std::isalpha(c)) {
                stripped_word.push_back(static_cast<char>(std::tolower(c)));
            }
        }
        stripped_word.push_back('.');
        if (stripped_word.size() == 2) return "";
        return stripped_word;
    }

    struct trigram {
        std::string ab, bc, abc;

        trigram(std::string &&ab, std::string &&bc, std::string &&abc) noexcept:
                ab(ab), bc(bc), abc(abc) {}
    };

    std::vector<trigram> get_trigrams(const std::string &word) noexcept {
        std::vector<trigram> trigrams;
        for (size_t i = 0; i < word.size() - 2; i++) {
            trigrams.emplace_back(
                    word.substr(i, 2),
                    word.substr(i + 1, 2),
                    word.substr(i, 3));
        }
        return trigrams;
    }

    class Word {
    public:
        std::string original;
        std::string stripped;

        explicit Word(std::string &original) noexcept {
            this->stripped = strip_word(original);
            this->original = std::move(original);
        }

        explicit Word(std::string &&original = "") noexcept {
            this->stripped = strip_word(original);
            this->original = std::move(original);
        }

        friend std::istream &operator>>(std::istream &is, Word &word) {
            is >> word.original;
            word.stripped = strip_word(word.original);
            return is;
        }

        bool operator<(const Word &right) const noexcept {
            return stripped < right.stripped;
        }
    };

    class Spellchecker {
    public:
        std::map<double, std::set<Word>> data;

        explicit Spellchecker(std::istream &is, double min_index, size_t words_keep_size) :
                words_keep_size(words_keep_size), min_index(min_index) {
            count_n_grams(is);
            is.clear();
            is.seekg(0, std::istream::beg);
            fill_word_list(is);
        }

    private:
        std::unordered_map<std::string, size_t> trigrams_count;
        std::unordered_map<std::string, size_t> digrams_count;
        size_t words_keep_size;
        size_t words_current_size = 0;
        double min_index;

        void count_n_grams(std::istream &is) {
            std::string word;
            while (is >> word) {
                std::string stripped = strip_word(word);
                if (stripped.empty()) continue;
                std::vector<trigram> trigrams = get_trigrams(stripped);
                for (trigram &i: trigrams) {
                    trigrams_count[i.abc]++;
                    digrams_count[i.ab]++;
                    digrams_count[i.bc]++;
                }
            }
        }

        void fill_word_list(std::istream &is) {
            Word word;
            while (is >> word) {
                if (word.stripped.empty()) continue;
                insert(std::move(word));
            }
        }

        double calc_word_index(const std::string &word) noexcept {
            double sum = 0;
            std::vector<trigram> trigrams = get_trigrams(word);
            for (const trigram &tg: trigrams) {
                double tg_index = calc_trigram_index(tg);
                sum += tg_index * tg_index;
            }
            return std::sqrt(sum / static_cast<double>(word.size() - 2));
        }

        double calc_trigram_index(const trigram &tg) noexcept {
            double ab = zero_log(digrams_count[tg.ab]);
            double bc = zero_log(digrams_count[tg.bc]);
            double abc = zero_log(trigrams_count[tg.abc]);
            return (ab + bc) / 2 - abc;
        }

        void insert(Word &&word) noexcept {
            double index = calc_word_index(word.stripped);
            if (index > min_index) {
                if (words_current_size < words_keep_size) {
                    if (data[index].insert(word).second) {
                        words_current_size++;
                    }
                } else {
                    auto least = data.begin();
                    if (index > least->first) {
                        if (data[index].insert(word).second) {
                            least->second.erase(least->second.begin());
                            if (least->second.empty()) {
                                data.erase(least);
                            }
                        }
                    }
                }
            }
        }
    };

    struct Args {
        std::string filename;
        double min_index = DEFAULT_MIN_INDEX;
        size_t max_words_number = SIZE_MAX;

        static std::optional<Args> parse(int argc, char *argv[]) {
            Args args;
            if (argc < 2) return std::nullopt;
            args.filename = argv[1];
            for (int i = 2; i < argc; ++i) {
                if (std::strcmp(argv[i], "-i") == 0) {
                    if (++i >= argc) return std::nullopt;
                    std::stringstream str(argv[i]);
                    str >> args.min_index;
                    if (str.bad()) return std::nullopt;
                } else if (std::strcmp(argv[i], "-w") == 0) {
                    if (++i >= argc) return std::nullopt;
                    std::stringstream str(argv[i]);
                    str >> args.max_words_number;
                    if (str.bad()) return std::nullopt;
                } else {
                    return std::nullopt;
                }
            }
            return args;
        }
    };
}

int main(int argc, char *argv[]) {
    std::optional<Args> args = Args::parse(argc, argv);
    if (!args.has_value()) {
        std::cerr << "Wrong input, specify file as first argument and use correct options" << std::endl;
        return 1;
    }
    std::fstream file(args->filename);
    if (!file.is_open()) {
        perror(("Error occurred while opening file " + std::string(argv[1])).c_str());
        return 1;
    }
    Spellchecker sp(file, args->min_index, args->max_words_number);
    size_t i = 0;
    for (auto &element: sp.data) {
        for (auto &word: element.second) {

            std::cout << ++i << ") " << word.original << ' ' << element.first << '\n';
        }
    }
    return 0;
}