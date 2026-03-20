

struct StepperConfig {
    uint stepPin, dirPin, diagPin, enPin;
    float stepsPerMm;            // after microstepping (e.g. 16×200 = 3200 steps/rev → stepsPerMm)
};

class TMC2209Driver {
public:
    TMC2209Driver(const StepperConfig& cfg);
    void enable(bool on);
    void setDirection(bool positive);
    void stepOnce();                     // single pulse (min pulse width respected)
    bool isDiagActive() const;           // stall / fault
    int32_t getPosition() const { return position; }
    void setPosition(int32_t pos) { position = pos; }

private:
    StepperConfig cfg;
    int32_t position = 0;
    absolute_time_t lastStepTime = {0};
};