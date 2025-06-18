// used graham algorithm from geeks4geeks : https://www.geeksforgeeks.org/dsa/convex-hull-using-graham-scan/

#include <iostream>
#include <string>
#include <vector> 
#include <algorithm>
#include <cmath>
#include <utility>
#include <sstream>

using namespace std;

// for better readability
typedef pair<float, float> Point;
vector<Point> points; // points storage as (X,Y)


int cross(const Point& O, const Point& A, const Point& B);
vector<Point> convex_hull_graham(vector<Point> points);
float calculate_area(const vector<Point>& hull);



Point parse_point(const string& s){
    size_t comma_pos = s.find(",");
    if (comma_pos == string::npos){
        cerr << "Invalid input format , Could not find ',' " << endl;
    }
    float x = stof(s.substr(0,comma_pos));
    float y = stof(s.substr(comma_pos+1));
    return {x,y};
    //we will have to use pushback into the vector array
}

int main(){
    string line;
    cout << "Please Enter a command:" << endl;
    while(getline(cin, line)){
        stringstream ss(line);
        string cmd;
        Point point;
        ss >> cmd;
        // ifelse kaele veharim
        if (cmd == "Newgraph"){
            int number_of_points;
            ss >> number_of_points;
            points.clear();

            cout << "Please enter the coordinates of the Points:" << endl;
            for (int i = 0; i < number_of_points; i++)
            {
                string xy;
	            cin >> xy;
                Point p = parse_point(xy);
                points.push_back(p);
            }
             cout << "Please Enter a command:" << endl;
            continue;

        }
        else if (cmd == "CH"){
            if (points.size() == 0){
                throw runtime_error("No graph exists");
            }
            // Calculate convex hull
            vector<Point> hull = convex_hull_graham(points);

            // Calculate and print area
            float area = calculate_area(hull);
            cout << "\nConvex Hull Area: " << area << endl;
             cout << "Please Enter a command:" << endl;
        }
        else if (cmd == "Newpoint"){
            
            string coordiantes;
            ss >> coordiantes;
            Point p = parse_point(coordiantes);
            // check for duplicates
            auto it = find(points.begin(),points.end(),p);
            if (it == points.end()){
                points.push_back(p);
            }else{
                cout <<"Point is already exists!" << endl;
            }
        cout << "Please Enter a command:" << endl;
        }
        else if(cmd == "Removepoint"){

            string rm_coordiantes;
            ss >> rm_coordiantes;
            Point p = parse_point(rm_coordiantes);
            auto it = find(points.begin(),points.end(),p);
            if (it != points.end()){
                points.erase(it);
            }
            else
                cout << "Could not Remove a non-existing Point" << endl;
         cout << "Please Enter a command:" << endl;
        }
        else if (cmd != "Removepoint" || cmd != "Newpoint" || cmd != "Newgraph" || cmd != "CH" ){
            throw runtime_error("Wrong command");
        }
    }

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