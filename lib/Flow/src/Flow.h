#ifndef FLOW_H
#define FLOW_H

class FlowSensor {
private:
    int lastIndex;
    double flowArray[50];

public:
    void init();
    double getFlow();
};

#endif /* FLOW_H */