#ifndef SUBJECT_MGR_H
#define SUBJECT_MGR_H

#include <map>
#include <string>
#include "obs_interface.h"

class SubjectManager {
public:
    void addSubject(Subject *subject) {
        subjects[subject->getName()] = subject;
    }

    Subject *getSubject(const std::string &name) {
        auto it = subjects.find(name);
        if (it != subjects.end()) {
            return it->second;
        }
        return nullptr;
    }

    bool unregisterSubject(const std::string &name) {
        auto it = subjects.find(name);
        if (it != subjects.end()) {
            subjects.erase(it);
            return true;
        }
        return false;
    }

    void listSubjects() {
        for (auto &[name, subject] : subjects) {
            std::cout << name << std::endl;
        }
    }



private:
    std::map<std::string, Subject *> subjects;
};

class LANinfo : public Subject {
    public:
    LANinfo() : Subject("LANinfo") {}
    void registerObserver(Observer *observer) override {
        observers.push_back(observer);
        std::cout << "observer num: " << observers.size() << std::endl;
    }
    void unregisterObserver(Observer *observer) override {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }
    void notifyObservers(const std::string &message) override {
        for (auto &observer : observers) {
            observer->update(message);
        }
    }

    void getObserverNums()
    {
        std::cout << "Observer nums: " << observers.size() << std::endl;
    }

    void transportMessage(Observer *observer, const std::string &message)
    {
        std::cout << "count dot" << std::endl;
        for (auto &obs : observers)
        {
            std::cout << "count dot" << std::endl;
            if (obs != observer)
            {
                obs->update(message);
            }
        }
    }





    // private:
    std::vector<Observer*> observers;

};

#endif // SUBJECT_MGR_H





