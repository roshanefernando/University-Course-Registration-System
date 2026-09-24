#include "User.h"
#include "FalseLoginAttemptException.h"

int User::nextId = 1000;

User::User(const std::string& username, const std::string& password)
    : username(username), password(password) {
    id = "U" + std::to_string(nextId++);
}

User::User(const std::string& id, const std::string& username, const std::string& password, bool)
    : id(id), username(username), password(password) {
    if (id.size() > 1 && id[0] == 'U') {
        try {
            int n = std::stoi(id.substr(1));
            if (n >= nextId) nextId = n + 1;
        } catch (const std::exception&) {  }
    }
}

std::string User::getId() const { return id; }
std::string User::getUsername() const { return username; }
std::string User::getPassword() const { return password; }
void User::setUsername(const std::string& newUsername) { username = newUsername; }

void User::login(const std::string& pw) const {
    if (pw != password) {
        throw FalseLoginAttemptException(username);
    }
}
