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

    std::map<std::string, Subject *> subjects;
};

class LANinfo : public Subject {
    public:
    
    std::vector<Observer *> observers = std::vector<Observer *>();
    LANinfo() : Subject("LANinfo", observers), observers() {}
    void registerObserver(Observer *observer) override {
        // std::cout << "Registering observer address: " << observer << std::endl;
        observers.push_back(observer);
        // std::cout << "observer num: " << observers.size() << std::endl;
        // std::cout << "observer addresses are: " << std::endl;
        // for (auto &obs : observers) {
        //     std::cout << obs << std::endl;
        // }
    }
    
    void transportMessage(Observer *observer, const std::string &message) override
    {
        // std::cout << "param in Observer address : " << observer <<  std::endl;
        for (auto &obs : observers)
        {
            // std::cout << "count dot 2" << std::endl;
            // std::cout << "obs: " << obs << std::endl;
            // std::cout << "observer: " << observer << std::endl;
            if (obs != observer)
            {
                // std::cout << "obs not null" << std::endl;
                obs->update(message);
            }
        }
    }

    void getObserverNums() override
    {
        std::cout << "Observer nums: " << observers.size() << std::endl;
    }

    void unregisterObserver(Observer *observer) override {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }
    void notifyObservers(const std::string &message) override {
        for (auto &observer : Subject::observers) {
            observer->update(message);
        }
    }
    // // private:
};

#endif // SUBJECT_MGR_H





