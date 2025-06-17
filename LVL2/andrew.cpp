// used andrew algorithm from wiki : https://en.wikibooks.org/wiki/Algorithm_Implementation/Geometry/Convex_hull/Monotone_chain

#include <iostream>
#include <string>
#include <vector> 
#include <algorithm>
using namespace std;

// for better readability
typedef pair<float, float> Point;

int cross(const Point& O, const Point& A, const Point& B);
vector<Point> convex_hull_monotone(vector<Point> points);
float calculate_area(const vector<Point>& hull);

int main(){
	//int num_of_points = 0;
	// cout << "Please enter the number of points" << endl;
	// cin >> num_of_points;

	vector<Point> points; // points storage as (X,Y)
	//points.reserve(num_of_points); 
	
	// for (int i = 0; i < num_of_points; i++)
	// {
	// 	cout << i <<": <X,Y> = ";
	// 	float x,y;
	// 	string line;
	// 	cin >> line;
	// 	size_t comma_pos = line.find(",");
	// 	// if we didnt found the comma ',' then print the error and throw
	// 	if (comma_pos == string::npos){
	// 		cerr << "Invalid input format , Could not find ',' in line number" << i+1 << endl;
	// 	}
	// 	x = stof(line.substr(0,comma_pos));
	// 	y = stof(line.substr(comma_pos+1));
	// 	points.push_back({x,y});
	// }

	// // TEST:
	// for (int i = 0; i < num_of_points; i++)
	// {
	// 	cout << "Point [" << i+1 << "]: "<< points[i].first << "," << points[i].second << endl;
	// }

    #include <cstdlib>
    points.reserve(1'000'000);
    for (int i = 0; i < 1'000'000; ++i)
    points.push_back({rand()%10000, rand()%10000});

	// Calculate convex hull
    vector<Point> hull = convex_hull_monotone(points);

	// Calculate and print area
    float area = calculate_area(hull);
    cout << "\nConvex Hull Area: " << area << endl;

	return 0;
}


int cross(const Point& O, const Point& A, const Point& B) {
    return (A.first - O.first) * (B.second - O.second) -
           (A.second - O.second) * (B.first - O.first);
}

// calculates the convex points
vector<Point> convex_hull_monotone(vector<Point> points) {
    int n = points.size(), k = 0;
    if (n <= 3) return points;

    sort(points.begin(), points.end());

    vector<Point> hull(2 * n);

    // Lower hull
    for (int i = 0; i < n; ++i) {
        while (k >= 2 && cross(hull[k-2], hull[k-1], points[i]) <= 0)
            k--;
        hull[k++] = points[i];
    }

    // Upper hull
    for (int i = n - 2, t = k + 1; i >= 0; --i) {
        while (k >= t && cross(hull[k-2], hull[k-1], points[i]) <= 0)
            k--;
        hull[k++] = points[i];
    }

    hull.resize(k - 1); // remove duplicate endpoint
    return hull;
}

// Calculate area using shoelace formula
float calculate_area(const vector<Point>& hull) {
    int n = hull.size();
    if (n < 3) return 0.0; // Need at least 3 points for area
    
    float area = 0.0;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n; // Next point (wrap around)
        area += hull[i].first * hull[j].second;
        area -= hull[j].first * hull[i].second;
    }
    
    return abs(area) / 2.0; // Take absolute value and divide by 2
}
	

