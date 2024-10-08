# Final weekly v1.0
In this pull request i fixed the following issues i had in the previous weekly version:
- Added log activies of alerts and emergencies to server.log
- Fixed over 150 code smells detected by SonarQube
- Deleted unused commented code
- Added SQA scripts to the project
- Updated the gitignore file to not track the SQA scripts and SQ files

---

## JSON

For JSON parsing and manipulation, i leverage the [cJSON](https://github.com/DaveGamble/cJSON) library developed by Dave Gamble. With our updated setup, integration is handled seamlessly via CMake and FetchContent, eliminating the need for manual downloading and installation. Simply execute your CMake build process, and the library will be automatically fetched, linked, and included in your project.

Upon completion of the build process, the cJSON library will be available at /external/cJSON inside the project directory, enabling easy inclusion in your project using #include <cJSON.h>.

## IPCs

### Client
In developing our client-server application, I opted to utilize TCP exclusively, foregoing the use of UDP. This decision stems from several critical factors inherent in the nature of our application's requirements. TCP offers reliable, connection-oriented communication, ensuring that data packets are delivered in order and without loss, vital for tasks such as file transfer or database access where data integrity is paramount. Additionally, TCP's built-in error detection and retransmission mechanisms further guarantee robustness in data transmission, particularly essential for long-distance or unreliable network environments. While UDP boasts lower overhead and faster transmission speeds, its connectionless and unreliable nature make it unsuitable for our application, which demands dependable and ordered data delivery. Therefore, by harnessing TCP's reliability and resilience, i prioritize the seamless and secure exchange of information, enhancing the overall performance and user experience of our client-server system.

### Alerts and Emergencies
For the alerts and emergencies, i have decided to use FIFOs to communicate with the server. This decision is based on the fact that FIFOs are unidirectional, which means that the data is sent in one direction and received in the other, which is ideal for our case, since the alerts and emergencies are generated by the modules (as a server library) and sent to the server, respectively.

### Supplies
For the supplies, i decided to use shared memory to communicate with the server. Shared memory minimizes overhead, ensuring swift data exchange and seamless coordination. This approach fosters seamless coordination between server operations and JSON data manipulation, facilitating rapid access and modification without incurring extraneous computational costs

---
# Build the project
To build the project i made some bash scripts to automate the process. If you want to only build it and check for errors use:

```bash
./scripts/RUN_LAB.sh
```

But if you want to test the project, with multiple clients and one server, this bash script will open one terminal for the server and five terminals for the clients, beign one of the clients the ubuntu with user privileges.

```bash
./scripts/RUN_CLIENTS.sh
```

<p align="center">
  <img src="https://github.com/user-attachments/assets/57fda7b0-d8b2-4c70-b0da-7c535020102a"><br>
  <em>Fig 1. Image of multiple clients</em>
</p>

## Example of the program running
In this first gif we can see a normal execution of the program, where two **clients** are connected on the right column to the **server** on the middle, one as a normal client and the other as ubuntu. The user with privileges modifies the supplies database with the `modify` command, and the other user can see the changes in real time when he sends a `state` command. At the same time both users are receiving alerts from the server, and finally the connection is closed abruptly when and emergency is triggered.

On the left we can see the server.log registering the activities of the users, the alerts and the emergencies.

<p align="center">
  <img src="https://github.com/user-attachments/assets/d1c5bfbb-46c0-4d73-8f57-11037a2d4099"><br>
  <em>Fig 2. Gif of supplies</em>
</p>

Here we can see an example of the authentication process, where the user enters the password and the server checks if the credentials are valid. If the credentials are correct, the user is authenticated and can access the server, otherwise the user is disconnected. Only the user with privileges `ubuntu` can modify the supplies database.

Currently the server allows only two kinds of users `ubuntu ubuntu` and `hostname hostname` so any other combination of users credentials will be not be allowed to connect.

<p align="center">
  <img src="https://github.com/user-attachments/assets/29ad1cc1-9958-4a69-a00d-968eb9517965"><br>
  <em>Fig 3. Auth example</em>
</p>

## QA 
To ensure the quality of the code i used SonarQube to detect code smells and bugs in the code. I fixed over 150 code smells and bugs detected by SonarQube. The summary of the analysis can be seen in the following image.

There are still 20 minor issues to fix, but they are not critical to the program and will be fixed in the next version, most of them are related to variables not initialized with a value, and minor issues with `sscanf` or `strcat`.

Most critical issues were fixed, but 2 of them are still present and accepted in the `server.c` file, because the server handles the clients, supplies, alerts and emergency communications inside the same while loop that checks the correspondings sockets for activity. So im getting maintainability issues because of the complexity of the code block.

One thing to future versions is to improve the coverage of the tests, it currently sits a 8%. But being on the deadline of the 4th weekly version, i could not improve it.

<p align="center">
  <img src="https://github.com/user-attachments/assets/87ba13cb-2ea3-4b8d-975c-4a4ac9c11291"><br>
  <em>Fig 4. Sonarqube summary</em>
</p>


<p align="center">
  <img src="https://github.com/user-attachments/assets/43ee5dc6-6621-4c22-97b6-019532f4a212"><br>
  <em>Fig 5. Accepted issues</em>
</p>

---

## Added unit tests
Improved coverage on alerts, emergency and supplies libraries

<p align="center">
  <img src="https://github.com/user-attachments/assets/95819ba4-6383-42c0-8a6c-c62ba0b0564f"><br>
  <em>Fig 6. Unit tests.</em>
</p>

```bash
------------------------------------------------------------------------------
                           GCC Code Coverage Report
Directory: .
------------------------------------------------------------------------------
File                                       Lines    Exec  Cover   Missing
------------------------------------------------------------------------------
lib/libdyn/src/alerts.c                       34      12    35%   46,48,50,52,56,58-59,61-62,64,66,72,74-75,77-78,80,92-93,110,112-113
lib/libdyn/src/supplies.c                     97      72    74%   12-13,66-67,87-88,98,109-110,130-131,137-138,145-146,164-165,171-172,178-179,185-186,193-194
lib/libsta/src/emergency.c                    25       4    16%   3,5-6,8-9,11,19-20,24,26-27,29,31-32,34,36,39,44,46,48,50
tests/unit/test_unit.c                        90      88    97%   111,113
------------------------------------------------------------------------------
TOTAL                                        246     176    71%
------------------------------------------------------------------------------

```

---

<p align="center">
  <img src="https://github.com/user-attachments/assets/cbcb7477-4a0b-4818-81bf-42bb77dffc46" alt="Diagrama de secuencias"><br>
  <em>Fig 7. Sequence diagram of a running example</em>
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/649e1f80-4ba6-4d5c-8925-fdf5216ff252" alt="Diagrama de secuencias"><br>
  <em>Fig 8. Flowchart of the project</em>
</p>
