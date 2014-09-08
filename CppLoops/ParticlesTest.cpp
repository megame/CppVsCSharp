#include "pch.h"
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
		vec.push_back(make_unique<Particle>(i, name, 0, (float) rand(), (float) rand()));
	}

	return vec;
}

void ProcessParticleFrame(vector<unique_ptr<Particle>> & particles)
{
	auto count = particles.size();

	for (decltype(count) i = 0; i < count; i++)
	{
		float effectX = 0, effectY = 0;

		for (decltype(count) j = 0; j < count; j++)
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







vector<Particle> GenerateParticleVector()
{
	vector<Particle> vec;

	for (int i = 0; i < NumberOfParticles; i++)
	{
		string name = "This is a particle number ";
		name += i;
		vec.push_back(Particle(i, name, 0, (float) rand(), (float) rand()));
	}

	return vec;
}

void ProcessParticleVectorFrame(vector<Particle> & particles)
{
	auto count = particles.size();

	for (decltype(count) i = 0; i < count; i++)
	{
		float effectX = 0, effectY = 0;

		for (decltype(count) j = 0; j < count; j++)
		{
			float dX = particles[i].vector().x - particles[j].vector().x;
			float dY = particles[i].vector().y - particles[j].vector().y;

			float magnitude = sqrtf(dX*dX + dY*dY);

			float factor = 6.672e-11f / (magnitude * magnitude * magnitude);

			effectX += dX * factor;
			effectY += dX * factor;
		}

		particles[i].vector().x += effectX;
		particles[i].vector().y += effectY;
	}
}

void ParticlesTest::VectorTest()
{
	auto particles = GenerateParticleVector();

	for (int iter = 0; iter < NumberOfIterations; iter++)
		ProcessParticleVectorFrame(particles);
}














void ProcessParticleVectorFrame2(vector<Particle> & particles)
{
	auto count = particles.size();

	auto xs = unique_ptr<float>(new float[count]);
	auto ys = unique_ptr<float>(new float[count]);

	auto arrX = xs.get();
	auto arrY = ys.get();

	for (decltype(count) i = 0; i < count; i++)
	{
		arrX[i] = particles[i].vector().x;
		arrY[i] = particles[i].vector().y;
	}

	for (decltype(count) i = 0; i < count; i++)
	{
		float effectX = 0, effectY = 0;

		for (decltype(count) j = 0; j < count; j++)
		{
			float dX = arrX[i] - arrX[j];
			float dY = arrY[i] - arrY[j];

			float magnitude = sqrtf(dX*dX + dY*dY);

			float factor = 6.672e-11f / (magnitude * magnitude * magnitude);

			effectX += dX * factor;
			effectY += dX * factor;
		}

		arrX[i] += effectX;
		arrY[i] += effectY;
	}

	for (decltype(count) i = 0; i < count; i++)
	{
		particles[i].vector().x = arrX[i];
		particles[i].vector().y = arrY[i];
	}
}

void ParticlesTest::VectorTest2()
{
	auto particles = GenerateParticleVector();

	for (int iter = 0; iter < NumberOfIterations; iter++)
		ProcessParticleVectorFrame2(particles);
}