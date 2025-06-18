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

using namespace std;

#define PORT "9034"   // port we're listening on

// for better readability
typedef pair<float, float> Point;
vector<Point> points; // points storage as (X,Y)
bool is_graph; // to know if we already has a graph


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
            // Calculate convex hull
            vector<Point> hull = convex_hull_graham(*points);

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
            auto it = find(points->begin(),points->end(),p);
            if (it == points->end()){
                points->push_back(p);
            }else{
                cout <<"Point is already exists!" << endl;
            }
        cout << "Please Enter a command:" << endl;
        }
        else if(cmd == "Removepoint"){

            string rm_coordiantes;
            ss >> rm_coordiantes;
            Point p = parse_point(rm_coordiantes);
            auto it = find(points->begin(),points->end(),p);
            if (it != points->end()){
                points->erase(it);
            }
            else
                cout << "Could not Remove a non-existing Point" << endl;
         cout << "Please Enter a command:" << endl;
        }
        else if (cmd != "Removepoint" || cmd != "Newpoint" || cmd != "Newgraph" || cmd != "CH" ){
                cout <<"Wrong command!" << endl;
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

/*
 * Add new incoming connections to the proper sets
 */
void handle_new_connection(int listener, fd_set *master, int *fdmax)
{
    socklen_t addrlen;
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    char remoteIP[INET6_ADDRSTRLEN];

    addrlen = sizeof remoteaddr;
    newfd = accept(listener,
        (struct sockaddr *)&remoteaddr,
        &addrlen);

    if (newfd == -1) {
        perror("accept");
    } else {
        FD_SET(newfd, master); // add to master set
        if (newfd > *fdmax) {  // keep track of the max
            *fdmax = newfd;
        }
        printf("selectserver: new connection from %s on "
            "socket %d\n",
            inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP),
            newfd);
    }
}

/*
 * Broadcast a message to all clients
 */
void broadcast(char *buf, int nbytes, int listener, int s,
               fd_set *master, int fdmax)
{
    for(int j = 0; j <= fdmax; j++) {
        // send to everyone!
        if (FD_ISSET(j, master)) {
            // except the listener and ourselves
            if (j != listener && j != s) {
                if (send(j, buf, nbytes, 0) == -1) {
                    perror("send");
                }
            }
        }
    }
}

/*
 * Handle client data and hangups
 */
void handle_client_data(int s, int listener, fd_set *master,
                        int fdmax)
{
    char buf[256];    // buffer for client data
    int nbytes;

    // handle data from a client
    if ((nbytes = recv(s, buf, sizeof buf, 0)) <= 0) {
        // got error or connection closed by client
        if (nbytes == 0) {
            // connection closed
            printf("selectserver: socket %d hung up\n", s);
        } else {
            perror("recv");
        }
        close(s); // bye!
        FD_CLR(s, master); // remove from master set
    } else {
        // we got some data from a client
        printf("message recieved from %d\n",s);
        handlemessage(buf,&points);
        broadcast(buf, nbytes, listener, s, master, fdmax);
    }
}

/*
 * Main
 */
int main(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    listener = get_listener_socket();

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data
        // to read
        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener)
                    handle_new_connection(i, &master, &fdmax);
                else
                    handle_client_data(i, listener, &master, fdmax);
            }
        }
    }

    return 0;
}