// Sampler.h
//
// Hongwei Li lihw81@gmail.com
//

#ifndef SAMPLER_H
#define SAMPLER_H

#include <vector>

struct Point
{
    float x;
    float y;
    float z;
};

struct Sample
{
    float              x;
    float              y;
    float              z;
    float              energy;
    float              maxRadius;
    bool               stable;
    std::vector<Point> points;
};

// Sphere sampling
class SphereSampler
{
public:
    SphereSampler(int num);
    ~SphereSampler();

    void sample();
    const Sample &sample(int i) { return m_samples[i]; }

private:
    float initialize();
    bool optimize(float &energy);
    void update(size_t i);

private:
    std::vector<Point>  m_points;
    std::vector<Sample> m_samples;
};

// Jittered sampling (stochastic sampling) on a ring
class RingSampler
{
public:
    RingSampler(int num);
    ~RingSampler();

    void sample();
    const Sample &sample(int i) { return m_samples[i]; }

private:
    std::vector<Point>  m_points;
    std::vector<Sample> m_samples;
};



#endif // !SAMPLER_H
