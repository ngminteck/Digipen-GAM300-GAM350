#pragma once


class Curve
{
public:
	Curve();
	virtual ~Curve();

protected:
	std::vector<Vec3> _way_points;
public:
	void add_way_point(const Vec3& point);
	void clear();

protected:
	void add_node(const Vec3& node);
	virtual void _on_way_point_added()=0;

protected:
	
	std::vector<double> _distances;
public:
	std::vector<Vec3> _nodes;
	Vec3 node(int i) const { return _nodes[i]; }
	double length_from_starting_point(int i) const { return _distances[i]; }
	bool has_next_node(int i) const { return static_cast<int>(_nodes.size()) > i; }
	int node_count() const {  return static_cast<int>(_nodes.size()); }
	bool is_empty() { return _nodes.empty(); }
	double total_length() const
	{
		assert(!_distances.empty());
		return _distances[_distances.size() - 1];
	}

protected:
	int _steps;
public:
	void increment_steps(int steps) { _steps+=steps; }
	void set_steps(int steps) { _steps = steps; }
	const int get_steps() { return _steps; }
};
