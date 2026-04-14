#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
    // --- AI TRAINING ---
    double w1 = 0.0, w2 = 0.0;
    double learningRate = 0.01;
    double dataset[4][3] = {{12.0, 1, 1}, {2.0, 10, 0}, {15.0, 2, 1}, {4.0, 12, 0}};

    for (int epoch = 0; epoch < 500; epoch++) {
        for (int i = 0; i < 4; i++) {
            double score = (dataset[i][0] * w1) + (dataset[i][1] * w2);
            int error = (int)dataset[i][2] - ((score > 1.0) ? 1 : 0);
            if (error != 0) {
                w1 += error * learningRate * dataset[i][0];
                w2 += error * learningRate * dataset[i][1];
            }
        }
    }
    cout << "AI Brain Trained. W1: " << w1 << " W2: " << w2 << endl;

    // --- NETWORK SETUP ---
    WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {AF_INET, htons(8080), INADDR_ANY};
    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 5);

    cout << "Listening for Python Vision data..." << endl;

    while (true) {
        SOCKET client = accept(server, NULL, NULL);
        char buffer[1024] = {0};
        if (recv(client, buffer, 1024, 0) > 0) {
            double livePeople = stod(buffer);
            // Decision: (10kW * w1) + (X people * w2)
            int ai_choice = ((10.0 * w1) + (livePeople * w2) > 1.0) ? 1 : 0;
            
            string resp = to_string(ai_choice);
            send(client, resp.c_str(), resp.length(), 0);
            cout << "People: " << livePeople << " -> " << (ai_choice ? "ECO" : "NORMAL") << endl;
        }
        closesocket(client);
    }
    return 0;
}
