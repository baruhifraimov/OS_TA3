#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netdb.h>
#include <vector> 
#include <algorithm>
#include <cmath>
#include <utility>
#include <sstream>
#include <iostream>

#define PORT "9034"
#define MAXDATASIZE 256

using namespace std;

// for better readability
typedef pair<float, float> Point;
vector<Point> points; // points storage as (X,Y)

// Convert vector<Point> to string
string serialize_points(const vector<Point>& points) {
    stringstream ss;
    ss << "Newgraph "; // Include size for validation
    for (const auto& p : points) {
        ss << ";" << p.first << "," << p.second;
    }
    return ss.str();
}

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

// Connects to server and returns the socket descriptor
int connect_to_server(const std::string& hostname, const std::string& port) {
    struct addrinfo hints{}, *servinfo, *p;
    int sockfd;

    hints.ai_family = AF_UNSPEC;       // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;   // TCP

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &servinfo) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    for (p = servinfo; p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
            continue;

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == nullptr) {
        std::cerr << "client: failed to connect\n";
        exit(2);
    }

    freeaddrinfo(servinfo);
    return sockfd;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " hostname\n";
        return 1;
    }

    int sockfd = connect_to_server(argv[1], PORT);
    std::cout << "Connected to server. Type messages and press Enter.\n";

    fd_set master;
    std::string input;
    char buf[MAXDATASIZE];

    for (;;) {
        FD_ZERO(&master);
        FD_SET(STDIN_FILENO, &master);  // user input
        FD_SET(sockfd, &master);        // server messages

        int maxfd = std::max(sockfd, STDIN_FILENO);

        if (select(maxfd + 1, &master, nullptr, nullptr, nullptr) == -1) {
            perror("select");
            exit(3);
        }

        // User input
        if (FD_ISSET(STDIN_FILENO, &master)) {

            std::getline(std::cin, input);

            if (std::cin.eof()) {
                std::cout << "Input closed. Exiting.\n";
                break;
            }

            stringstream ss(input);
            string cmd;
            Point point;
            ss >> cmd;
            // ifelse
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
                string message = serialize_points(points);
                // need to change to points as the message
                if (send(sockfd, message.c_str(), message.length(), 0) == -1) {
                perror("send");
                }
                cout << "Graph has been created successfuly." << endl;
            }
            else{

            if (send(sockfd, input.c_str(), input.length(), 0) == -1) {
                perror("send");
                }
            }
        }

        // Incoming message from server
        if (FD_ISSET(sockfd, &master)) {
            int nbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
            if (nbytes <= 0) {
                if (nbytes == 0)
                    std::cout << "Server closed the connection.\n";
                else
                    perror("recv");
                break;
            } else {
                buf[nbytes] = '\0';
                std::cout << "Message: " << buf << std::endl;
            }
        }
    }

    close(sockfd);
    return 0;
}
