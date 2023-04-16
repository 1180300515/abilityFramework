#ifndef OBS_INTERFACE_H
#define OBS_INTERFACE_H

class Observer {
    public:
        Observer() = default;
        virtual ~Observer() = default;
        virtual void update() = 0;
};

class Subject {
public:
    Subject() = default;
    virtual ~Subject() = default;
    virtual void registerObserver(Observer* observer) = 0;
    virtual void removeObserver(Observer* observer) = 0;
    virtual void notifyObservers() = 0;
};

#endif // OBS_INTERFACE_H