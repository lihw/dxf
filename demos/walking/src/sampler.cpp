// SphereSampler.cpp
//
// Hongwei Li lihw81@gmail.com
//

#include "sampler.h"

#include <stdint.h>
#include <float.h>
#include <stdlib.h>
#include <algorithm>


const static uint32_t capacity = 256;
const static float PI = 3.1415926535898f;

//
// Sphere sampler
//

//
// Helper functions 
//
static float dot(const Sample &sample, const Point &point)
{
    float d = sample.x * point.x + sample.y * point.y + sample.z * point.z;
    d = std::min(d, 1.0f);
    return acosf(d);
}

static float dot(const Sample &sample1, const Sample &sample2)
{
    float d = sample1.x * sample2.x + sample1.y * sample2.y + sample1.z * sample2.z;
    d = std::min(d, 1.0f);
    return acosf(d);
}


static uint32_t randomUint32()
{
    return rand(); 
}

static float randomFloat()
{
    return (float)rand() / (float)RAND_MAX;
}

//
// Help structs
//
struct HeapElem 
{
    float energy;
    int   pindex;
    
    // The smallest energy element goes to the root
    bool operator<(const HeapElem& other) 
    {
        return energy > other.energy; 
    };
};



//
// class SphereSampler
//
SphereSampler::SphereSampler(int num)
{
    m_points.resize(num * capacity);
    m_samples.resize(num);
}

SphereSampler::~SphereSampler()
{
}

void SphereSampler::sample()
{
    float energy;

    float bestEnergy = initialize();

    int iter = 0;
    while (!optimize(energy)) 
    {
        if (bestEnergy == energy) 
            break;

        fprintf(stderr, "Iterations: %d, energy: %f\n", iter++, energy);
        fflush(stderr);

        bestEnergy = energy;
    }
}

float SphereSampler::initialize()
{
    srand(10001);

    // Distribute points on the sphere uniformly
    size_t numPoints = m_points.size();
    for (size_t i = 0; i < numPoints; ++i)
    {
        m_points[i].y = randomFloat() * 2.0f - 1.0f;
        float l = sqrtf(1.0f - m_points[i].y * m_points[i].y);
        float phi = randomFloat() * 2.0f * PI;
        m_points[i].z = l * cosf(phi);
        m_points[i].x = l * sinf(phi);

        // Assign point to a sample
        size_t s = i / capacity;
        m_samples[s].points.push_back(m_points[i]);
    }

    float energy = 0;
    // Compute the energy and maxRadius of each sample.
    size_t numSamples = m_samples.size();
    for (size_t i = 0; i < numSamples; ++i)
    {
        uint32_t j = randomUint32() % capacity;

        m_samples[i].x = m_samples[i].points[j].x;
        m_samples[i].y = m_samples[i].points[j].y;
        m_samples[i].z = m_samples[i].points[j].z;

        m_samples[i].energy = 0;
        m_samples[i].maxRadius = 0;
        m_samples[i].stable = false;

        for (size_t j = 0; j < capacity; ++j)
        {
            float r = dot(m_samples[i], m_samples[i].points[j]);
            m_samples[i].energy += r * r;
            m_samples[i].maxRadius = std::max(m_samples[i].maxRadius, r);
        }

        energy += m_samples[i].energy;
    }

    return energy;
}

void SphereSampler::update(size_t i)
{
    float x = 0;
    float y = 0;
    float z = 0;
    m_samples[i].energy = 0;

    for (int p = 0; p < capacity; p++) 
    {
        x += m_samples[i].points[p].x;
        y += m_samples[i].points[p].y;
        z += m_samples[i].points[p].z;
    }

    x /= (float)capacity;
    y /= (float)capacity;
    z /= (float)capacity;

    m_samples[i].x = x;
    m_samples[i].y = y;
    m_samples[i].z = z;
    
    for (int p = 0; p < capacity; p++) 
    {
        float r = dot(m_samples[i], m_samples[i].points[p]);
        m_samples[i].energy += r * r;
        m_samples[i].maxRadius = std::max(m_samples[i].maxRadius, r);
    }
}


