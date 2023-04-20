#ifndef OBS_INTERFACE_H
#define OBS_INTERFACE_H

#include <string>
#include <iostream>
#include <vector>

class Observer
{
public:
    ~Observer() = default;
    void update(const std::string &message)
    {
        std::cout << "Observer received message: " << message << std::endl;
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
    Subject(const std::string &name) : name(name) {}
    virtual ~Subject() = default;

    const std::string &getName() const
    {
        return name;
    }

    virtual void registerObserver(Observer *observer) = 0;
    virtual void unregisterObserver(Observer *observer) = 0;
    virtual void notifyObservers(const std::string &message) = 0;

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
    std::string name;
    std::vector<Observer*> observers;
};

#endif // OBS_INTERFACE_H