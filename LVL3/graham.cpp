// used graham algorithm from geeks4geeks : https://www.geeksforgeeks.org/dsa/convex-hull-using-graham-scan/

#include <iostream>
#include <string>
#include <vector> 
#include <algorithm>
#include <cmath>
using namespace std;

// for better readability
typedef pair<float, float> Point;

int cross(const Point& O, const Point& A, const Point& B);
vector<Point> convex_hull_graham(vector<Point> points);
float calculate_area(const vector<Point>& hull);

int main(){
    while(1){
        
    }
	int num_of_points = 0;
	cout << "Please enter the number of points" << endl;
	cin >> num_of_points;

	vector<Point> points; // points storage as (X,Y)
	points.reserve(num_of_points); 
	
	for (int i = 0; i < num_of_points; i++)
	{
		cout << i <<": <X,Y> = ";
		float x,y;
		string line;
		cin >> line;
		size_t comma_pos = line.find(",");
		if (comma_pos == string::npos){
			cerr << "Invalid input format , Could not find ',' in line number " << i+1 << endl;
		}
		x = stof(line.substr(0,comma_pos));
		y = stof(line.substr(comma_pos+1));
		points.push_back({x,y});
	}

	// TEST:
	for (int i = 0; i < num_of_points; i++)
	{
		cout << "Point [" << i+1 << "]: "<< points[i].first << "," << points[i].second << endl;
	}

	// Calculate convex hull
    vector<Point> hull = convex_hull_graham(points);

	// Calculate and print area
    float area = calculate_area(hull);
    cout << "\nConvex Hull Area: " << area << endl;

	return 0;
}


int cross(const Point& O, const Point& A, const Point& B) {
    return (A.first - O.first) * (B.second - O.second) -
           (A.second - O.second) * (B.first - O.first);
}

// calculates the convex points using Graham Scan
vector<Point> convex_hull_graham(vector<Point> points) {
    if (points.size() <= 3) return points;

    Point pivot = *min_element(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.second < b.second || (a.second == b.second && a.first < b.first);
    });

    sort(points.begin(), points.end(), [&pivot](const Point& a, const Point& b) {
        int c = cross(pivot, a, b);
        if (c == 0)
            return hypot(pivot.first - a.first, pivot.second - a.second)
                 < hypot(pivot.first - b.first, pivot.second - b.second);
        return c > 0;
    });

    vector<Point> hull;
    for (const auto& p : points) {
        while (hull.size() >= 2 && cross(hull[hull.size()-2], hull[hull.size()-1], p) <= 0)
            hull.pop_back();
        hull.push_back(p);
    }
    return hull;
}

// Calculate area using shoelace formula
float calculate_area(const vector<Point>& hull) {
    int n = hull.size();
    if (n < 3) return 0.0;
    
    float area = 0.0;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += hull[i].first * hull[j].second;
        area -= hull[j].first * hull[i].second;
    }
    
    return abs(area) / 2.0;
}