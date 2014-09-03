#include "stdafx.h"
#include "ParticlesTest.h"
#include "Particle.h"

using namespace std;

const int NumberOfParticles = 10240;
const int NumberOfIterations = 5;

vector<unique_ptr<Particle>> GenerateParticles()
{
	vector<unique_ptr<Particle>> vec;

	for (int i = 0; i < NumberOfParticles; i++)
	{
		string name = "This is a particle number ";
		name += i;
		vec.push_back(make_unique<Particle>(i, name, 0, (float)rand(), (float)rand()));
	}

	return vec;
}

void ProcessParticleFrame(vector<unique_ptr<Particle>> & particles)
{
	auto count = particles.size();

	for (int i = 0; i < count; i++)
	{
		float effectX = 0, effectY = 0;

		for (int j = 0; j < count; j++)
		{
			float dX = particles[i]->vector().x - particles[j]->vector().x;
			float dY = particles[i]->vector().y - particles[j]->vector().y;

			float magnitude = sqrtf(dX*dX + dY*dY);

			float factor = 6.672e-11f / (magnitude * magnitude * magnitude);

			effectX += dX * factor;
			effectY += dX * factor;
		}

		particles[i]->vector().x += effectX;
		particles[i]->vector().y += effectY;
	}
}

void ParticlesTest::Test()
{
	auto particles = GenerateParticles();

	for (int iter = 0; iter < NumberOfIterations; iter++)
		ProcessParticleFrame(particles);
}
















void ProcessParticleFrame2(vector<unique_ptr<Particle>> & particles)
{
	auto count = particles.size();

	auto xs = unique_ptr<float>(new float[count]);
	auto ys = unique_ptr<float>(new float[count]);

	for (int i = 0; i < count; i++)
	{
		xs.get()[i] = particles[i]->vector().x;
		ys.get()[i] = particles[i]->vector().y;
	}

	for (int i = 0; i < count; i++)
	{
		float effectX = 0, effectY = 0;

		for (int j = 0; j < count; j++)
		{
			float dX = xs.get()[i] - xs.get()[j];
			float dY = ys.get()[i] - ys.get()[j];

			float magnitude = sqrtf(dX*dX + dY*dY);

			float factor = 6.672e-11f / (magnitude * magnitude * magnitude);

			effectX += dX * factor;
			effectY += dX * factor;
		}

		xs.get()[i] += effectX;
		ys.get()[i] += effectY;
	}

	for (int i = 0; i < count; i++)
	{
		particles[i]->vector().x = xs.get()[i];
		particles[i]->vector().y = ys.get()[i];
	}
}