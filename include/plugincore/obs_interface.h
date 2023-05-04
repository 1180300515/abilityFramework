#ifndef OBS_INTERFACE_H
#define OBS_INTERFACE_H

#include <string>
#include <iostream>
#include <vector>
#include "utils/color.h"

class Observer
{
public:
    ~Observer() = default;
    void update(const std::string &message)
    {
        std::cout << L_RED << "received message: " << message << NONE << std::endl;
    }
    std::vector<std::string> getSubjectNames() const
    {
        return subjectNames;
    }
    void addSubjectName(const std::string &name)
    {
        subjectNames.push_back(name);
    }

private:
    std::vector<std::string> subjectNames;
};

class Subject
{
public:
    Subject(const std::string &name, std::vector<Observer*>& observers) : name(name), observers(observers) {}
    virtual ~Subject() = default;

    const std::string &getName() const
    {
        return name;
    }

    virtual void registerObserver(Observer *observer) = 0;
    virtual void unregisterObserver(Observer *observer) = 0;
    virtual void notifyObservers(const std::string &message) = 0;

    virtual void getObserverNums() = 0;
    // {
    //     std::cout << "Observer nums: " << observers.size() << std::endl;
    // }

    virtual void transportMessage(Observer *observer, const std::string &message) = 0;
    // {
    //     std::cout << "count dot" << std::endl;
    //     for (auto &obs : observers)
    //     {
    //         std::cout << "count dot 2" << std::endl;
    //         std::cout << "obs: " << obs << std::endl;
    //         std::cout << "observer: " << observer << std::endl;
    //         if (obs != observer)
    //         {
    //             std::cout << "obs not null" << std::endl;
    //             obs->update(message);
    //         }
    //     }
    // }


// private:
    std::string name;
    std::vector<Observer*> observers;
};

#endif // OBS_INTERFACE_H