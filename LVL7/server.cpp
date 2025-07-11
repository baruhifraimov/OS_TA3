/* 
*   for chat simulation we used beej implementation https://beej.us/guide/bgnet/
*   selectserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector> 
#include <algorithm>
#include <cmath>
#include <utility>
#include <sstream>
#include <iostream>
#include <pthread.h>

using namespace std;

#define MAXDATASIZE 256
#define PORT "9034"   // port we're listening on

// for better readability
typedef pair<float, float> Point;
vector<Point> points; // points storage as (X,Y)
bool is_graph; // to know if we already has a graph

// threads
pthread_mutex_t graph_mutex = PTHREAD_MUTEX_INITIALIZER; //sets the lock to default state

float cross(const Point& O, const Point& A, const Point& B);
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

int handlemessage(string message, vector<Point>* points ){
    if(message.size() > 0){
        stringstream ss(message);
        string cmd;
        Point point;
        ss >> cmd;
        string pair_str;
        // ifelse kaele veharim
        if (cmd == "Newgraph"){
            while (getline(ss, pair_str, ';')) {
                size_t comma_pos = pair_str.find(',');
                if (comma_pos != string::npos) {
                    int x = stof(pair_str.substr(0, comma_pos));
                    int y = stof(pair_str.substr(comma_pos + 1));
                    points->emplace_back(x, y);
                }
            }
            cout << "Received " << points->size() << " points" << endl;
        }
        else if (cmd == "CH"){
            if (points->size() == 0){
                cout <<"No graph exists" << endl;
            }
            else{
                // Calculate convex hull
                vector<Point> hull = convex_hull_graham(*points);

                // Calculate and print area
                float area = calculate_area(hull);
                cout << "\nConvex Hull Area: " << area << endl;
            }
        }
        else if (cmd == "Newpoint"){
            
            string coordiantes;
            ss >> coordiantes;
            Point p = parse_point(coordiantes);
            // check for duplicates
            auto it = find(points->begin(),points->end(),p);
            if (it == points->end()){
                points->push_back(p);
                cout <<"Point added successfuly!" << endl;
            }else{
                cout <<"Point is already exists!" << endl;
            }
        }
        else if(cmd == "Removepoint"){
            if (points->size() == 0){
                cout <<"No Points to remove" << endl;
            }
            else{
                string rm_coordiantes;
                ss >> rm_coordiantes;
                Point p = parse_point(rm_coordiantes);
                auto it = find(points->begin(),points->end(),p);
                if (it != points->end()){
                    points->erase(it);
                    cout <<"Point removed successfuly!" << endl;
                }
                else
                    cout << "Could not Remove a non-existing Point" << endl;
            }
        }
        else if (cmd != "Removepoint" && cmd != "Newpoint" && cmd != "Newgraph" && cmd != "CH" ){
                cout << message << endl;
                cout <<"Wrong command!" << endl;
        }
    }

	return 0;
}


float cross(const Point& O, const Point& A, const Point& B) {
    return (A.first - O.first) * (B.second - O.second) -
           (A.second - O.second) * (B.first - O.first);
}

vector<Point> convex_hull_graham(vector<Point> input_points) {
    if (input_points.size() <= 1) return input_points;
    
    // Remove duplicates
    sort(input_points.begin(), input_points.end());
    input_points.erase(unique(input_points.begin(), input_points.end()), input_points.end());
    
    if (input_points.size() <= 2) return input_points;
    
    // Find pivot (bottommost point, leftmost if tie)
    auto pivot_it = min_element(input_points.begin(), input_points.end(), 
        [](const Point& a, const Point& b) {
            return a.second < b.second || (a.second == b.second && a.first < b.first);
        });
    
    Point pivot = *pivot_it;
    
    // Move pivot to front
    swap(*input_points.begin(), *pivot_it);
    
    // Sort remaining points by polar angle with respect to pivot
    sort(input_points.begin() + 1, input_points.end(), 
        [&pivot](const Point& a, const Point& b) {
            float c = cross(pivot, a, b);
            if (abs(c) < 1e-9) { // Handle collinear points
                // If collinear, sort by distance from pivot
                float dist_a = hypot(pivot.first - a.first, pivot.second - a.second);
                float dist_b = hypot(pivot.first - b.first, pivot.second - b.second);
                return dist_a < dist_b;
            }
            return c > 0;
        });
    
    // Build convex hull
    vector<Point> hull;
    for (const auto& p : input_points) {
        // Remove points that create right turn
        while (hull.size() >= 2 && cross(hull[hull.size()-2], hull[hull.size()-1], p) < 0) {
            hull.pop_back();
        }
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


/*
 * Convert socket to IP address string.
 * addr: struct sockaddr_in or struct sockaddr_in6
 */
