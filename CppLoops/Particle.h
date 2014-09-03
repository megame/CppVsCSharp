#pragma once

struct Vector
{
	float x, y;
};

struct Particle
{
public:
	Particle(int id, std::string name, int type, float x, float y)
	{
		_id = id;
		_name = name;
		_type = type;
		_vec = { x, y };
	}

public:
	int _id;
	std::string _name;
	int _type;
	Vector _vec;
public:
	int id() const { return _id; }
	Vector & vector() { return _vec; }
};

