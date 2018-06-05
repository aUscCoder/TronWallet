#include <string>

#include "base/containers/flat_map.h"
#include "base/containers/flat_set.h"

const char characters[] = "abcdefghijklmnopqrstuvwxyz";

base::flat_map<size_t, base::flat_set<std::string>> map_word_len_associations;

std::string gen_word(size_t length) {
  auto iter = map_word_len_associations.find(length);
  if (iter != map_word_len_associations.end() && iter->second.size() > 1000) {
    auto iter2 = iter->second.begin();
    std::advance(iter2, (rand() % iter->second.size()));
    return *iter2;
  }
  std::string word;
  word.reserve(length);
  for (size_t i = 0; i < length; i++) {
    word.append(1, characters[rand() % 26]);
  }
  word[0] = word[0] + 26; // Capital.
  map_word_len_associations[length].insert(word);

  return word;
}

int main() {
  base::flat_map<std::string, base::flat_set<std::string>> map_word_associations;
  for (size_t i = 0; i < 1000; i++) {
    base::flat_set<std::string> assoc;
    assoc.reserve(1000);
    for (size_t j = 0; j < 1000; j++) {
      assoc.insert(gen_word(j));
    }
    map_word_associations.insert(std::make_pair(gen_word(i), assoc));
  }
}