const char *inet_ntop2(void *addr, char *buf, size_t size)
{
    struct sockaddr_storage *sas = static_cast<struct sockaddr_storage *>(addr);
    struct sockaddr_in *sa4;
    struct sockaddr_in6 *sa6;
    void *src;

    switch (sas->ss_family) {
        case AF_INET:
            sa4 = static_cast<struct sockaddr_in *>(addr);
            src = &(sa4->sin_addr);
            break;
        case AF_INET6:
            sa6 = static_cast<struct sockaddr_in6 *>(addr);
            src = &(sa6->sin6_addr);
            break;
        default:
            return nullptr;
    }

    return inet_ntop(sas->ss_family, src, buf, size);
}

/*
 * Return a listening socket
 */
int get_listener_socket(void)
{
    struct addrinfo hints, *ai, *p;
    int yes=1;    // for setsockopt() SO_REUSEADDR, below
    int rv;
    int listener;

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    return listener;
}

void* handle_client_thread(void* arg){ // arg is an int pointer that contains the socked_id of our client.
    int client_fd = *((int*)arg); // perform casting to int* and a dereference.
    char buffer[MAXDATASIZE]; //to receive the messages of the client.
    while(true){ //while the connection exists:
        memset(buffer,0,sizeof(buffer));
        int nbytes = recv(client_fd, buffer , sizeof(buffer),0); // waiting and blocking .
        if(nbytes <=0){ // error handleing
            if(nbytes == 0){ printf("Client on socket %d disconnected succesfully\n",client_fd);}
            else{perror("recv");
            printf("Client on socket %d disconnected with error\n", client_fd);}
            break;
        }

        buffer[nbytes] = '\0';
        printf("Message received from client %d : %s \n", client_fd,buffer);
        
        // thread lock handling , other threads wont be able to use handlemessage func while one is using.
        pthread_mutex_lock(&graph_mutex);
        handlemessage(buffer, &points);
        pthread_mutex_unlock(&graph_mutex);
    }

    close(client_fd);
    delete (int*)arg; //releasing the memory of arg.
    printf("Session ended for client: %d\n",client_fd);

    return nullptr;
}

/*
 * Main
 */
int main(void)
{
    int listener;     // listening socket descriptor
    listener = get_listener_socket();
    printf("Server is listening on port %s...\n", PORT);
    
    while (true) { // an endless loop , the sever always open for new clients.
        struct sockaddr_storage remoteaddr;
        socklen_t addrlen = sizeof(remoteaddr);
        int newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen); // wait for new connection from a client

        // error handling for acccept
        if(newfd == -1){
            perror("accept");
            continue;
        }
        
        char remoteIP[INET6_ADDRSTRLEN];
        printf("New connection from %s on socket %d\n",
        inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP),
        newfd);

        int* pclient = new int(newfd); // pointer of the client descriptor for pthread_create

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client_thread, pclient) != 0 ){
            perror("pthread_create");
            close(newfd);
            delete pclient;
            continue;
        }
        pthread_detach(thread);
    }
    
    close(listener);
    return 0;
}