bool SphereSampler::optimize(float &energy)
{
    size_t numSamples = m_samples.size();

    std::vector<bool> stable(numSamples, true);

    for (size_t i = 0; i < numSamples - 1; i++) 
    {
        for (size_t j = i + 1; j < numSamples; j++) 
        {
            // when either of two site is stable, it
            // is no need to do point swapping
            if (m_samples[i].stable && m_samples[j].stable)
            {
                continue;
            }

            // if the bounding circle of two sites are not
            // overlappoing, they don't need point swapping
            float d = dot(m_samples[i], m_samples[j]);
            if (d > m_samples[i].maxRadius + m_samples[j].maxRadius)
            {
                continue;
            }

            std::vector<HeapElem> heapi;
            std::vector<HeapElem> heapj;

            for (size_t p = 0; p < capacity; ++p) 
            {
                // Distance to paired site
                float d1 = dot(m_samples[j], m_samples[i].points[p]);
                float d2 = dot(m_samples[i], m_samples[i].points[p]);

                float energy = d1 * d1 - d2 * d2;
                
                HeapElem elem;
                elem.energy = energy;
                elem.pindex = p;

                heapi.push_back(elem);
            }

            if (heapi.empty())
            {
                continue;
            }
            std::make_heap(heapi.begin(), heapi.end());

            for (size_t p = 0; p < capacity; p++) 
            {
                float d1 = dot(m_samples[i], m_samples[j].points[p]);
                float d2 = dot(m_samples[j], m_samples[j].points[p]);

                float energy = d1 * d1 - d2 * d2;
                if (energy > -heapi.front().energy) 
                {
                    continue;
                }
                
                HeapElem elem;
                elem.energy = energy;
                elem.pindex = p;

                heapj.push_back(elem);
            }

            if (heapj.empty())
            {
                continue;
            }
            std::make_heap(heapj.begin(), heapj.end());

            size_t maxHeapSize = std::min(heapi.size(), heapj.size());
            size_t swap;
            for (swap = 0; swap < maxHeapSize; swap++) 
            {
                HeapElem &hei = heapi.front();
                HeapElem &hej = heapj.front();

                if (hei.energy + hej.energy > 0)
                {
                    break;
                }
        
                std::swap(m_samples[i].points[hei.pindex], 
                          m_samples[j].points[hej.pindex]);
        
                std::pop_heap(heapi.begin(), heapi.end() - swap);
                std::pop_heap(heapj.begin(), heapj.end() - swap);
            }
            
            if (swap > 0) 
            {
                update(i);
                update(j);

                stable[i] = false;
                stable[j] = false;
            }
        }
    }
    
    bool ret = true;
    energy = 0;
    for (size_t s = 0; s < numSamples; s++) 
    {
        m_samples[s].stable = stable[s];
        ret &= stable[s];
        
        energy += m_samples[s].energy;
    }

    return ret;
}

//
// Ring sampler
//

RingSampler::RingSampler(int num)
{
    m_samples.resize(num);
}

RingSampler::~RingSampler()
{
}

void RingSampler::sample()
{
    srand(1001);

    // First distribute the samples on the ring evenly.
    float deltaAngle = 2.0f * PI / (float)m_samples.size();
    float angleNoiseRange = deltaAngle * 0.1f;

    float angle = randomFloat() * 2.0f * PI;
    
    for (int i = 0; i < m_samples.size(); ++i)
    {
        float thisAngle = angle + angleNoiseRange * (randomFloat() * 2.0f - 1.0f);

        Sample sample;
        sample.x = cosf(thisAngle);
        sample.y = sinf(thisAngle);

        m_samples[i] = sample;

        angle += deltaAngle;
    }
}

