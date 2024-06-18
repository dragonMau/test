#include <vector>
#include <cmath>
#include <cstdint>

#include <iostream>

struct ReturnDataRequest {
    /*
        if 0, then not save.
        if 1, then save all.
        if 2+, then save each n-th.
    */
    uint8_t power;
    uint8_t position;
    uint8_t velocity;
};

struct ReturnData {
    std::vector<double> power;
    std::vector<double> position;
    std::vector<double> velocity;

    uint64_t power_i = 0;
    uint64_t position_i = 0;
    uint64_t velocity_i = 0;
};

class Oscillator {
public:
    Oscillator(double freq, double l_torq = 0.0, double s_torq = 100.0, uint16_t sample_rate = 44100, uint8_t iters = 1) 
        : freq(freq), l_torq(l_torq), s_torq(s_torq), sample_rate(sample_rate), iters(iters) {
        osc_k = std::pow(2 * M_PI * freq, 2);
        sample_time = 1.0 / sample_rate;
        iter_time = sample_time / iters;
        power_p = 0;
        power_n = 0;
        pos_x = 0;
        vel_x = 0;
        prev_v = 0;
    }

    void step(double force = 0) {
        stepOscillator(force);
    }

    double getPower() const {
        return 100000 * (power_p - power_n);
    }

    void setFreq(double new_freq) {
        freq = new_freq;
        osc_k = std::pow(2 * M_PI * freq, 2);
    }

    void setSampleRate(uint16_t new_sr) {
        sample_rate = new_sr;
        sample_time = 1.0 / sample_rate;
        iter_time = sample_time / iters;
    }

    void setIters(uint8_t new_iters) {
        iters = new_iters;
        iter_time = sample_time / iters;
    }

    void setLTorq(double new_l_torq) {
        l_torq = new_l_torq;
    }

    void setSTorq(double new_s_torq) {
        s_torq = new_s_torq;
    }

    ReturnData runOscillator(const std::vector<double>& force, const ReturnDataRequest& request) {
        uint64_t length = force.size();
        uint64_t pow_len = (request.power) ? length / request.power : 0;
        uint64_t pos_len = (request.position) ? length / request.position : 0;
        uint64_t vel_len = (request.velocity) ? length / request.velocity : 0;

        ReturnData return_data;
        return_data.power.resize(pow_len);
        return_data.position.resize(pos_len);
        return_data.velocity.resize(vel_len);

        for (uint64_t i = 0; i < length; ++i) {
            double tick_force = force[i];
            stepOscillator(tick_force);

            if (request.power && !(i % request.power)) {
                return_data.power[return_data.power_i++] = getPower();
            }
            if (request.position && !(i % request.position)) {
                return_data.position[return_data.position_i++] = pos_x;
            }
            if (request.velocity && !(i % request.velocity)) {
                return_data.velocity[return_data.velocity_i++] = vel_x;
            }
        }
        return return_data;
    }

private:
    double freq;
    double osc_k;
    double l_torq;
    double s_torq;
    uint16_t sample_rate;
    double sample_time;
    uint8_t iters;
    double iter_time;
    double power_p;
    double power_n;
    double pos_x;
    double vel_x;
    double prev_v;

    void stepOscillator(double force = 0) {
        for (uint8_t i = 0; i < iters; ++i) {
            double acc = -pos_x * osc_k - vel_x * s_torq + force;
            vel_x += acc * iter_time;
            pos_x += vel_x * iter_time;

            if (vel_x < 0) {
                if (-vel_x <= l_torq) {
                    vel_x = 0;
                } else {
                    vel_x += l_torq;
                }
                if (prev_v >= 0) {
                    power_p = pos_x;
                }
            }
            if (vel_x > 0) {
                if (vel_x <= l_torq) {
                    vel_x = 0;
                } else {
                    vel_x -= l_torq;
                }
                if (prev_v <= 0) {
                    power_n = pos_x;
                }
            }
            prev_v = vel_x;
        }
    }
};