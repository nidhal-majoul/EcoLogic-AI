#include <iostream>
#include <winsock2.h> 
#include <string>

#pragma comment(lib, "ws2_32.lib") 
using namespace std;

int main() {
    // --- STEP 1: AI TRAINING ---
    double w1 = 0.0, w2 = 0.0; 
    double learningRate = 0.01;

    double dataset[5][3] = {
        {12.0, 1, 1}, 
        {15.0, 2, 1},  
        {2.0, 10, 0},  
        {10.0, 0, 0},  
        {1.0, 0, 0}   
    };

    for (int epoch = 0; epoch < 1000; epoch++) {
        for (int i = 0; i < 5; i++) {
            double score = (dataset[i][0] * w1) + (dataset[i][1] * w2);
            int pred = (score > 1.0) ? 1 : 0; 
            int error = (int)dataset[i][2] - pred;
            if (error != 0) {
                w1 += error * learningRate * dataset[i][0];
                w2 += error * learningRate * dataset[i][1];
            }
        }
    }
    cout << "Model Trained. w1: " << w1 << " w2: " << w2 << endl;

    // --- STEP 2: NETWORK SETUP ---
    WSADATA wsa; 
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    
    // Correct way to initialize the address structure
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9999);

    if (bind(server, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        cout << "Bind failed" << endl;
        return 1;
    }

    listen(server, 5);
    cout << "Listening on port 9999..." << endl;

    // --- STEP 3: LIVE INFERENCE ---
    while (true) {
        SOCKET client = accept(server, NULL, NULL);
        if (client != INVALID_SOCKET) {
            char buffer[1024] = {0};
            int bytesReceived = recv(client, buffer, 1024, 0);
            
            if (bytesReceived > 0) {
                try {
                    // Convert buffer to string, then to double
                    string dataStr(buffer);
                    double livePeople = stod(dataStr); 
                    double liveEnergy = 10.0; 
                    
                    double liveScore = (liveEnergy * w1) + (livePeople * w2);
                    int ai_choice = (liveScore > 1.0) ? 1 : 0;
                    
                    string resp = to_string(ai_choice);
                    send(client, resp.c_str(), (int)resp.length(), 0);
                    
                    cout << "Detection: " << livePeople << " people -> Decision: " 
                         << (ai_choice == 1 ? "NORMAL" : "ECO") << endl;
                } catch (...) {
                    cout << "Received invalid data format" << endl;
                }
            }
            closesocket(client);
        }
    }

    closesocket(server);
    WSACleanup();
    return 0;
}
