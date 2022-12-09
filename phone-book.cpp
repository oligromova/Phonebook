#include "phone-book.h"

bool phone_book_t::create_user(const std::string &number, const std::string &name) {
  if (was[number])
    return false;

  was[number] = true;
  find_user[number] = user_info_t{user_t{number, name}, 0};

  users.insert(user_info_t{user_t{number, name}, 0});

  for (int i = 0; i <= number.size(); ++i) {
    numbers[number.substr(0, i)].insert(user_info_t{user_t{number, name}, 0});
  }
  return true;
}

bool phone_book_t::add_call(const call_t &call) {
  if (!was[call.number])
    return false;

  user_info_t cur = find_user[call.number];

  history.push_back(call);
  users.erase(cur);

  for (int i = 0; i <= call.number.size(); ++i) {
    numbers[call.number.substr(0, i)].erase(cur);
  }

  cur.total_call_duration_s += call.duration_s;
  users.insert(cur);
  find_user[call.number].total_call_duration_s += call.duration_s;
  for (size_t i = 0; i <= call.number.size(); ++i) {
    numbers[call.number.substr(0, i)].insert(cur);
  }
  return true;
}

std::vector<call_t> phone_book_t::get_calls(size_t start_pos, size_t count) const {
  std::vector<call_t> answer;
  for (int i = start_pos; i < std::min(start_pos + count, history.size()); ++i) {
    answer.push_back(history[i]);
  }
  return answer;
}

std::vector<user_info_t> phone_book_t::search_users_by_number(const std::string &number_prefix, size_t count) const {
  std::vector<user_info_t> answer;
  auto iter = numbers.find(std::string(number_prefix));
  if (iter != numbers.end()) {
    if (iter->first == number_prefix) {
      const std::set<user_info_t, cmp_search_numbers>& s = iter->second;
      std::copy_n(s.begin(), std::min(s.size(), count), std::back_inserter(answer));
    }
  }
  return answer;
}

std::vector<user_info_t> phone_book_t::search_users_by_name(const std::string &name_prefix, size_t count) const {
  std::vector<user_info_t> answer;
  auto itlow = users.lower_bound(user_info_t{user_t{"", name_prefix}, 1000000});
  if (itlow != users.end()) {
    for (int i = 0; i < count && itlow != users.end(); ++i) {
      if (itlow->user.name.substr(0, name_prefix.size()) == name_prefix) {
        answer.push_back(*itlow);
        itlow.operator++();
      }
    }
  }
  return answer;
}

void phone_book_t::clear() {
  users.clear();
  numbers.clear();
  history.clear();
  was.clear();
  find_user.clear();
}

size_t phone_book_t::size() const {
  return users.size();
}

bool phone_book_t::empty() const {
  return users.empty();
}